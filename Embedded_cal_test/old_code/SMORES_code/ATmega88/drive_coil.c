/*
 * drive_coil.c
 *
 * Created: 8/19/2014
 *  Author: Chao Liu (chao.liu0307@gmail.com)
 */ 

#include "drive_coil.h"

PWM_Init COMMON_PLUS;
PWM_Init COMMON_MINUS;

void init_coil(unsigned int frequency, float duty_cycle, unsigned int pulse_number)
{
     
    set(DDRB, 5);
    set(DDRB, 6);   // digital output control for coil 1
    
    set(DDRD, 7);
    set(DDRD, 6);   // digital output control for coil 2
    
    set(DDRD, 5);
	set(DDRD, 4);   // digital output control for coil 3

	set(DDRC, 3);
	set(DDRC, 2);   // digital output control for coil 4

	clear(PORTB, 5);
	clear(PORTB, 6);    // shut down the coil 1

	clear(PORTD, 6);
	clear(PORTD, 7);    // shut down the coil 2

	clear(PORTD, 5);    
	clear(PORTD, 4);    // shut donw the coil 3

	clear(PORTC, 3);
	clear(PORTC, 2);    // shut down the coil 4

	//PWM_Init COMMON_PLUS;
	COMMON_PLUS.PWM_CHANNEL = OC1A;
	COMMON_PLUS.frequency = frequency;
	COMMON_PLUS.duty_cycle = duty_cycle;
	COMMON_PLUS.pulse_number = pulse_number;

	pwm_channel_init(COMMON_PLUS);

	//PWM_Init COMMON_MINUS;
	COMMON_MINUS.PWM_CHANNEL = OC1B;
	COMMON_MINUS.frequency = frequency;
	COMMON_MINUS.duty_cycle = duty_cycle;
	COMMON_MINUS.pulse_number = pulse_number;;

	pwm_channel_init(COMMON_MINUS);
		
}

void drive_coil_set_pulse_number(unsigned int pulse_number)
{
    pwm_set_pulse_number(COMMON_PLUS.PWM_CHANNEL, pulse_number);
    pwm_set_pulse_number(COMMON_MINUS.PWM_CHANNEL, pulse_number);
}

void drive_coil_set_frequency(unsigned int frequency)
{
    pwm_set_frequency(frequency);
}

void drive_coil_set_duty_cycle(float duty_cycle)
{
    pwm_set_duty_cycle(COMMON_PLUS.PWM_CHANNEL, duty_cycle);
    pwm_set_duty_cycle(COMMON_MINUS.PWM_CHANNEL, duty_cycle);
}

void drive_coil_communication(void)
{
    pwm_set_pulse_number(COMMON_MINUS.PWM_CHANNEL, 65535);
}

void drive_coil1(DIRECTION direction)
{
	switch (direction)
	{
		case PLUS:
			COIL1_MINUS_OFF;
			COMMON_PLUS_OFF;
			COIL1_PLUS_ON;
			COMMON_MINUS_ON;
			//OCR1A = 24000;
			break;
		case MINUS:
			COIL1_PLUS_OFF;
			COMMON_MINUS_OFF;
			COIL1_MINUS_ON;
			COMMON_PLUS_ON;
			//OCR1B = 24000;
			break;
		case STOP:
			COIL1_PLUS_OFF;
			COIL1_MINUS_OFF;
			COMMON_PLUS_OFF;
			COMMON_MINUS_OFF;
			break;
	}			
}

void drive_coil2(DIRECTION direction)
{
	switch (direction)
	{
		case PLUS:
			COIL2_MINUS_OFF;
			COMMON_PLUS_OFF;
			COIL2_PLUS_ON;
			COMMON_MINUS_ON;
			//OCR1A = 24000;
			break;
		case MINUS:
			COIL2_PLUS_OFF;
			COMMON_MINUS_OFF;
			COIL2_MINUS_ON;
			COMMON_PLUS_ON;
			//OCR1B = 24000;
			break;
		case STOP:
			COIL2_PLUS_OFF;
			COIL2_MINUS_OFF;
			COMMON_PLUS_OFF;
			COMMON_MINUS_OFF;
			break;
	}			
}


void drive_coil3(DIRECTION direction)
{
	switch (direction)
	{
		case PLUS:
			COIL3_MINUS_OFF;
			COMMON_PLUS_OFF;
			COIL3_PLUS_ON;
			COMMON_MINUS_ON;
			//OCR1A = 7;
			break;
		case MINUS:
			COIL3_PLUS_OFF;
			COMMON_MINUS_OFF;
			COIL3_MINUS_ON;
			COMMON_PLUS_ON;
			//OCR1B =7;
			break;
		case STOP:
			COIL3_PLUS_OFF;
			COIL3_MINUS_OFF;
			COMMON_PLUS_OFF;
			COMMON_MINUS_OFF;
			break;
	}			
}

void drive_coil4(DIRECTION direction)
{
	switch (direction)
	{
		case PLUS:
			COIL4_MINUS_OFF;
			COMMON_PLUS_OFF;
			COIL4_PLUS_ON;
			COMMON_MINUS_ON;
			//OCR1A = 24000;
			break;
		case MINUS:
			COIL4_PLUS_OFF;
			COMMON_MINUS_OFF;
			COIL4_MINUS_ON;
			COMMON_PLUS_ON;
			//OCR1B = 24000;
			break;
		case STOP:
			COIL4_PLUS_OFF;
			COIL4_MINUS_OFF;
			COMMON_PLUS_OFF;
			COMMON_MINUS_OFF;
			break;
	}			
}
