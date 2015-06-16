/**
  *****************************************************************************************
  * @file    stm32_cc3000.c 
  * @author  
  * @version V1.1.0
  * @date    06-June-2014
  * @brief   This source file contains all the user-defined functions to interact with the 
  *			 CC3000 module. It allows communication with the CC3000 module by utilizing
  *          the wlan API and netapp API functions, which would eventually call the HCI
  *			 functions, which in turn calls SPI functions that are defined in this source
  *			 file.
  *****************************************************************************************
*/

//*****************************************************************************
//
//! \addtogroup SMORES_wifi_custom_api
//! @{
//
//*****************************************************************************


/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "stm32_cc3000.h"
// NOTE: Required by cc3000 Host driver
unsigned char wlan_tx_buffer[CC3000_TX_BUFFER_SIZE];

// The magic number that resides at the end of the TX/RX buffer (1 byte after the allocated size)
// for the purpose of detection of the overrun. The location of the memory where the magic number
// resides shall never be written. In case it is written - the overrun occured and either recevie function
// or send function will stuck forever.
#define CC3000_BUFFER_MAGIC_NUMBER (0xDE)
#define HEADERS_SIZE_EVNT          (SPI_HEADER_SIZE + 5)
#define READ                       (3)
#define WRITE                      (1)
#define HI(value)                  (((value) & 0xFF00) >> 8)
#define LO(value)                  ((value) & 0x00FF)

//define type spi_state_t and its associated values to indicate the current SPI state
typedef enum {
  SPI_STATE_POWERUP,
  SPI_STATE_INITIALIZED,
  SPI_STATE_IDLE,
  SPI_STATE_WRITE_IRQ,
  SPI_STATE_WRITE_FIRST_PORTION,
  SPI_STATE_WRITE_EOT,
  SPI_STATE_READ_IRQ,
  SPI_STATE_READ_FIRST_PORTION,
  SPI_STATE_READ_EOT
} spi_state_t;

//Declare type spi_information_t as a structure with fields that indicate the transmit 
//and receive packet information
typedef struct {
  spi_rx_handler_t spi_rx_handler;
  unsigned short tx_packet_length;
  unsigned short rx_packet_length;
  spi_state_t state;
  unsigned char *tx_packet;
  unsigned char *rx_packet;
} spi_information_t;

//global variables
volatile int cc3000_connected;
volatile int cc3000_dhcp;
volatile int cc3000_dhcp_configured;
volatile int cc3000_smart_config_finished;
volatile int cc3000_stop_smart_config;
volatile int cc3000_ok_to_shutdown;
volatile int cc3000_ping_report_count;
volatile int cc3000_spi_irq_enabled;
volatile int cc3000_spi_is_in_irq;
volatile netapp_pingreport_args_t cc3000_ping_report;
uint8_t cc3000_closed_sockets[MAX_SOCKETS];
spi_information_t cc3000_spi_info;
uint8_t cc3000_spi_buffer[CC3000_RX_BUFFER_SIZE];

//static function calls
void cc3000_setup_vbat_en();
void cc3000_setup_spi();
void cc3000_setup_irq();

void cc3000_async_callback(long eventType, char* data, unsigned char length);
char* cc3000_send_FW_patch(unsigned long *length);
char* cc3000_send_driver_patch(unsigned long *length);
char* cc3000_send_bootloader_patch(unsigned long *length);
long cc3000_read_wlan_irq();
void cc3000_wlan_irq_enable();
void cc3000_wlan_irq_disable();
void cc3000_write_wlan_pin(unsigned char val);
void cc3000_irq_poll();
int cc3000_scan_ssids(uint32_t time);
int cc3000_connect_open(const char *ssid);
int cc3000_connect_secure(const char *ssid, const char *key, int32_t secMode);

void cc3000_spi_assert();
void cc3000_spi_deassert();
void cc3000_spi_read_header();
void cc3000_spi_cont_read();
int cc3000_spi_read_data_cont();
int cc3000_spi_first_write(unsigned char *ucBuf, unsigned short usLength);
void cc3000_spi_write_data_sync(unsigned char *data, unsigned short size);
void cc3000_spi_read_data_sync(unsigned char *data, unsigned short size);
void cc3000_spi_trigger_rx_processing();
uint8_t cc3000_spi_transfer(uint8_t d);
long start_ip = MOD_START_IP;
//------------Give the modules a static IP------------------------//
void cc3000_initialize_ip(int module_ip)
{
  module_ip = module_ip<<24;
  start_ip=start_ip+module_ip;
  #define STATIC_IP_ADDRESS     start_ip
  debug_write_line("Wrote static Ip"); 
  debug_write_ip(start_ip); 

}
//-----------------------------------------------------------------//
//**************************************************************************************
//
//!  cc3000_startup_check
//!
//!  @param    none
//!		
//!  @return   none
//!
//!  @brief    This function calls the cc3000_setup function to setup the neccessary 
//!			   registers to interface with the CC3000 SPI peripheral, then it confirms
//!			   the functionality of the SPI peripheral by checking the firmwareversion.
//!			   This function then deletes the previous connection profiles, requests 
//!			   for the pre-defined static IP address and connects to the desired access
//!			   point.
//
//**************************************************************************************
void cc3000_startup_check() {
  uint8_t cc3000MajorFirmwareVersion, cc3000MinorFirmwareVersion;

  RCC_ClearFlag(); // Clear reset flags

  cc3000_setup(0, 0);
  //This portion serves the purpose to verify the functionality of the SPI peripheral
  cc3000_get_firmware_version(&cc3000MajorFirmwareVersion, &cc3000MinorFirmwareVersion);
  debug_write("major: 0x");
  debug_write_u8(cc3000MajorFirmwareVersion, 16);
  debug_write_line("");
  debug_write("minor: 0x");
  debug_write_u8(cc3000MinorFirmwareVersion, 16);
  debug_write_line("");
  if (cc3000MajorFirmwareVersion != 0x01 || cc3000MinorFirmwareVersion != 0x18) {
    debug_write_line("Wrong firmware version!");
  //  while (1);
  }

  cc3000_display_mac_address();
  
  //Calls cc3000_delete_profiles function to delete the saved profile from  
  //previous connections
  debug_write_line("Deleting old connection profiles");
  if (cc3000_delete_profiles() != 0) {
    debug_write_line("Failed!");
    while (1);
  }


  //Requests for the predefined static IP address
#ifdef STATIC_IP_ADDRESS
  unsigned long aucIP = STATIC_IP_ADDRESS;//module_static_ip;  //defined in module_ip.h
  unsigned long aucSubnetMask = STATIC_SUBNET_MASK;
  unsigned long aucDefaultGateway = STATIC_DEFAULT_GATEWAY;
  unsigned long aucDNSServer = STATIC_DNS_SERVER;
  debug_write_line("setting up profile, gm"); 
  if (netapp_dhcp(&aucIP, &aucSubnetMask, &aucDefaultGateway, &aucDNSServer) != 0) {
    debug_write_line("netapp_dhcp Failed!");
    while (1);
  }

#else
  unsigned long aucIP = 0;
  unsigned long aucSubnetMask = 0;
  unsigned long aucDefaultGateway = 0;
  unsigned long aucDNSServer = 0;
  if (netapp_dhcp(&aucIP, &aucSubnetMask, &aucDefaultGateway, &aucDNSServer) != 0) {
    debug_write_line("netapp_dhcp Failed!");
    while (1);
  }
#endif

  // Attempt to connect to an access point
  char *ssid = WLAN_SSID; // Max 32 chars 
  debug_write("Attempting to connect to ");
  debug_write_line(ssid);

  // NOTE: Secure connections are not available in 'Tiny' mode!
  if (cc3000_connect_to_ap(WLAN_SSID, WLAN_PASS, WLAN_SECURITY) != 0) {
    debug_write_line("Connect Failed!");
    while (1);
  }

  debug_write_line("Connected!");

  //CAN BE DELETED?
#ifndef STATIC_IP_ADDRESS
  // Wait for DHCP to complete
  debug_write_line("Request DHCP");
  while (cc3000_check_dhcp() != 0) {
    mWaitms(100);
  }
#endif

  while (!cc3000_is_connected()) {
    mWaitms(100);
  }
  //Prints the IP and other information of the connection
  cc3000_display_ipconfig();
}

