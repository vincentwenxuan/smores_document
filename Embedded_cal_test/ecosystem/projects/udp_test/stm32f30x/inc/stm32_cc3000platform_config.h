/**
  ******************************************************************************
  * @file    stm32_cc3000platform_config.h 
  * @author  
  * @version V1.1.0
  * @date    06-June-2014
  * @brief   Header for stm32_cc3000.c module
  ******************************************************************************
*/
 
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef PLATFORM_CONFIG_H
#define	PLATFORM_CONFIG_H

#ifdef	__cplusplus
extern "C" {
#endif


/* Define the external interrupt interface with CC3000 */
#define CC3000_IRQ_RCC         RCC_AHBPeriph_GPIOA
#define CC3000_IRQ_PORT        GPIOA
#define CC3000_IRQ_PIN         GPIO_Pin_2
#define CC3000_IRQ_EXTI_LINE   EXTI_Line2
#define CC3000_IRQ_EXTI_PORT   EXTI_PortSourceGPIOA
#define CC3000_IRQ_EXTI_PIN    GPIO_PinSource2
#define CC3000_IRQ_EXTI_CH     EXTI2_TS_IRQn

/* Define GPIO pin to interface with the VBEN pin on CC3000*/
#define CC3000_VBAT_EN_RCC     RCC_AHBPeriph_GPIOA
#define CC3000_VBAT_EN_PORT    GPIOA
#define CC3000_VBAT_EN_PIN     GPIO_Pin_3

/* Communication boards SPIx Interface with CC3000*/
#define CC3000_CS_RCC          RCC_AHBPeriph_GPIOA
#define CC3000_CS_PORT         GPIOA
#define CC3000_CS_PIN          GPIO_Pin_4

#define CC3000_SCK_GPIO_PORT   GPIOA
#define CC3000_SCK_GPIO_SOURCE GPIO_PinSource5
#define CC3000_SCK_AF   	   GPIO_AF_5
#define CC3000_SCK_GPIO_PIN    GPIO_Pin_5

#define CC3000_MISO_GPIO_PORT   GPIOA
#define CC3000_MISO_GPIO_SOURCE GPIO_PinSource6
#define CC3000_MISO_AF   	    GPIO_AF_5
#define CC3000_MISO_GPIO_PIN    GPIO_Pin_6

#define CC3000_MOSI_GPIO_PORT   GPIOA
#define CC3000_MOSI_GPIO_SOURCE GPIO_PinSource7
#define CC3000_MOSI_AF   	    GPIO_AF_5
#define CC3000_MOSI_GPIO_PIN    GPIO_Pin_7

#define CC3000_SPI			    SPI1
#define CC3000_SPI_CLK		    RCC_APB2Periph_SPI1

/* Define GPIO configuration of the SPI peripheral*/ 
#define CC3000_SPI_GPIO_MODE	GPIO_Mode_AF
#define CC3000_SPI_GPIO_SPEED	GPIO_Speed_50MHz
#define CC3000_SPI_GPIO_OTYPE	GPIO_OType_PP
#define CC3000_SPI_GPIO_PUPD	GPIO_PuPd_NOPULL

/* Connection properties. Change according to settings*/
//IP and PORT for TRANSMISSION
#define SEND_PORT_NUM 			 8888
#define SEND_ADDR1 				 0xFF0AA8C0  // 192.168.10.255
#define SEND_ADDR2				 0x660AA8C0  // 192.168.10.102 (always verify)
//IP and PORT for RECEPTION
#define RECV_PORT_NUM 			 2222
#define LISTEN_ADDR  			 0x00 // Receive from all

//AP Information 
#define WLAN_SSID    			  "Gumsticks Modlab"
#define WLAN_PASS    			  "Modlab3142"
// Security can be WLAN_SEC_UNSEC, WLAN_SEC_WEP, WLAN_SEC_WPA or WLAN_SEC_WPA2
#define WLAN_SECURITY					  WLAN_SEC_WPA2
#define CC3000_APP_BUFFER_SIZE  	          (5)
#define CC3000_RX_BUFFER_OVERHEAD_SIZE        (20)

//GENERIC ROUTER INFORMATION (change according to router)
#define STATIC_SUBNET_MASK 			 0x00FFFFFF//255.255.255.0
#define STATIC_DEFAULT_GATEWAY 		 0x010AA8C0//192.168.10.1
#define STATIC_DNS_SERVER 			 0x010AA8C0//192.168.10.1
#define MOD_START_IP				 0x6E0AA8C0 //Module IPs will start from 
                                          //192.168.10.(110+module number)



#ifdef	__cplusplus
}
#endif

#endif	/* PLATFORM_CONFIG_H */

