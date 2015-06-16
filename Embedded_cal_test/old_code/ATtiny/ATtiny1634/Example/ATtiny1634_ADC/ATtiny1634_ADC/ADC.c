/*
 * ADC.c
 *
 * Created: 7/28/2014 4:19:19 PM
 *  Author: Chao Liu(chao.liu0307@gmail.com)
 */ 

#include "ADC.h"

void adc_set_ref(ADC_Ref adcRef)
{
	ADMUX |= (adcRef << REFS0);
}

void adc_set_prescalar(uint8_t adcPrescalar)
{
	switch(adcPrescalar)
	{
		case 2:
		clear(ADCSRA, ADPS2);
		clear(ADCSRA, ADPS1);
		clear(ADCSRA, ADPS0);
		break;
		case 4:
		clear(ADCSRA, ADPS2);
		set(ADCSRA, ADPS1);
		clear(ADCSRA, ADPS0);
		break;
		case 8:
		clear(ADCSRA, ADPS2);
		set(ADCSRA, ADPS1);
		set(ADCSRA, ADPS0);
		break;
		case 16:
		set(ADCSRA, ADPS2);
		clear(ADCSRA, ADPS1);
		clear(ADCSRA, ADPS0);
		break;	
		case 32:
		set(ADCSRA, ADPS2);
		clear(ADCSRA, ADPS1);
		set(ADCSRA, ADPS0);
		break;
		case 64:
		set(ADCSRA, ADPS2);
		set(ADCSRA, ADPS1);
		clear(ADCSRA, ADPS0);
		break;	
		case 128:
		set(ADCSRA, ADPS2);
		set(ADCSRA, ADPS1);
		set(ADCSRA, ADPS0);
		break;
		default:	// default is 64
		set(ADCSRA, ADPS2);
		set(ADCSRA, ADPS1);
		clear(ADCSRA, ADPS0);
		break;		
	}
}

void adc_set_channel(ADC_Channel adcChannel)
{
	ADMUX |= (adcChannel << MUX0);
}

void adc_pin_init(ADC_Channel adcChannel)
{
	switch(adcChannel)
	{
		case ADC0:
		set(DIDR0, ADC0D);
		break;
		case ADC1:
		set(DIDR0, ADC1D);
		break;
		case ADC2:
		set(DIDR0, ADC2D);
		break;
		case ADC3:
		set(DIDR0, ADC3D);
		break;
		case ADC4:
		set(DIDR0, ADC4D);
		break;	
		case ADC5:
		set(DIDR1, ADC5D);
		break;
		case ADC6:
		set(DIDR1, ADC6D);
		break;
		case ADC7:
		set(DIDR1, ADC7D);
		break;
		case ADC8:
		set(DIDR1, ADC8D);
		break;	
		case ADC9:
		set(DIDR2, ADC9D);
		break;	
		case ADC10:
		set(DIDR2, ADC10D);
		break;
		case ADC11:
		set(DIDR2, ADC11D);
		break;			
	}
}