//**************************************************************************************
//
//!  cc3000_setup
//!
//!  @param    usPatchesAvailableAtHost  flag to indicate if patches available
//!                                      from host or from EEPROM. Due to the 
//!                                      fact the patches are burn to the EEPROM
//!                                      using the patch programmer utility, the 
//!                                      patches will be available from the EEPROM
//!                                      and not from the host.
//!	@param	   useSmartConfigData		 flag to indicate if SmartConfig is desired.
//!									     Note that this feature has not been tested.
//!		
//!  @return   0                         if setup is successful
//!
//!  @return   1                         if WLAN Set Event Mask failed
//!
//!  @return   2                         if connection attempt has timed out using 
//!                                      SmartConfig.
//!
//!  @brief    Initalizes the global parameters and calls the cc3000_setup_vbat_en, 
//!			   cc3000_setup_irq and cc3000_setup_spi functions to initialize the
//!			   neccessary registers to interface with the CC3000 SPI peripheral. This 
//!			   function also starts the WLAN connection. Note the 
//!			   the SmartConfig feature is included in the function but has not been 
//!			   tested. 
//
//**************************************************************************************
int cc3000_setup(int patchesAvailableAtHost, int useSmartConfigData) {
  debug_write_line("cc3000_setup");
  cc3000_connected = 0;
  cc3000_dhcp = 0;
  cc3000_dhcp_configured = 0;
  cc3000_smart_config_finished = 0;
  cc3000_stop_smart_config = 0;
  cc3000_ok_to_shutdown = 0;
  cc3000_ping_report_count = 0;
  cc3000_spi_irq_enabled = 0;
  cc3000_spi_is_in_irq = 0;
  for (int i = 0; i < MAX_SOCKETS; i++) {
    cc3000_closed_sockets[i] = 0;
  }

  cc3000_setup_vbat_en();
  cc3000_setup_irq();
  cc3000_setup_spi();
  cc3000_wlan_irq_enable();
     
  
  debug_write_line("wlan_init");
  wlan_init(
          cc3000_async_callback,
          cc3000_send_FW_patch,
          cc3000_send_driver_patch,
          cc3000_send_bootloader_patch,
          cc3000_read_wlan_irq,
          cc3000_wlan_irq_enable,
          cc3000_wlan_irq_disable,
          cc3000_write_wlan_pin);

  debug_write_line("ccc300 connected ?");
  debug_write_u8(cc3000_is_connected(),8);
  debug_write_line("wlan_start");
  wlan_start(patchesAvailableAtHost);

  // Check if we should erase previous stored connection details
  // (most likely written with data from the SmartConfig app)
  debug_write_line("wlan_ioctl");
  if (useSmartConfigData) {
    // Auto Connect - the C3000 device tries to connect to any AP it detects during scanning:
    // wlan_ioctl_set_connection_policy(1, 0, 0)

    // Fast Connect - the CC3000 device tries to reconnect to the last AP connected to:
    wlan_ioctl_set_connection_policy(0, 1, 0);

    // Use Profiles - the CC3000 device tries to connect to an AP from profiles:
    // wlan_ioctl_set_connection_policy(0, 0, 1);
  } else {
    // Manual connection only (no auto, profiles, etc.)
    wlan_ioctl_set_connection_policy(0, 0, 0);
    // Delete previous profiles from memory
    wlan_ioctl_del_profile(255);
  }

  if (wlan_set_event_mask(
          HCI_EVNT_WLAN_UNSOL_INIT |
          //HCI_EVNT_WLAN_ASYNC_PING_REPORT |// we want ping reports
          //HCI_EVNT_BSD_TCP_CLOSE_WAIT |
          //HCI_EVNT_WLAN_TX_COMPLETE |
          HCI_EVNT_WLAN_KEEPALIVE) != 0) {
    debug_write_line("WLAN Set Event Mask FAIL");
    return 1;
  }

  // Wait for re-connection is we're using SmartConfig data
  if (useSmartConfigData) {
    // Wait for a connection
    uint32_t timeout = 0;
    while (!cc3000_connected) {
      cc3000_irq_poll();
      if (timeout > WLAN_CONNECT_TIMEOUT_MS) {
        debug_write_line("Timed out using SmartConfig data");
      }
      return 2;
    }
    timeout += 10;
    mWaitms(10);
  }

  mWaitms(1000);
  if (cc3000_dhcp) {
    mdnsAdvertiser(1, (char *) cc3000_device_name, strlen(cc3000_device_name));
  }

  debug_write_line("END cc3000_setup");
  return 0;
}


//**************************************************************************************
//
//!  cc3000_setup_vbat_en
//!
//!  @param    none
//!   
//!  @return   none
//!
//!  @brief    Initalizes and configure the corresponding pin for the VBAT_EN
//!            (VBEN) pin for the CC3000 module.
//
//**************************************************************************************
void cc3000_setup_vbat_en() {
  debug_write_line("cc3000_setup_vbat_en");
  GPIO_InitTypeDef gpioConfig;

  RCC_APB2PeriphClockCmd(CC3000_VBAT_EN_RCC, ENABLE);
  gpioConfig.GPIO_Pin = CC3000_VBAT_EN_PIN;
  gpioConfig.GPIO_Mode = GPIO_Mode_OUT;
  gpioConfig.GPIO_OType = GPIO_OType_PP;
  gpioConfig.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(CC3000_VBAT_EN_PORT, &gpioConfig);

  GPIO_ResetBits(CC3000_VBAT_EN_PORT, CC3000_VBAT_EN_PIN);

  mWaitms(500);
}


