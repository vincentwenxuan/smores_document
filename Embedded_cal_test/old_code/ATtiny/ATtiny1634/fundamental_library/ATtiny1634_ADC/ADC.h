/*
 * ADC.h
 *
 * Created: 7/28/2014 4:19:32 PM
 *  Author: Chao Liu(chao.liu0307@gmail.com)
 */ 


#ifndef ADC_H_
#define ADC_H_

#include "m_general.h"

#define ENABLE 1
#define DISABLE 0

#define adcInterruptEnable	sei();set(ADCSRA, ADIE)
#define adcInterruptDisable	cli();clear(ADCSRA, ADIE)
#define adcAutoTriggerEnable	set(ADCSRA, ADATE)
#define adcAutoTriggerDisable	clear(ADCSRA, ADATE)
#define adcEnable	set(ADCSRA, ADEN)
#define adcDisable	clear(ADCSRA, ADEN)
#define adcStart	set(ADCSRA, ADSC)

typedef enum adc_channel
{
	ADC0 = 0,
	ADC1,
	ADC2,
	ADC3,
	ADC4,
	ADC5,
	ADC6,
	ADC7,
	ADC8,
	ADC9,
	ADC10,
	ADC11	
}ADC_Channel;

typedef enum adc_ref
{
	VCC = 0,
	EXT,
	INT,
}ADC_Ref;
	

void adc_set_channel(ADC_Channel adcChannel);
void adc_set_ref(ADC_Ref adcRef);
void adc_set_prescalar(uint8_t adcPrescalar);
void adc_pin_init(ADC_Channel adcChannel);

#endif /* ADC_H_ */