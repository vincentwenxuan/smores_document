/*
 * drive_coil.h
 *
 * Created: 8/19/2014
 *  Author: Chao Liu (chao.liu0307@gmail.com)
 */ 


#ifndef DRIVE_COIL_H_
#define DRIVE_COIL_H_


#include "common_peripherals.h"
#include "PWM.h"

#define COMMON_PLUS_ON        START_PWM_OC1B
#define COMMON_PLUS_OFF       STOP_PWM_OC1B

#define COMMON_MINUS_ON       START_PWM_OC1A
#define COMMON_MINUS_OFF      STOP_PWM_OC1A

#define COIL1_PLUS_ON   set(PORTB, 5)
#define COIL1_PLUS_OFF  clear(PORTB, 5)
#define COIL1_MINUS_ON  set(PORTC, 3)
#define COIL1_MINUS_OFF clear(PORTC, 3)

#define COIL2_PLUS_ON   set(PORTD, 7)
#define COIL2_PLUS_OFF  clear(PORTD, 7)
#define COIL2_MINUS_ON  set(PORTD, 6)
#define COIL2_MINUS_OFF clear(PORTD, 6)

#define COIL3_PLUS_ON   set(PORTD, 5)
#define COIL3_PLUS_OFF  clear(PORTD, 5)
#define COIL3_MINUS_ON  set(PORTD, 4)
#define COIL3_MINUS_OFF clear(PORTD, 4)

#define COIL4_PLUS_ON   set(PORTB, 4)
#define COIL4_PLUS_OFF  clear(PORTB, 4)
#define COIL4_MINUS_ON  set(PORTC, 2)
#define COIL4_MINUS_OFF clear(PORTC, 2)

typedef enum Coil
{
	COIL1 = 1,
	COIL2,
	COIL3,
	COIL4
}COIL;

typedef enum Direction
{
    STOP = 0,
    PLUS,
    MINUS
}DIRECTION;

void init_coil(unsigned long frequency, float duty_cycle, unsigned int pulse_number);
void drive_coil_set_pulse_number(unsigned int pulse_number);
void drive_coil_set_frequency(unsigned long frequency);
void drive_coil_set_duty_cycle(float duty_cycle);
void drive_coil_communication(void);
void drive_coil1(DIRECTION direction);
void drive_coil2(DIRECTION direction);
void drive_coil3(DIRECTION direction);
void drive_coil4(DIRECTION direction);

#endif /* DRIVE_COIL_H_ */