//**************************************************************************************
//
//!  cc3000_setup_irq
//!
//!  @param    none
//!		
//!  @return   none
//!
//!  @brief    Initalizes and configure the corresponding pin for the external interrupt
//!		      (IRQ) pin for the CC3000 module.
//
//**************************************************************************************
void cc3000_setup_irq() {
  debug_write_line("cc3000_setup_irq");
  RCC_AHBPeriphClockCmd(CC3000_IRQ_RCC, ENABLE);

  GPIO_InitTypeDef gpioConfig;
  gpioConfig.GPIO_Pin = CC3000_IRQ_PIN;
  gpioConfig.GPIO_Mode = GPIO_Mode_IN;
  gpioConfig.GPIO_PuPd = GPIO_PuPd_NOPULL;
  gpioConfig.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(CC3000_IRQ_PORT, &gpioConfig);

  /* Enable SYSCFG clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  
  /* Connect EXTI0 Line to PA2 pin */
  SYSCFG_EXTILineConfig(CC3000_IRQ_EXTI_PORT,CC3000_IRQ_PIN);

  // Configure EXTI line
  EXTI_InitTypeDef extiConfig;
  extiConfig.EXTI_Line = CC3000_IRQ_EXTI_LINE;
  extiConfig.EXTI_Mode = EXTI_Mode_Interrupt;
  extiConfig.EXTI_Trigger = EXTI_Trigger_Falling;
  extiConfig.EXTI_LineCmd = ENABLE;
  EXTI_Init(&extiConfig);
}

//***************************************************************************************************************
//
//!  cc3000_setup_spi
//!
//!  @param    none
//!		
//!  @return   none
//!
//!  @brief    Initalizes and configure the corresponding pins for the SPI peripheral 
//! 		   used to interact with the CC3000 module.
//
//***************************************************************************************************************
void cc3000_setup_spi() {
  debug_write_line("cc3000_setup_spi");
  GPIO_InitTypeDef gpioConfig;
  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_APB2PeriphClockCmd(CC3000_SPI_CLK, ENABLE);

  //Chip select configuration
  cc3000_spi_deassert(); // set pin high before initializing as output pin to not false trigger CS
  RCC_AHBPeriphClockCmd(CC3000_CS_RCC, ENABLE);
  gpioConfig.GPIO_Pin = CC3000_CS_PIN;
  gpioConfig.GPIO_Mode = GPIO_Mode_OUT;
  gpioConfig.GPIO_OType = GPIO_OType_PP;
  gpioConfig.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(CC3000_CS_PORT, &gpioConfig);
  cc3000_spi_deassert();
  
  RCC_APB2PeriphClockCmd(CC3000_SPI_CLK, ENABLE);
  
  // SPI Pin Definition
  GPIO_PinAFConfig(CC3000_SCK_GPIO_PORT, CC3000_SCK_GPIO_SOURCE, CC3000_SCK_AF);    
  GPIO_PinAFConfig(CC3000_MISO_GPIO_PORT, CC3000_MISO_GPIO_SOURCE, CC3000_MISO_AF); 
  GPIO_PinAFConfig(CC3000_MOSI_GPIO_PORT, CC3000_MOSI_GPIO_SOURCE, CC3000_MOSI_AF);
  //This following line is commented out since the chip select pin is being controlled in the program.
  // GPIO_PinAFConfig(GPIOA, GPIO_PinSource4, GPIO_AF_5);   //Controlled by cc3000_spi_deassert/assert function
  
  //GPIO initialization for the MISO, MOSI and SCK pins of the SPI peripheral
  GPIO_InitStructure.GPIO_Pin = CC3000_SCK_GPIO_PIN | CC3000_MISO_GPIO_PIN | CC3000_MOSI_GPIO_PIN;   
  GPIO_InitStructure.GPIO_Mode = CC3000_SPI_GPIO_MODE;
  GPIO_InitStructure.GPIO_Speed = CC3000_SPI_GPIO_SPEED;
  GPIO_InitStructure.GPIO_OType = CC3000_SPI_GPIO_OTYPE;
  GPIO_InitStructure.GPIO_PuPd = CC3000_SPI_GPIO_PUPD;
  //MISO, MOSI and SCK pins for SP1 share the same GPIO port.
  GPIO_Init(CC3000_MISO_GPIO_PORT,&GPIO_InitStructure);  

  //SPI configuration for the MISO, MOSI and SCK pins of the SPI peripheral
  SPI_InitTypeDef spiConfig;
  SPI_StructInit(&spiConfig);
  spiConfig.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  spiConfig.SPI_Mode = SPI_Mode_Master;
  spiConfig.SPI_DataSize = SPI_DataSize_8b;
  spiConfig.SPI_NSS = SPI_NSS_Soft;
  // May need to decrease SPI speed later
  spiConfig.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2; 
  spiConfig.SPI_FirstBit = SPI_FirstBit_MSB;

  // Clock is low upon the assertion of the chip select and data is sampled on falling edge
  spiConfig.SPI_CPOL = SPI_CPOL_Low;
  spiConfig.SPI_CPHA = SPI_CPHA_2Edge;
  
  //Assign the defined configuration to the register and enable the SPI 
  SPI_Init(CC3000_SPI, &spiConfig);
  SPI_RxFIFOThresholdConfig(CC3000_SPI, SPI_RxFIFOThreshold_QF);
  SPI_Cmd(CC3000_SPI, ENABLE);
  
}

//**************************************************************************************
//
//!  cc3000_get_firmware_version
//!
//!  @param    major  used to store the MajorFirmwareVersion
//!	 @param	   minor  used to store the MajorFirmwareVersion
//!		
//!  @return   0 if the information if successfully read from the nonvolatile 
//!				 memory of the CC3000 module		
//!	@return	   1 otherwise
//!
//!  @brief    Stores the firmware information on paramters "major" and "minor"
//
//**************************************************************************************
int cc3000_get_firmware_version(uint8_t *major, uint8_t *minor) {
  uint8_t fwpReturn[2];

  debug_write_line("cc3000_get_firmware_version");
  if (nvmem_read_sp_version(fwpReturn) != 0) {
    debug_write_line("Unable to read the firmware version");
    return 1;
  }

  *major = fwpReturn[0];
  *minor = fwpReturn[1];

  return 0;
}


//**************************************************************************************
//
//!  cc3000_async_callback
//!
//!  @param    eventType  event type
//!	 @param		   data		  data
//!	 @param		   length	  data length
//!		
//!  @return   none
//!
//!  @brief    This function is called by the API function, namely 
//!			   hci_unsol_event_handler when an unsolicited event has occured, it serves 
//!			   the purpose to update some of the global variables.
//
//**************************************************************************************
void cc3000_async_callback(long eventType, char* data, unsigned char length) {
  if (eventType == HCI_EVNT_WLAN_ASYNC_SIMPLE_CONFIG_DONE) {
    cc3000_smart_config_finished = 1;
    cc3000_stop_smart_config = 1;
  }

  if (eventType == HCI_EVNT_WLAN_UNSOL_CONNECT) {
    cc3000_connected = 1;
  }

  if (eventType == HCI_EVNT_WLAN_UNSOL_DISCONNECT) {
    cc3000_connected = 0;
    cc3000_dhcp = 0;
    cc3000_dhcp_configured = 0;
  }

  if (eventType == HCI_EVNT_WLAN_UNSOL_DHCP) {
    cc3000_dhcp = 1;
  }

  if (eventType == HCI_EVENT_CC3000_CAN_SHUT_DOWN) {
    cc3000_ok_to_shutdown = 1;
  }

  if (eventType == HCI_EVNT_WLAN_ASYNC_PING_REPORT) {
    //PRINT_F("CC3000: Ping report\n\r");
    cc3000_ping_report_count++;
    memcpy((uint8_t*) & cc3000_ping_report, data, length);
  }

  if (eventType == HCI_EVNT_BSD_TCP_CLOSE_WAIT) {
    uint8_t socketnum;
    socketnum = data[0];
    if (socketnum < MAX_SOCKETS) {
      cc3000_closed_sockets[socketnum] = 1;
    }
  }
}

