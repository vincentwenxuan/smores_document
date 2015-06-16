/*
Author Chaitanya Bhargava (cbhar@seas.upenn.edu)
Settings for I2C usage on the STM32F303
mGeneral includes all the ST library files and the wait functions
*/	

#ifdef	__cplusplus
extern "C" {
#endif

#define I2C_NUM            			I2C1
#define I2C_TIMING		       		0x0043070E;//0x4010000A;//0x0010000A;//0x4010000A;//0X50321312;//0x40110405;//0x00313090
									// See I2Cx_TIMINGR in Reference manual	(RM0316)
#define I2C_RCC		       			RCC_APB1Periph_I2C1
#define I2C_PORT        			GPIOB
#define I2C_AF						GPIO_AF_4
#define I2C_SCK_PIN     			GPIO_Pin_8
#define I2C_SCK_PIN_SOURCE			GPIO_PinSource8
#define I2C_SDA_PIN     			GPIO_Pin_9
#define I2C_SDA_PIN_SOURCE			GPIO_PinSource9

#ifdef	__cplusplus
}
#endif