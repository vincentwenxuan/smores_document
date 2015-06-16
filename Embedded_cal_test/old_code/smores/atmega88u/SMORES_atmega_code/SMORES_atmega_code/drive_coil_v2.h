/*
 * drive_coils_v2.h
 *
 * Created: 9/8/2014 2:39:32 PM
 *  Author: Chaitanya Bhargava (cbhar@seas.upenn.edu)
 */ 


#ifndef DRIVE_COIL_V2_H_
#define DRIVE_COIL_V2_H_

#include "m_general.h"
/*----------------------------defines------------------------*/

#define COIL1_HIGH_ON		set(PORTB, 5)
#define COIL1_HIGH_OFF		clear(PORTB, 5)
#define COIL1_LOW_ON		set(PORTC, 3)
#define COIL1_LOW_OFF		clear(PORTC, 3)

#define COIL2_HIGH_ON		set(PORTD, 7)
#define COIL2_HIGH_OFF		clear(PORTD, 7)
#define COIL2_LOW_ON		set(PORTD, 6)
#define COIL2_LOW_OFF		clear(PORTD, 6)

#define COIL3_HIGH_ON		set(PORTD, 5)
#define COIL3_HIGH_OFF		clear(PORTD, 5)
#define COIL3_LOW_ON		set(PORTD, 4)
#define COIL3_LOW_OFF		clear(PORTD, 4)

#define COIL4_HIGH_ON		set(PORTB, 4)
#define COIL4_HIGH_OFF		clear(PORTB, 4)
#define COIL4_LOW_ON		set(PORTC, 2)
#define COIL4_LOW_OFF		clear(PORTC, 2)

#define COMM_HIGH_ON		set(TCCR1B,CS12);set(TCCR1A, COM1B1)						//enable timer1 and connect ch B to PB2
#define COMM_HIGH_OFF		clear(TCCR1B,CS12);clear(TCCR1A, COM1B1);clear(PORTB,2)		//OFF defines called in the interrupt once all pulses passed	
#define COMM_LOW_ON			set(TCCR1B,CS12);set(TCCR1A, COM1A1);set(TCCR1A,COM1A0)//;clear(PORTD,1)		//Since connected to NOR 
#define COMM_LOW_OFF		clear(TCCR1B,CS12);clear(TCCR1A, COM1A1);clear(TCCR1A, COM1A0)//;set(PORTB,1)	//along with D1	

typedef enum Coil_num
{
	CO1 = 0,
	CO2,
	CO3,
	CO4
}COIL_NUM;

typedef enum Dir
{
	STOP_ALL = 0,
	TURN_ON,
	TURN_OFF,
	USART_SENDER,
	USART_LISTENER
}DIR;

void init_all_coils();
void coil_setting(COIL_NUM coil_num,DIR direction);
void set_comm_channels();
void set_comm_freq(uint32_t freq);
void set_comm_duty_cycle(float duty_cycle);
void set_comm_nos_pulses(uint16_t num);
void Carrier_Wave(int state);
void reset_pulses_counter(void);

#endif /* DRIVE_COIL_V2_H_*/