char* cc3000_send_FW_patch(unsigned long *length) {
  mYellowON;
  *length = 0;
  return NULL;
}

char* cc3000_send_driver_patch(unsigned long *length) {
  *length = 0;
  return NULL;
}

char* cc3000_send_bootloader_patch(unsigned long *length) {
  *length = 0;
  return NULL;
}

//**************************************************************************************
//
//!  cc3000_read_wlan_irq
//!
//!  @param    none
//!		
//!  @return   0 if the IRQ pin on CC3000 is low		
//!	 @return   1 otherwise
//!
//!  @brief    Used to read the IRQ pin on CC3000
//
//**************************************************************************************
long cc3000_read_wlan_irq() {
  return GPIO_ReadInputDataBit(CC3000_IRQ_PORT, CC3000_IRQ_PIN) == Bit_SET ? 1 : 0;
}

//**************************************************************************************
//
//!  cc3000_wlan_irq_enable
//!
//!  @param    none
//!		
//!  @return   none
//!
//!  @brief    Used to enable the external interrupt pin which is connected to
//! 		   the (IRQ pin from CC3000)
//
//**************************************************************************************
void cc3000_wlan_irq_enable() {
  // debug_write_line("cc3000_wlan_irq_enable");

  cc3000_spi_irq_enabled = 1;

  NVIC_InitTypeDef nvicConfig;
  nvicConfig.NVIC_IRQChannel = CC3000_IRQ_EXTI_CH;
  nvicConfig.NVIC_IRQChannelPreemptionPriority = 0x0F;
  nvicConfig.NVIC_IRQChannelSubPriority = 0x0F;
  nvicConfig.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&nvicConfig);
}

//**************************************************************************************
//
//!  cc3000_wlan_irq_disable
//!
//!  @param    none
//!		
//!  @return   none
//!
//!  @brief    Used to disable the external interrupt pin which is connected to
//! 		   the (IRQ pin from CC3000)
//
//**************************************************************************************
void cc3000_wlan_irq_disable() {
  // debug_write_line("cc3000_wlan_irq_disable");

  cc3000_spi_irq_enabled = 0;

  NVIC_InitTypeDef nvicConfig;
  nvicConfig.NVIC_IRQChannel = CC3000_IRQ_EXTI_CH;
  nvicConfig.NVIC_IRQChannelPreemptionPriority = 0x0F;
  nvicConfig.NVIC_IRQChannelSubPriority = 0x0F;
  nvicConfig.NVIC_IRQChannelCmd = DISABLE;
  NVIC_Init(&nvicConfig);
}

//**************************************************************************************
//
//!  cc3000_write_wlan_pin
//!
//!  @param    none
//!		
//!  @return   none
//!
//!  @brief    Used to write the GPIO pin that is connected to VBEN pin of the CC3000 
//!			   module. Note that this pin should be logic high as indicated on the 
//!			   datasheet
//
//**************************************************************************************
void cc3000_write_wlan_pin(unsigned char val) {
  if (val) {
    GPIO_SetBits(CC3000_VBAT_EN_PORT, CC3000_VBAT_EN_PIN);
  } else {
    GPIO_ResetBits(CC3000_VBAT_EN_PORT, CC3000_VBAT_EN_PIN);
  }
}

//**************************************************************************************
//
//!  cc3000_irq_poll
//!
//!  @param    none
//!		
//!  @return   none
//!
//!  @brief    This function calls the cc3000_irq function only if the IRQ pin is low,
//!			   the SPI is not in the interrupt mode and that the CC3000_SPI_IRQ is 
//!			   enabled
//
//**************************************************************************************
void cc3000_irq_poll() {
  if (cc3000_read_wlan_irq() == 0 && cc3000_spi_is_in_irq == 0 && cc3000_spi_irq_enabled != 0) {
    cc3000_irq();
  }
}

//**************************************************************************************
//
//!  cc3000_spi_read_header
//!
//!  @param    none
//!		
//!  @return   none
//!
//!  @brief    This function stores the 10-byte received data (5 SPI header + 5 HCI 
//!            header) in the cc3000_spi_info.rx_packet location.
//!			    
//
//**************************************************************************************
void cc3000_spi_read_header() {
  cc3000_spi_read_data_sync(cc3000_spi_info.rx_packet, 10);
}

//**************************************************************************************
//
//!  cc3000_spi_read_data_sync
//!
//!  @param    data	pointer to the location where the received data would be stored.
//!	 @param	   size indicates how many bytes shold be read from the CC3000 module
//!		
//!  @return   none
//!
//!  @brief    This function first sends the opcode for read operation to the CC3000 
//!			   module and stores the header information in the location "data"
//
//**************************************************************************************
void cc3000_spi_read_data_sync(unsigned char *data, unsigned short size) {
  int i = 0;
  for (i = 0; i < size; i++) {
    data[i] = cc3000_spi_transfer(0x03);
  }

  //  debug_write("spi read: ");
  //  debug_write_u8_array(data, size);
  //  debug_write_line("");
}

