/*
 * ADC.h
 *
 * Created: 8/7/2014 2:39:32 PM
 *  Author: Chao Liu(chao.liu0307@gmail.com)
 */ 


#ifndef ADC_H_
#define ADC_H_

#include "m_general.h"

#define adcInterruptEnable      sei();set(ADCSRA, ADIE)
#define adcInterruptDisable     cli();clear(ADCSRA, ADIE)
#define adcAutoTriggerEnable	set(ADCSRA, ADATE)
#define adcAutoTriggerDisable	clear(ADCSRA, ADATE)
#define adcEnable               set(ADCSRA, ADEN)
#define adcDisable              clear(ADCSRA, ADEN)
#define adcStart                set(ADCSRA, ADSC)

typedef enum adc_channel
{
	ADC0 = 0,
	ADC1,
	ADC2,
	ADC3,
	ADC4,
	ADC5
}ADC_Channel;

typedef enum adc_ref
{
	EXT = 0,
	VCC = 1,
	INT = 3
}ADC_Ref;

typedef enum adc_mode
{
    FREE_RUN = 0,
    ANA_COM,
    EXT_INT,
    TIM0_COM,
    TIM0_OVERFLOW,
    TIM1_COM,
    TIM1_OVERFLOW,
    TIM1_CAP
}ADC_Mode;

typedef struct ADC_Parameter
{
    ADC_Channel ADC_CHANNEL;
    unsigned char ADC_PRESCALAR;
    ADC_Ref ADC_REF;
    ADC_Mode ADC_MODE;
}ADC_Init;

void adc_init(ADC_Init ADCx);
void adc_set_channel(ADC_Channel adcChannel);
void adc_set_ref(ADC_Ref adcRef);
void adc_set_prescalar(uint8_t adcPrescalar);
void adc_pin_init(ADC_Channel adcChannel);
void adc_pin_deInit(ADC_Channel adcChannel);
void adc_set_mode(ADC_Mode adcMode);

#endif /* ADC_H_ */