/*
 * ADC.c
 *
 * Created: 8/6/2014 2:41:25 PM
 *  Author: Chao Liu(chao.liu0307@gmail.com)
 */ 

#include "ADC.h"

void adc_set_ref(ADC_Ref adcRef)
{
    ADMUX &= 0x2F;
	ADMUX |= (adcRef << REFS0);
}

void adc_set_prescalar(uint8_t adcPrescalar)
{
    ADCSRA &= 0xF8;
	switch(adcPrescalar)
	{
		case 2:
            ADCSRA |= 0 << ADPS0;
            break;
		case 4:
            ADCSRA |= 2 << ADPS0;
            break;
		case 8:
            ADCSRA |= 3 << ADPS0;
            break;
		case 16:
            ADCSRA |= 4 << ADPS0;
            break;
		case 32:
            ADCSRA |= 5 << ADPS0;
            break;
		case 64:
            ADCSRA |= 6 << ADPS0;
            break;
		case 128:
            ADCSRA |= 7 << ADPS0;
            break;
		default:	// default is 64
            ADCSRA |= 6 << ADPS0;
            break;
	}
}

void adc_set_channel(ADC_Channel adcChannel)
{
    ADMUX &= 0xF0;
	ADMUX |= (adcChannel << MUX0);
    adc_pin_init(adcChannel);
}

void adc_pin_init(ADC_Channel adcChannel)
{
	switch(adcChannel)
	{
		case ADC0:
            DIDR0 = 1 << ADC0D;
            break;
		case ADC1:
            DIDR0 = 1 << ADC1D;
            break;
		case ADC2:
            DIDR0 = 1 << ADC2D;
            break;
		case ADC3:
            DIDR0 = 1 << ADC3D;
            break;
		case ADC4:
            DIDR0 = 1 << ADC4D;
            break;
		case ADC5:
            DIDR0 = 1 << ADC5D;
            break;
	}
}

void adc_pin_deInit(ADC_Channel adcChannel)
{
	switch(adcChannel)
	{
		case ADC0:
            DIDR0 = 0 << ADC0D;
            break;
		case ADC1:
            DIDR0 = 0 << ADC1D;
            break;
		case ADC2:
            DIDR0 = 0 << ADC2D;
            break;
		case ADC3:
            DIDR0 = 0 << ADC3D;
            break;
		case ADC4:
            DIDR0 = 0 << ADC4D;
            break;
		case ADC5:
            DIDR0 = 0 << ADC5D;
            break;
	}
}

void adc_set_mode(ADC_Mode adcMode)
{
    ADCSRB &= 0xF8;
    ADCSRB |= adcMode << ADTS0;
}

void adc_init(ADC_Init ADCx)
{
    adc_set_channel(ADCx.ADC_CHANNEL);
    adc_pin_init(ADCx.ADC_CHANNEL);
    adc_set_prescalar(ADCx.ADC_PRESCALAR);
    adc_set_ref(ADCx.ADC_REF);
    adc_set_mode(ADCx.ADC_MODE);
}