//**************************************************************************************
//
//!  cc3000_spi_cont_read
//!
//!  @param    none
//!		
//!  @return   none
//!
//!  @brief    This function normally called after the header had been read. It calls 
//!			   the cc3000_spi_read_data_cont function to receive the rest of the packet  
//!			   and then calls cc3000_spi_trigger_rx_processing function to update the
//!			   buffer.
//
//**************************************************************************************
void cc3000_spi_cont_read() {
  // The header was read - continue with the payload read
  if (!cc3000_spi_read_data_cont()) {
    // All the data was read - finalize handling by switching to the task and calling from task Event Handler
    cc3000_spi_trigger_rx_processing();
  }
}
//**************************************************************************************
//
//!  cc3000_spi_trigger_rx_processing
//!
//!  @param    none
//!		
//!  @return   none
//!
//!  @brief    This function disables the IRQ pin and disselect the CC3000 chip, then 
//!			   it updates the received buffer position.
//!
//!	 @Warning  This function hangs up the program if the magic number is reached (buffer 
//!			   overrun occurred)
//
//**************************************************************************************
void cc3000_spi_trigger_rx_processing() {
  // Trigger Rx processing
  cc3000_wlan_irq_disable();
  cc3000_spi_deassert();

  // The magic number that resides at the end of the TX/RX buffer (1 byte after the allocated size)
  // for the purpose of detection of the overrun. If the magic number is overriten - buffer overrun
  // occurred - and we will stuck here forever!
  if (cc3000_spi_info.rx_packet[CC3000_RX_BUFFER_SIZE - 1] != CC3000_BUFFER_MAGIC_NUMBER) {
    // You've got problems if you're here!
    debug_write_line("CC3000: ERROR - magic number missing!");
    while (1);
  }

  cc3000_spi_info.state = SPI_STATE_IDLE;
  cc3000_spi_info.spi_rx_handler(cc3000_spi_info.rx_packet + SPI_HEADER_SIZE);
}
//**************************************************************************************
//
//!  cc3000_spi_read_data_cont
//!
//!  @param    none
//!		
//!  @return   0 at the end of the function
//!
//!  @brief    This function determines the number of bytes to receive from the header
//!			   obtained and retrieve the rest of the data by calling the 
//!			   cc3000_spi_read_data_sync function.
//
//**************************************************************************************
int cc3000_spi_read_data_cont() {
  int data_to_recv;
  unsigned char *evnt_buff, type;

  // debug_write_line("cc3000_spi_read_data_cont");

  // Determine what type of packet we have
  evnt_buff = cc3000_spi_info.rx_packet;
  data_to_recv = 0;
  STREAM_TO_UINT8((uint8_t *) (evnt_buff + SPI_HEADER_SIZE), HCI_PACKET_TYPE_OFFSET, type);

  switch (type) {
    case HCI_TYPE_DATA: // 0x02
      //      debug_write_line("HCI_TYPE_DATA");

      // We need to read the rest of data..
      STREAM_TO_UINT16((char *) (evnt_buff + SPI_HEADER_SIZE), HCI_DATA_LENGTH_OFFSET, data_to_recv);
      if (!((HEADERS_SIZE_EVNT + data_to_recv) & 1)) {
        data_to_recv++;
      }

      if (data_to_recv) {
        cc3000_spi_read_data_sync(evnt_buff + 10, data_to_recv);
      }
      break;

    case HCI_TYPE_EVNT: // 0x04
      //      debug_write_line("HCI_TYPE_EVNT");

      // Calculate the rest length of the data
      STREAM_TO_UINT8((char *) (evnt_buff + SPI_HEADER_SIZE), HCI_EVENT_LENGTH_OFFSET, data_to_recv);
      data_to_recv -= 1;

      // Add padding byte if needed
      if ((HEADERS_SIZE_EVNT + data_to_recv) & 1) {
        data_to_recv++;
      }

      if (data_to_recv) {
        cc3000_spi_read_data_sync(evnt_buff + 10, data_to_recv);
      }

      cc3000_spi_info.state = SPI_STATE_READ_EOT;
      break;

    default:
      debug_write("unknown type: ");
      debug_write_u8(type, 16);
      debug_write_line("");
      break;
  }

  return 0;
}

//**************************************************************************************
//
//!  cc3000_spi_write_data_sync
//!
//!  @param    data      pointer to the location where the data should be sent.
//!	 @param	   size      determines the number of bytes to transfer.
//!		
//!  @return   none
//!
//!  @brief    This function writes "size" bytes starting from the "data" location to 
//!			   the CC3000 module
//
//**************************************************************************************
void cc3000_spi_write_data_sync(unsigned char *data, unsigned short size) {
  uint8_t loc;

   // debug_write("data size ");
   // debug_write_u8_array(data, size);
  //  debug_write_line("");

  for (loc = 0; loc < size; loc++) {
    cc3000_spi_transfer(data[loc]);
  }

}

//**************************************************************************************
//
//!  cc3000_irq
//!
//!  @param    none
//!		
//!  @return   none
//!
//!  @brief    This function is called when the external interrupt (IRQ) pin from the 
//!            CC3000 module is triggered. It signals the MCU to read/write the data 
//!			   from/to the CC3000 module and this function retrieves/sends the data.
//!			   Note that the IRQ would also be triggered upon power up, when this
//! 		   is the case, the MCU updates the SPI state instead.
//! 		   
//
//**************************************************************************************
void cc3000_irq() {
  mWaitus(10);
  debug_write_line("cc3000_irq");
  //  debug_led_set(1);
  cc3000_spi_is_in_irq = 1;

  if (cc3000_spi_info.state == SPI_STATE_POWERUP) {
    // IRQ line was low ... perform a callback on the HCI Layer
    cc3000_spi_info.state = SPI_STATE_INITIALIZED;
  } else if (cc3000_spi_info.state == SPI_STATE_IDLE) {
    cc3000_spi_info.state = SPI_STATE_READ_IRQ;

    // IRQ line goes down - start reception
    cc3000_spi_assert();

    // Wait for TX/RX Compete which will come as DMA interrupt
    cc3000_spi_read_header();
    cc3000_spi_info.state = SPI_STATE_READ_EOT;
    cc3000_spi_cont_read();
  } else if (cc3000_spi_info.state == SPI_STATE_WRITE_IRQ) {
    cc3000_spi_write_data_sync(cc3000_spi_info.tx_packet, cc3000_spi_info.tx_packet_length);
    cc3000_spi_info.state = SPI_STATE_IDLE;

    cc3000_spi_deassert();
  }

  cc3000_spi_is_in_irq = 0;
  //  debug_led_set(0);
}

//**************************************************************************************
//
//!  cc3000_spi_assert
//!
//!  @param    none
//!		
//!  @return   none
//!
//!  @brief    This function asserts the CS pin on the MCU.
//
//**************************************************************************************
void cc3000_spi_assert() {
  GPIO_ResetBits(CC3000_CS_PORT, CC3000_CS_PIN);
}

//**************************************************************************************
//
//!  cc3000_spi_deassert
//!
//!  @param    none
//!		
//!  @return   none
//!
//!  @brief    This function deasserts the CS pin on the MCU.
//
//**************************************************************************************
void cc3000_spi_deassert() {
  GPIO_SetBits(CC3000_CS_PORT, CC3000_CS_PIN);
}

//**************************************************************************************
//
//!  cc3000_spi_transfer
//!
//!  @param    none
//!		
//!  @return   8 bit value received from the MISO pin.
//!
//!  @brief    This function shifts the 8 bit value "d" out of the MOSI pin and does 
//! 		   not return until both the transmit buffer is empty and the received 
//!			   buffer is not empty.
//
//**************************************************************************************
uint8_t cc3000_spi_transfer(uint8_t d) {
  // uint8_t recieved_data;
  SPI_SendData8(CC3000_SPI, d);
  while (SPI_I2S_GetFlagStatus(CC3000_SPI, SPI_I2S_FLAG_TXE) == RESET);
  while (SPI_I2S_GetFlagStatus(CC3000_SPI, SPI_I2S_FLAG_RXNE) == RESET);
  return SPI_ReceiveData8(CC3000_SPI);
  
}

//**************************************************************************************
//
/**
 *   NOTE: Required by cc3000 Host driver
 */
//!  SpiOpen
//!
//!  @param    rx_handler
//!		
//!  @return   none
//!
//!  @brief    This function initializes SPI hardware and registers the SPI RX handler 
//!	 		   that is called on each packet received from the CC3000 device.
//
//**************************************************************************************
void SpiOpen(spi_rx_handler_t rx_handler) {
  debug_write_line("SpiOpen");

  cc3000_spi_info.state = SPI_STATE_POWERUP;

  memset(cc3000_spi_buffer, 0, sizeof (cc3000_spi_buffer));
  memset(wlan_tx_buffer, 0, sizeof (wlan_tx_buffer));

  cc3000_spi_info.spi_rx_handler = rx_handler;
  cc3000_spi_info.tx_packet_length = 0;
  cc3000_spi_info.tx_packet = NULL;
  cc3000_spi_info.rx_packet = cc3000_spi_buffer;
  cc3000_spi_info.rx_packet_length = 0;

  cc3000_spi_buffer[CC3000_RX_BUFFER_SIZE - 1] = CC3000_BUFFER_MAGIC_NUMBER;
  wlan_tx_buffer[CC3000_TX_BUFFER_SIZE - 1] = CC3000_BUFFER_MAGIC_NUMBER;
  
  // Enable interrupt on the GPIO pin of WLAN IRQ
  tSLInformation.WlanInterruptEnable();

  debug_write_line("END SpiOpen");
}

