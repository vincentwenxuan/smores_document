/*
Author Chaitanya Bhargava (cbhar@seas.upenn.edu)
Settings for ADC usage on the STM32F303
mGeneral includes all the ST library files and the wait functions
*/	

#ifdef	__cplusplus
extern "C" {
#endif
/*-------------------------ADC_A-------------------------*/	
#define ADC_A_NUM						ADC1
#define ADC_A_RCC		       			RCC_AHBPeriph_ADC12
#define ADC_A_PORT   	     			GPIOA
#define ADC_A_PIN		     			GPIO_Pin_2
#define ADC_A_CHANNEL					ADC_Channel_3

#ifdef	__cplusplus
}
#endif