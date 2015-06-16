/**
  *****************************************************************************************
  * @file    PWM.h
  * @author  Chao Liu (chao.liu0307@gmail.com)
  * @version V1
  * @date    22-August-2014
  * @brief   This header file is used for PWM on ATmega88
  *****************************************************************************************
*/

/*
 * PWM.h
 *
 * Created: 8/22/2014 1:20 PM
 *  Author: Chao Liu(chao.liu0307@gmail.com)
 */ 


#ifndef PWM_H_
#define PWM_H_

#include "m_general.h"

#define START_PWM_OC1A    set(DDRB, 1);sei();set(TIMSK1, OCIE1A)   ///< Spark PWM Output on OC1A
#define START_PWM_OC1B    set(DDRB, 2);sei();set(TIMSK1, OCIE1B)   ///< Spark PWM Output on OC1B

#define STOP_PWM_OC1A     clear(DDRB, 1);cli();clear(TIMSK1, OCIE1A)   ///< Stop PWM Output on OC1A
#define STOP_PWM_OC1B     clear(DDRB, 2);cli();clear(TIMSK1, OCIE1B)   ///< Stop PWM Output on OC1B

typedef enum pwm_channel
{
	OC1A = 1,
	OC1B
}PWM_Channel;

typedef struct PWM_Parameter
{
	PWM_Channel PWM_CHANNEL;   ///< PWM Channel, OC1A or OC1B
    unsigned int frequency;   ///< Frequency of PWM Signal
    float duty_cycle;   ///< Duty Cycle of PWM Signal
    unsigned int pulse_number;   ///< Number of Pulses of the PWM Output
}PWM_Init;

void pwm_channel_init(PWM_Init PWMx);
void pwm_set_frequency(unsigned int frequency);
void pwm_set_duty_cycle(PWM_Channel PWM_CHANNEL, float duty_cycle);
void pwm_set_pulse_number(PWM_Channel PWM_CHANNEL, unsigned int pulse_number);

#endif /* PWM_H_ */