//**************************************************************************************
//
/**
 *   NOTE: Required by cc3000 Host driver
 */
//!  SpiClose
//!
//!  @param    none
//!		
//!  @return   none
//!
//!  @brief    This function has not been implemented and seems not to have an impact
//! 		   in running the codes.
//
//**************************************************************************************
void SpiClose() {
  debug_write_line("TODO SpiClose"); // TODO SpiClose
}

//**************************************************************************************
//
/**
 *   NOTE: Required by cc3000 Host driver
 */
//!  SpiResumeSpi
//!
//!  @param    none
//!		
//!  @return   none
//!
//!  @brief    This function calls the cc3000_wlan_irq_enable function to enable the 
//!			   external interrupt.
//
//**************************************************************************************
void SpiResumeSpi() {
  cc3000_wlan_irq_enable();
}

//**************************************************************************************
//
/**
 *   NOTE: Required by cc3000 Host driver
 */
//!  SpiWrite
//!
//!  @param    pUserBuffer		address of the data (packet) to be transmitted.
//!
//!	 @param		   usLength			number of bytes of the packet to be transmitted
//!
//!  @return   none
//!
//!  @brief    Depending on the state field of the cc3000_spi_info variable, this 
//!			   either calls the cc3000_spi_first_write function or calls the 
//!			   cc3000_spi_write_data_sync function to transmit "usLength" bytes of data
//!			   from the location "pUserBuffer" to the CC3000 module.
//
//**************************************************************************************
long SpiWrite(unsigned char *pUserBuffer, unsigned short usLength) {
  unsigned char ucPad = 0;
  // debug_write_line("SPI WRITE FUNCTION");
  // Figure out the total length of the packet in order to figure out if there is padding or not
  if (!(usLength & 0x0001)) {
    ucPad++;
  }

  pUserBuffer[0] = WRITE;
  pUserBuffer[1] = HI(usLength + ucPad);
  pUserBuffer[2] = LO(usLength + ucPad);
  pUserBuffer[3] = 0;
  pUserBuffer[4] = 0;

  usLength += (SPI_HEADER_SIZE + ucPad);

  // The magic number that resides at the end of the TX/RX buffer (1 byte after the allocated size)
  // for the purpose of overrun detection. If the magic number is overwritten - buffer overrun
  // occurred - and we will be stuck here forever! 
  if (wlan_tx_buffer[CC3000_TX_BUFFER_SIZE - 1] != CC3000_BUFFER_MAGIC_NUMBER) {
    debug_write_line("CC3000: Error - No magic number found in SpiWrite");
    while (1);
  }

  if (cc3000_spi_info.state == SPI_STATE_POWERUP) {
    while (cc3000_spi_info.state != SPI_STATE_INITIALIZED);
  }

  if (cc3000_spi_info.state == SPI_STATE_INITIALIZED) {
    // This is time for first TX/RX transactions over SPI: the IRQ is down - so need to send read buffer size command
    cc3000_spi_first_write(pUserBuffer, usLength);
  } else {
    // We need to prevent here race that can occur in case two back to back packets are sent to the
    // device, so the state will move to IDLE and once again to not IDLE due to IRQ
    tSLInformation.WlanInterruptDisable();
    //debug_write_line("BE4 SPI STATE CHECK");
    while (cc3000_spi_info.state != SPI_STATE_IDLE);
    //debug_write_line("AFTER SPI STATE CHECK");
    cc3000_spi_info.state = SPI_STATE_WRITE_IRQ;
    cc3000_spi_info.tx_packet = pUserBuffer;
    cc3000_spi_info.tx_packet_length = usLength;

    // Assert the CS line and wait till SSI IRQ line is active and then initialize write operation
    cc3000_spi_assert();

    // Re-enable IRQ - if it was not disabled - this is not a problem...
    tSLInformation.WlanInterruptEnable();

    // Check for a missing interrupt between the CS assertion and enabling back the interrupts
    if (tSLInformation.ReadWlanInterruptPin() == 0) {
      // mGreenON;
      cc3000_spi_write_data_sync(cc3000_spi_info.tx_packet, cc3000_spi_info.tx_packet_length);

      cc3000_spi_info.state = SPI_STATE_IDLE;

      cc3000_spi_deassert();
    }

  }

  // Due to the fact that we are currently implementing a blocking situation
  // here we will wait till end of transaction */
  while (SPI_STATE_IDLE != cc3000_spi_info.state);
  return (0);
}

//**************************************************************************************
//
//!  cc3000_spi_first_write
//!
//!  @param    ucBuf		    address of the data (packet) to be transmitted.
//!
//!	 @param		   usLength			number of bytes of the packet to be transmitted
//!
//!  @return   none
//!
//!  @brief    As indicated in the CC3000 datasheet, this function first calls
//!			   the cc3000_spi_write_data_sync function to send 4 bytes(header) of
//!			   the packet to the CC3000, waits for at least 50us, then calls the
//!			   cc3000_spi_write_data_sync function again to transmit the rest of the 
//!			   packet
//
//**************************************************************************************
int cc3000_spi_first_write(unsigned char *ucBuf, unsigned short usLength) {
  // Workaround for the first transaction
  cc3000_spi_assert();
  // delay (stay low) for ~50us
  mWaitms(1); // TODO: 1ms really?

  // SPI writes first 4 bytes of data
  cc3000_spi_write_data_sync(ucBuf, 4);

  mWaitms(1);

  cc3000_spi_write_data_sync(ucBuf + 4, usLength - 4);

  /* From this point on - operate in a regular manner */
  cc3000_spi_info.state = SPI_STATE_IDLE;

  cc3000_spi_deassert();
  

  return (0);
}

//**************************************************************************************
//
//!  cc3000_get_mac_address
//!
//!  @param    addr		    address of the machine address in the nonvolatile memory of
//!	          						the CC3000 module
//!
//!  @return   none
//!
//!  @brief    This function calls the nvmem_read function from the nvmem API and 
//!            returns the data in the location "addr" of the nonvolatile memory
//!			   of the CC3000 module as an integer.
//
//**************************************************************************************
int cc3000_get_mac_address(uint8_t *addr) {
  debug_write_line("cc3000_get_mac_address");
  return nvmem_read(NVMEM_MAC_FILEID, 6, 0, addr);
}

//**************************************************************************************
//
//!  cc3000_delete_profiles
//!
//!  @param    none
//!
//!  @return       0		if the function successfully deletes the profiles.
//!	 @return		   1		if the function fails to set the connection policy.
//!	 @return		   2		if the function fails to delete the profiles.
//!
//!  @brief    This function calls the WLAN API functions to delete all the exisiting 
//!			   connection profiles.
//
//**************************************************************************************
int cc3000_delete_profiles() {
  if (wlan_ioctl_set_connection_policy(0, 0, 0) != 0) {
    debug_write_line("Failed to set up connection policy");
    return 1;
  }

  if (wlan_ioctl_del_profile(255) != 0) {
    debug_write_line("Failed deleting profiles");
    return 2;
  }

  return 0;
}

