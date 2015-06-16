/**
  *****************************************************************************************
  * @file    ADC.h
  * @author  Chao Liu (chao.liu0307@gmail.com)
  * @version V1
  * @date    06-August-2014
  * @brief   This header file is used for ADC on ATmega88
  *****************************************************************************************
*/

/*
 * ADC.h
 *
 * Created: 8/7/2014 2:39:32 PM
 *  Author: Chao Liu(chao.liu0307@gmail.com)
 */ 


#ifndef ADC_H_
#define ADC_H_

#include "m_general.h"

#define adcInterruptEnable      set(ADCSRA, ADIE)   ///< Enable ADC Interrupt 
#define adcInterruptDisable     clear(ADCSRA, ADIE)    ///< Disable ADC Interrupt
#define adcAutoTriggerEnable	set(ADCSRA, ADATE)    ///< Enable ADC Auto Trigger
#define adcAutoTriggerDisable	clear(ADCSRA, ADATE)    ///< Disable ADC Auto Trigger
#define adcEnable               set(ADCSRA, ADEN)    ///< Enable ADC
#define adcDisable              clear(ADCSRA, ADEN)   ///< Disable ADC
#define adcStart                set(ADCSRA, ADSC)    ///< Start A/D Conversion

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
	EXT = 0,    ///< External AREF
	VCC = 1,    ///< VCC of ATmega88
	INT = 3     ///< Internal 1.1V Reference Voltage
}ADC_Ref;

typedef enum adc_mode
{
    FREE_RUN = 0,   ///< Free Running Mode
    ANA_COM,        ///< Analog Comparator
    EXT_INT,        ///< External Interrupt Request 0
    TIM0_COM,       ///< Timer/Counter0 Compare Match A
    TIM0_OVERFLOW,  ///< Timer/Counter0 Overflow
    TIM1_COM,       ///< Timer/Counter1 Compare Match B
    TIM1_OVERFLOW,  ///< Timer/Counter1 Overflow
    TIM1_CAP        ///< Timer/Counter1 Capture Event
}ADC_Mode;

typedef struct ADC_Parameter
{
    ADC_Channel ADC_CHANNEL;   ///< ADC Channel
    unsigned char ADC_PRESCALAR;    ///< ADC Clock Prescalar
    ADC_Ref ADC_REF;     ///< ADC Reference Voltage
    ADC_Mode ADC_MODE;    ///< ADC Trigger Source
}ADC_Init;

void adc_init(ADC_Init ADCx);
void adc_set_channel(ADC_Channel adcChannel);
void adc_set_ref(ADC_Ref adcRef);
void adc_set_prescalar(uint8_t adcPrescalar);
void adc_pin_init(ADC_Channel adcChannel);
void adc_pin_deInit(ADC_Channel adcChannel);
void adc_set_mode(ADC_Mode adcMode);

#endif /* ADC_H_ */
