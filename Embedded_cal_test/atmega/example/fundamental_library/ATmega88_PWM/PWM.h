/*
 * PWM.h
 *
 * Created: 8/22/2014 1:20 PM
 *  Author: Chao Liu(chao.liu0307@gmail.com)
 */ 


#ifndef PWM_H_
#define PWM_H_

#include "m_general.h"

#define START_PWM_OC1A    set(DDRB, 1);TCNT1 = 0x00;set(TIMSK1, OCIE1A);set(TCCR1B, CS10)
#define START_PWM_OC1B    set(DDRB, 2);TCNT1 = 0x00;set(TIMSK1, OCIE1B);set(TCCR1B, CS10)

#define STOP_PWM_OC1A     clear(DDRB, 1);set(PORTB, 1);clear(TIMSK1, OCIE1A);TCNT1 = 0x00;clear(TCCR1B, CS10)
#define STOP_PWM_OC1B     clear(DDRB, 2);clear(TIMSK1, OCIE1B);TCNT1 = 0x00;clear(TCCR1B, CS10)

typedef enum pwm_channel
{
	OC1A = 1,
	OC1B
}PWM_Channel;

typedef struct PWM_Parameter
{
	PWM_Channel PWM_CHANNEL;
    unsigned long frequency;
    float duty_cycle;
    unsigned int pulse_number;
}PWM_Init;

void pwm_channel_init(PWM_Init PWMx);
void pwm_set_frequency(unsigned int frequency);
void pwm_set_duty_cycle(PWM_Channel PWM_CHANNEL, float duty_cycle);
void pwm_set_pulse_number(PWM_Channel PWM_CHANNEL, unsigned int pulse_number);

#endif /* PWM_H_ */
