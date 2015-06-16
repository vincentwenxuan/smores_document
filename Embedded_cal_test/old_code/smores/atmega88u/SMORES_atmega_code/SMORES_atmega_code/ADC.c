/**
  *****************************************************************************************
  * @file    ADC.c 
  * @author  Chao Liu (chao.liu0307@gmail.com)
  * @version V1
  * @date    06-August-2014
  * @brief   This source file is used for ADC on ATmega88
  *****************************************************************************************
*/

/*
 * ADC.c
 *
 * Created: 8/6/2014 2:41:25 PM
 *  Author: Chao Liu(chao.liu0307@gmail.com)
 */ 

#include "ADC.h"

//**************************************************************************************
//
//!  adc_set_ref
//!
//!  @param    adcRef: EXT, VCC, INT
//!          
//!  @return   none
//!
//!  @brief    This function is used to set the reference voltage for ADC peripherial.     
//
//**************************************************************************************
void adc_set_ref(ADC_Ref adcRef)
{
    ADMUX &= 0x2F;
	ADMUX |= (adcRef << REFS0);
}

//**************************************************************************************
//
//!  adc_set_prescalar
//!
//!  @param    adcPrescalar: 2, 4, 8, 16, 32, 64, 128
//!
//!  @return   none
//!
//!  @brief    This function is used to set the prescalar for ADC peripherial. The ADC clock
//!            should be set between 50 and 200kHz.
//
//**************************************************************************************
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

//**************************************************************************************
//
//!  adc_set_channel
//!
//!  @param    adcChannel: ADC0, ADC1, ADC2, ADC3, ADC4, ADC5
//!
//!  @return   none
//!
//!  @brief    This function is used to set the ADC channel for ADC peripherial. 
//
//**************************************************************************************
void adc_set_channel(ADC_Channel adcChannel)
{
    ADMUX &= 0xF0;
	ADMUX |= (adcChannel << MUX0);
    adc_pin_init(adcChannel);
}

//**************************************************************************************
//
//!  adc_pin_init
//!
//!  @param    adcChannel: ADC0, ADC1, ADC2, ADC3, ADC4, ADC5
//!
//!  @return   none
//!
//!  @brief    This function is used to set the ADC pin for ADC peripherial. When using
//!            a specific IO to do ADC, we have to set the corresponding pin.
//
//**************************************************************************************
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

//**************************************************************************************
//
//!  adc_set_deInit
//!
//!  @param    adcChannel: ADC0, ADC1, ADC2, ADC3, ADC4, ADC5
//!
//!  @return   none
//!
//!  @brief    This function is used to deinitialize the ADC channel for ADC peripherial. 
//
//**************************************************************************************
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

//**************************************************************************************
//
//!  adc_set_mode
//!
//!  @param    adcMode: FREE_RUN, ANA_COM, EXT_INT, TIM0_COM, TIM0_OVERFLOW, TIM1_COM, TIM1_OVERFLOW, TIM1_CAP
//!
//!  @return   none
//!
//!  @brief    This function is used to set the ADC trigger source for ADC peripherial. 
//
//**************************************************************************************
void adc_set_mode(ADC_Mode adcMode)
{
    ADCSRB &= 0xF8;
    ADCSRB |= adcMode << ADTS0;
}

//**************************************************************************************
//
//!  adc_init
//!
//!  @param    ADCx: ADCx.ADC_CHANNEL, ADCx.ADC_PRESCALAR, ADCx.ADC_REF,
//!                           ADCx.ADC_MODE
//!
//!  @return   none
//!
//!  @brief    This function is used to initialize the ADC peripherial.
//
//**************************************************************************************
void adc_init(ADC_Init ADCx)
{
    adc_set_channel(ADCx.ADC_CHANNEL);
    adc_pin_init(ADCx.ADC_CHANNEL);
    adc_set_prescalar(ADCx.ADC_PRESCALAR);
    adc_set_ref(ADCx.ADC_REF);
    adc_set_mode(ADCx.ADC_MODE);
}