//***************************************************************************************
//
//!  cc3000_connect_to_ap
//!
//!  @param    ssid		pointer to the SSID value of the router
//!	 @param	   key		pointer to the key value (password) of the router
//!	 @param  secmode  value that indicates the type of security mode for the connection
//!
//!  @return   0		at the end of the function.
//!			   
//!
//!  @brief    Depending on the security mode and the length of the key, this function 
//!			   either calls the cc3000_connect_open function to connect to an open 
//!			   network or the cc3000_connect_secure function to connect to an secured 
//!			   network using the given information.
//
//***************************************************************************************
int cc3000_connect_to_ap(const char *ssid, const char *key, uint8_t secmode) {
  int16_t timer;

  do {
    cc3000_irq_poll();

    // Setup a 4 second SSID scan
    cc3000_scan_ssids(4000);

    // Wait for results
    mWaitms(4500);

    cc3000_scan_ssids(0);

    // Attempt to connect to an access point
    if ((secmode == 0) || (strlen(key) == 0)) {
      // Connect to an unsecured network
      if (cc3000_connect_open(ssid) != 0) {
        debug_write_line("Fail!");
        continue;
      }
    } else {
      // NOTE: Secure connections are not available in 'Tiny' mode!
#ifndef CC3000_TINY_DRIVER
      // Connect to a secure network using WPA2, etc
      if(cc3000_connect_secure(ssid,key,secmode) ==0)
      {
        debug_write_line("cc3000_connect_secure no error");
      }
      if (cc3000_connect_secure(ssid, key, secmode) != 0) {
        debug_write_line("Fail!");
        continue;
      }
#endif
    }

    timer = WLAN_CONNECT_TIMEOUT_MS;

    // Wait around a bit for the async connected signal to arrive or timeout
    debug_write_line("Waiting to connect...");
    while ((timer > 0) && !cc3000_connected) {
      debug_write_line("cc3000 not connected still in connect_to_ap"); 
      cc3000_irq_poll();
      mWaitms(10);
      timer -= 10;
    }
    if (timer <= 0) {
      debug_write_line("Timed out!");
    }
  } while (!cc3000_connected);

  return 0;
}

//***************************************************************************************
//
//!  cc3000_scan_ssids
//!
//!  @param    time		start/stop application scan: 
//!                     1 = start scan with default interval value of 10 min. 
//!           		    in order to set a different scan interval value apply the value 
//!                     in milliseconds. minimum 1 second. 0=stop). Wlan reset
//!                    (wlan_stop() wlan_start()) is needed when changing scan interval
//!                     value. Saved: No
//!
//!  @return   On success, zero is returned. On error, -1 is returned   
//!			   
//!
//!  @brief    This functions call the wlan_ioctl_set_scan_params function from the 
//!			   WLAN API and start and stop scan procedure. Set scan parameters.
//
//***************************************************************************************
int cc3000_scan_ssids(uint32_t time) {
  const unsigned long intervalTime[16] = {
    2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000,
    2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000
  };

  // We can abort a scan with a time of 0
  if (time) {
    debug_write_line("Started AP/SSID scan");
  }
   debug_write_line(" wlan ioctl set scan params returned"); 
   debug_write_u8 (wlan_ioctl_set_scan_params(time, 20, 100, 5, 0x7FF, -120, 0, 300, (unsigned long *) &intervalTime),8);
  return wlan_ioctl_set_scan_params(time, 20, 100, 5, 0x7FF, -120, 0, 300, (unsigned long *) &intervalTime);
}

//***************************************************************************************
//
//!  cc3000_connect_open
//!
//!  @param    ssid		pointer to the SSID value of the router
//!                     
//!  @return   On success, 0 is returned.   
//!			      If it failed to set connection policy, 1 is returned. 
//!			       Otherwise, 2 is returned.
//!
//!  @brief    This functions calls the WLAN API functions and connect the CC3000 module
//!			   to the AP with the given SSID.
//
//***************************************************************************************
int cc3000_connect_open(const char *ssid) {
#ifndef CC3000_TINY_DRIVER
  if (wlan_ioctl_set_connection_policy(0, 0, 0) != 0) {
    debug_write_line("Failed to set connection policy");
    return 1;
  }
  mWaitms(500);
  if (wlan_connect(WLAN_SEC_UNSEC, (char*) ssid, strlen(ssid), 0, NULL, 0) != 0) {
    debug_write_line("SSID connection failed");
    return 2;
  }
  return 0;

#else
  return wlan_connect(ssid, ssidLen);
#endif  
}

//***************************************************************************************
//
//!  cc3000_connect_secure
//!
//!  @param    ssid		pointer to the SSID value of the router
//!	 @param	   key		pointer to the key value (password) of the router
//!	 @param  secmode  value that indicates the type of security mode for the connection
//!                     
//!  @return   On success, zero is returned.   
//!	 @return   If the security mode is invalid, one is returned.
//!	 @return   If the SSID exceeds maximum length, two is returned.
//!	 @return   If the key exceeds maximum length, three is returned.
//!	 @return   If it failed to set connection policy, four is returned.
//!	 @return   If the function fails to connect, five is returned.
//!
//!  @brief    This functions calls the WLAN API functions and connect the CC3000 module
//!			   to the AP with the given SSID, key and security mode.
//
//***************************************************************************************
int cc3000_connect_secure(const char *ssid, const char *key, int32_t secMode) {
  if ((secMode < 0) || (secMode > 3)) {
    debug_write_line("Security mode must be between 0 and 3");
    return 1;
  }

  if (strlen(ssid) > MAXSSID) {
    debug_write("SSID length must be < ");
    debug_write_i32(MAXSSID, 10);
    debug_write_line("");
    return 2;
  }

  if (strlen(key) > MAXLENGTHKEY) {
    debug_write("Key length must be < ");
    debug_write_i32(MAXLENGTHKEY, 10);
    debug_write_line("");
    return 3;
  }

  if (wlan_ioctl_set_connection_policy(0, 0, 0) != 0) {
    debug_write_line("Failed setting the connection policy");
    return 4;
  }

  mWaitms(500);

  if (wlan_connect(secMode, (char *) ssid, strlen(ssid), NULL, (unsigned char *) key, strlen(key)) != 0) {
    debug_write_line("SSID connection failed");
    return 5;
  }

  // Wait for 'HCI_EVNT_WLAN_UNSOL_CONNECT' in CC3000_UsynchCallback

  return 0;
}
//***************************************************************************************
//
//!  cc3000_check_dhcp
//!
//!  @param    none
//!                     
//!  @return   1 if cc3000_dhcp is zero
//!	 @return   0 otherwise
//!
//!  @brief    This functions returns a value indicating whether cc3000_dhcp is zero.
//
//***************************************************************************************
int cc3000_check_dhcp() {
  return cc3000_dhcp == 0 ? 1 : 0;
}

