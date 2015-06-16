/*
 * ATtiny1634_ADC.c
 *
 * Created: 7/28/2014 4:16:48 PM
 *  Author: Chao Liu(chao.liu0307@gmail.com)
 */ 


#include "m_general.h"
#include "ADC.h"
#include "USART.h"

int adc_value;

int main(void)
{
	USART_Channel usart1 = USART1;
	USART USART_CHANNEL;
	USART_CHANNEL.USART_channel = usart1;
	USART_CHANNEL.USART_BAUDRATE = 9600;
	USART_CHANNEL.USART_RECEIVER_STATUS = ENABLE;
	USART_CHANNEL.USART_TRANSMITTER_STATUS = ENABLE;
	USART_CHANNEL.USART_STOP_BIT = 1;
	USART_Init(USART_CHANNEL);
	
	ADC_Channel adc = ADC1;
	ADC_Ref vol_ref = VCC;
	
	adc_set_channel(adc);
	adc_pin_init(adc);
	adc_set_prescalar(128);
	adc_set_ref(vol_ref);
	
	adcInterruptEnable;
	adcEnable;
	adcAutoTriggerEnable;
	adcStart;
	
    while(1)
    {
        //TODO:: Please write your application code
		USART1_Transmit_int(adc_value);
		USART1_Transmit_string("\n\r");
    }
}

ISR(ADC_READY_vect)
{
	adc_value = ADC;
	set(ADCSRA, ADIF);
}