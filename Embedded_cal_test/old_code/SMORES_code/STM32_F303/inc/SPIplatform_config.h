/*
Author Chaitanya Bhargava (cbhar@seas.upenn.edu)
Settings for SPI usage on the STM32F303
mGeneral includes all the ST library files and the wait functions
If using some wireless llike the Zigbee or the WiFi then this 
library is not required unless modified as such
*/


#ifdef __cplusplus
 extern "C" {
#endif
#define SPI_NUM	            			SPI1
#define SPI_RCC							RCC_APB2Periph_SPI1
#define SPI_PORT						GPIOA
#define SPI_AF							GPIO_AF_5
#define SPI_SCK_PIN 					GPIO_Pin_5
#define SPI_SCK_PIN_SOURCE 				GPIO_PinSource5
#define SPI_MISO_PIN 					GPIO_Pin_6
#define SPI_MISO_PIN_SOURCE				GPIO_PinSource6
#define SPI_MOSI_PIN 					GPIO_Pin_7
#define SPI_MOSI_PIN_SOURCE 			GPIO_PinSource7
#define SPI_NSS_PORT					GPIOA
#define SPI_NSS_PIN						GPIO_Pin_4
#define SPI_MODE						SPI_Mode_Master
#define SPI_SCALING						SPI_BaudRatePrescaler_256


#ifdef __cplusplus
}
#endif