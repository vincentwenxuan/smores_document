/*
Author Chaitanya Bhargava (cbhar@seas.upenn.edu)
Settings for USART usage on the STM32F303
*/

#ifdef	__cplusplus
extern "C" {
#endif

#define DEBUG_USART            			USART3
#define DEBUG_USART_BAUD       			19200//ERROR ACTUAL BAUD RATE IS 2X
#define DEBUG_USART_IRQ        			USART3_IRQn
#define DEBUG_USART_RCC        			RCC_APB1Periph_USART3
#define DEBUG_USART_PORT       			GPIOB
#define DEBUG_USART_TX_PIN     			GPIO_Pin_10
#define DEBUG_USART_TX_PIN_SOURCE		GPIO_PinSource10
#define DEBUG_USART_RX_PIN     			GPIO_Pin_11
#define DEBUG_USART_RX_PIN_SOURCE		GPIO_PinSource11
#define DEBUG_USART_AF					GPIO_AF_7	

#ifdef	__cplusplus
}
#endif