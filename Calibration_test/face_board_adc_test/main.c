/*
 * Name: main.c
 * Created: 8/7/2014
 * Author: Chao Liu (chao.liu0307@gmail.com)
 */

#include "common_peripherals.h"
#include "USART.h"
#include "ADC.h"

int adc_value[2];

int main(void)
{
    /* insert your hardware initialization here */
	InitPeripherals();
	
	m_clockdivide(0);
	USART USART_CHANNEL;
	USART_CHANNEL.USART_BAUDRATE = 9600;
	USART_CHANNEL.USART_RECEIVER_STATUS = RECEIVER_ENABLE;
	USART_CHANNEL.USART_TRANSMITTER_STATUS = TRANSMITTER_ENABLE;
	USART_CHANNEL.USART_STOP_BIT = 1;
    USART_CHANNEL.USART_PARITY = PARITY_NONE;
	
	USART_Init(USART_CHANNEL);
    
    /*
	ADC_Channel adc = ADC3;
	ADC_Ref vol_ref = VCC;
    ADC_Mode mode = FREE_RUN;
	
	adc_set_channel(adc);
	adc_pin_init(adc);
	adc_set_prescalar(128);
	adc_set_ref(vol_ref);
    adc_set_mode(mode);
	*/
    
    ADC_Init ADC_test;
    ADC_test.ADC_CHANNEL = ADC0;
    ADC_test.ADC_REF = VCC;
    ADC_test.ADC_PRESCALAR = 64;
    ADC_test.ADC_MODE = FREE_RUN;
    
    adc_init(ADC_test);
    
	adcInterruptEnable;
	adcEnable;
	adcAutoTriggerEnable;
	adcStart;
	
	set(DDRB, 1);
	sei();
    m_blue(ON);
    for(;;){
        /* insert your main loop code here */
		//USART_Transmit_int(adc_value);
		//USART_Transmit_string("\n\r");
    USART_Transmit_int(adc_value[0]);
	USART_Transmit_char(' ');
	USART_Transmit_int(adc_value[1]);
    USART_Transmit_string("\n\r");
	m_wait(10);
		
		
		
    }
    return 0;   /* never reached */
}

ISR(ADC_vect)
{
	m_yellow(ON);
    static ADC_Channel ADC_channel = ADC0;
    if (ADC_channel == ADC0) {
        adc_value[0] = ADC;
		ADC = 0;
    } else
    {
        adc_value[1] = ADC;
		ADC = 0;
    }
	int i = 0;
	for(i=0;i<375;i++)
	{
		m_blue(TOGGLE);
	};
    ADC_channel = !ADC_channel;
    adc_set_channel(ADC_channel);
    set(ADCSRA, ADIF);
}
