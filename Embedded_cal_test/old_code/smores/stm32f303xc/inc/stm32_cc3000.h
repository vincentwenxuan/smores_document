/**
  ******************************************************************************
  * @file    stm32_cc3000.h 
  * @author  
  * @version V1.1.0
  * @date    06-June-2014
  * @brief   Header for stm32_cc3000.c module
  ******************************************************************************
*/
 
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STM32_CC3000_H
#define	STM32_CC3000_H

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "mGeneral.h"
#include "wlan.h"
#include "hci.h"
#include "socket.h"
#include "nvmem.h"
#include "netapp.h"

#include "stm32_cc3000platform_config.h"
#include "USARTdebug.h"


#ifdef	__cplusplus
extern "C" {
#endif

/* Define parameters for WLAN connection */
#define WLAN_CONNECT_TIMEOUT_MS 	10000
#define MAX_SOCKETS             	32
#define MAXSSID				  	        32
#define MAXLENGTHKEY 		          32

/* declare type spi_rx_handler_t as a pointer to a function */
  typedef void (*spi_rx_handler_t)(void *p);

/* external global variables */
  extern char cc3000_device_name[];
  extern unsigned char wlan_tx_buffer[];
  
/* Function prototypes */
  void cc3000_startup_check();
  int cc3000_setup(int patchesAvailableAtHost, int useSmartConfigData);
  int cc3000_get_firmware_version(uint8_t *major, uint8_t *minor);
  int cc3000_get_mac_address(uint8_t *addr);
  void cc3000_irq();
  int cc3000_delete_profiles();
  void cc3000_setup_spi();
  int cc3000_connect_to_ap(const char *ssid, const char *key, uint8_t secmode);
  int cc3000_check_dhcp();
  int cc3000_is_socket_closed(uint32_t sock);
  void cc3000_display_mac_address();
  int cc3000_display_ipconfig();
  volatile int cc3000_is_connected();
  void cc3000_socket_closed_acknowledge(uint32_t sock);
  void cc3000_initialize_ip(int module_ip);
  
  //Required by cc3000 Host driver
  void SpiOpen(spi_rx_handler_t rx_handler);
  void SpiClose();
  void SpiResumeSpi();
  long SpiWrite(unsigned char *pUserBuffer, unsigned short usLength);

  //debugging functions
  void debug_write_ip(uint32_t ip);
  void debug_write_mac(uint8_t* mac);
  void debug_write_ssid(uint8_t* ssid);

  uint32_t u8iparraytol(uint8_t* a);
  uint32_t iptol(uint8_t a, uint8_t b, uint8_t c, uint8_t d);
  int16_t connect_udp(uint32_t destIP, uint16_t destPort);
  int available(int16_t socket);
  
  

#ifdef	__cplusplus
}
#endif

#endif	/* STM32_CC3000_H */