//***************************************************************************************
//
//!  cc3000_is_socket_closed
//!
//!  @param    none
//!                     
//!  @return   1 if socket is closed
//!	 @return   0 otherwise
//!
//!  @brief    This functions returns a value indicating whether the number "sock" 
//!			   element of the cc3000_closed_sockets arrray is closed.
//
//***************************************************************************************
int cc3000_is_socket_closed(uint32_t sock) {
  return cc3000_closed_sockets[sock];
}


void cc3000_socket_closed_acknowledge(uint32_t sock){
	cc3000_closed_sockets[sock]=0;
}
//***************************************************************************************
//
//!  available
//!
//!  @param    socket	the socket target to check on
//!                     
//!  @return   1 if data is available
//!	 @return   0 otherwise
//!
//!  @brief    This functions returns a value indicating whether there is data available
//!			   in the socket.
//
//***************************************************************************************
int available(int16_t socket) {
  // not open!
  
  if (socket < 0) {
    debug_write("socket not open");
    return 0;
  }

  // do a select() call on this socket
  timeval timeout;
  CC3000_fd_set fd_read;
  memset(&fd_read, 0, sizeof (fd_read));
  CC3000_FD_SET(socket, &fd_read);

  timeout.tv_sec = 0;
  timeout.tv_usec = 100; // 5 millisec
  int s = select(socket + 1, &fd_read, NULL, NULL, &timeout);
  if (s == 1) {
    return 1; // some data is available to read
  } else {
    return 0; // no data is available
  }
  // return s;
}

//***************************************************************************************
//
//!  iptol
//!
//!  @param      a		The number before the first decimal point in an IP address
//!	 @param		   b		The number before the second decimal point in an IP address
//!	 @param		   c		The number before the third decimal point in an IP address
//!	 @param		   d		The number after the third decimal point in an IP address
//!                     
//!  @return   32 bit representation of the IP address
//!
//!  @brief    This functions takes the IP address which is represented in four 8-bit 
//!			   numbers and returns it as a 32 bit number.
//
//***************************************************************************************
uint32_t iptol(uint8_t a, uint8_t b, uint8_t c, uint8_t d) {
  return d << 24 | c << 16 | b << 8 | a;
}

//***************************************************************************************
//
//!  cc3000_display_mac_address
//!
//!  @param    none
//!                     
//!  @return   none
//!
//!  @brief    This functions calls the cc3000_get_mac_address function and prints
//!			   the machine address information if it is successfully retrieved.
//
//***************************************************************************************
void cc3000_display_mac_address() {
  uint8_t macAddress[20];

  if (cc3000_get_mac_address(macAddress) != 0) {
    debug_write_line("Unable to retrieve MAC Address!");
  } else {
    debug_write("MAC Address: ");
    debug_write_u8_array(macAddress, 6);
    debug_write_line("");
  }
}

//***************************************************************************************
//
//!  cc3000_is_connected
//!192.168.10.101
//!  @param    none
//!                     
//!  @return   none
//!
//!  @brief    This functions returns the value of cc3000_connected to indicate whether
//!			   an connection has been established.
//
//***************************************************************************************
volatile int cc3000_is_connected() {
  return cc3000_connected;
}

//***************************************************************************************
//
//!  cc3000_display_ipconfig
//!
//!  @param    none
//!                     
//!  @return   0 on success.
//!	 @return   1 otherwise.
//!
//!  @brief    This functions prints the IP address, subnet mask, Gateway of other
//!			   information of the connection.
//
//***************************************************************************************
int cc3000_display_ipconfig() {
  tNetappIpconfigRetArgs ipconfig;

  // Display the IP address DNS, Gateway, etc.
  netapp_ipconfig(&ipconfig);

  if (ipconfig.aucIP[3] == 0) {
    debug_write_line("no valid IP addresses");
    return 1;
  }

  debug_write("IP: ");
  debug_write_ip(u8iparraytol(ipconfig.aucIP));
  debug_write_line("");

  debug_write("Subnet Mask: ");
  debug_write_ip(u8iparraytol(ipconfig.aucSubnetMask));
  debug_write_line("");

  debug_write("Gateway: ");
  debug_write_ip(u8iparraytol(ipconfig.aucDefaultGateway));
  debug_write_line("");

  debug_write("DHCPsrv: ");
  debug_write_ip(u8iparraytol(ipconfig.aucDHCPServer));
  debug_write_line("");

  debug_write("DNSserv: ");
  debug_write_ip(u8iparraytol(ipconfig.aucDNSServer));
  debug_write_line("");

  debug_write("MAC: ");
  debug_write_mac(ipconfig.uaMacAddr);
  debug_write_line("");

  debug_write("SSID: ");
  debug_write_ssid(ipconfig.uaSSID);
  debug_write_line("");

  return 0;
}

//***************************************************************************************
//
//!  u8iparraytol
//!
//!  @param    none
//!                     
//!  @return   32 bit representation of the value.
//!
//!  @brief    This functions takes an 8 bit pointer (which is an IP address represented 
//!			   in four 8-bit values) and returns the corresponding value as a 32-bit 
//!			   value.
//
//***************************************************************************************
uint32_t u8iparraytol(uint8_t* a) {
  uint32_t l;
  memcpy(&l, a, 4);
  return l;
}

//***************************************************************************************
//
//!  debug_write_ip
//!
//!  @param    ip		IP address in 32 bit representation.
//!                     
//!  @return   none
//!
//!  @brief    This functions takes 32 bit value (IP address) and prints it as 4 decrete
//!			   values separated by 3 decimal points.
//
//***************************************************************************************
void debug_write_ip(uint32_t ip) {
  debug_write_u8(ip >> 24, 10);
  debug_write_ch('.');
  debug_write_u8(ip >> 16, 10);
  debug_write_ch('.');
  debug_write_u8(ip >> 8, 10);
  debug_write_ch('.');
  debug_write_u8(ip >> 0, 10);
}

//***************************************************************************************
//
//!  debug_write_mac
//!
//!  @param    mac		8 bit pointer to the machine address value.
//!                     
//!  @return   none
//!
//!  @brief    This functions takes an 8-bit pointer and prints the next 5 bytes in 
//!			   the machine address representation.
//
//***************************************************************************************
void debug_write_mac(uint8_t* mac) {
  for (int i = 5; i >= 0; i--) {
    if (i < 5) {
      debug_write_ch(':');
    }
    debug_write_u8(mac[i], 16);
  }
}

//***************************************************************************************
//
//!  debug_write_ssid
//!
//!  @param    ssid		8 bit pointer to the SSID of the wireless network
//!                     
//!  @return   none
//!
//!  @brief    This functions takes an 8-bit pointer and prints it in the SSID 
//!			   representation.
//
//***************************************************************************************
void debug_write_ssid(uint8_t* ssid) {
  for (int i = 0; i < 32; i++) {
    if (ssid[i] == 0) {
      break;
    }
    debug_write_ch(ssid[i]);
  }
}
/**
  * @brief  This function handles IRQ interrupt from CC3000. This 
  * interrupt line is connected to PA2.
  * @param  None
  * @retval None
  */
void EXTI2_TS_IRQHandler(void) 
{
  if (EXTI_GetITStatus(CC3000_IRQ_EXTI_LINE) != RESET) {
    cc3000_irq();
    EXTI_ClearITPendingBit(CC3000_IRQ_EXTI_LINE);
  }
}
//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//
//*****************************************************************************