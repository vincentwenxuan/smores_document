/**
  *****************************************************************************************
  * @file    PWM.c 
  * @author  Chao Liu (chao.liu0307@gmail.com)
  * @version V1
  * @date    22-August-2014
  * @brief   This source file is used for PWM on ATmega88
  *****************************************************************************************
*/

/*
 * PWM.c
 *
 * Created: 8/22/2014 1:20 PM
 *  Author: Chao Liu(chao.liu0307@gmail.com)
 */ 

#include "PWM.h"

unsigned int _pulse_number_ch1;
unsigned int _pulse_number_ch2;

unsigned int count1 = 0;
unsigned int count2 = 0;

//**************************************************************************************
//
//!  pwm_channel_init
//!
//!  @param    PWMx: Struct of PWM Channel, PWMx.PWM_CHANNEL, PWMx.frequency, PWMx.duty_cycle, PWMx.pulse_number
//!          
//!  @return   none
//!
//!  @brief    This function is used to initialize PWM channel, including the channel,
//!            the frequency, the duty cycle and the number of pulses.     
//
//**************************************************************************************

void pwm_channel_init(PWM_Init PWMx)
{	
    clear(TCCR1B, CS12);
    clear(TCCR1B, CS11);
    clear(TCCR1B, CS10);  // close timer
    
    set(TCCR1B, WGM13);
    set(TCCR1B, WGM12);
    set(TCCR1A, WGM11);
    clear(TCCR1A, WGM10);

    ICR1 = (int)(8000000/PWMx.frequency) - 1;
   
	if(PWMx.PWM_CHANNEL == 1)
	{
	    clear(DDRB, 1);
		
		set(TCCR1A, COM1A1);
		set(TCCR1A, COM1A0);

		OCR1A = (int)((float)ICR1 * PWMx.duty_cycle);

		_pulse_number_ch1 = PWMx.pulse_number;

	}
	else
	{
		clear(DDRB, 2);   // pwm output, set when necessary

		set(TCCR1A, COM1B1);
		clear(TCCR1A, COM1B0);
		
		OCR1B = (int)((float)ICR1 * PWMx.duty_cycle);

		_pulse_number_ch2 = PWMx.pulse_number;
		
	}
}

//**************************************************************************************
//
//!  pwm_set_frequency
//!
//!  @param    frequency: Frequency of PWM Signal
//!          
//!  @return   none
//!
//!  @brief    This function is used to set the frequency of PWM signal.
//
//**************************************************************************************
void pwm_set_frequency(unsigned long frequency)
{
    ICR1 = (int)(8000000/frequency) - 1;	
}

//**************************************************************************************
//
//!  pwm_set_duty_cycle
//!
//!  @param    PWM_CHANNEL: OC1A or OC1B
//!  @param    duty_cycle: Duty Cycle of PWM Signal
//!
//!  @return   none
//!
//!  @brief    This function is used to set the duty cycle for a given pwm channel.
//
//**************************************************************************************
void pwm_set_duty_cycle(PWM_Channel PWM_CHANNEL, float duty_cycle)
{
	if(PWM_CHANNEL == 1)
	{
		OCR1A = (int)((float)ICR1 * duty_cycle);
	}
	else
	{
		OCR1B = (int)((float)ICR1 * duty_cycle);
	}
}

//**************************************************************************************
//
//!  pwm_set_pulse_number
//!
//!  @param    PWM_CHANNEL: OC1A or OC1B
//!  @param    pulse_number: Number of Pulses to Output
//!
//!  @return   none
//!
//!  @brief    This function is used to set the number of pulses for a given pwm channel.
//
//**************************************************************************************
void pwm_set_pulse_number(PWM_Channel PWM_CHANNEL, unsigned int pulse_number)
{
	if(PWM_CHANNEL == 1)
	{
		_pulse_number_ch1 = pulse_number;
	}
	else
	{
		_pulse_number_ch2 = pulse_number;
	}
}

//**************************************************************************************
//
//!  Timer 1 Compare Match A Interrupt
//!
//!  @param    none
//!
//!  @return   none
//!
//!  @brief    This function is used to call Timer 1 Compare Match A interrupt.
//
//**************************************************************************************
ISR (TIMER1_COMPA_vect)
{
	count1++;
	if(count1 > _pulse_number_ch1)
	{
		count1 = 0;
		STOP_PWM_OC1A;
	}
}
	
//**************************************************************************************
//
//!  Timer 1 Compare Match B Interrupt
//!
//!  @param    none
//!
//!  @return   none
//!
//!  @brief    This function is used to call Timer 1 Compare Match B interrupt.
//
//**************************************************************************************
ISR (TIMER1_COMPB_vect)
{
	count2++;
	if(count2 > _pulse_number_ch2)
	{
		count2 = 0;
		STOP_PWM_OC1B;
	}
}
