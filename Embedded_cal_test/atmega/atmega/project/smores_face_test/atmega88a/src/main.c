/* Name: main.c
 * Author: <insert your name here>
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

#include "common_peripherals.h"

#define PORT_TEST
//#define LED_TEST
//#define PWM_TEST
//#define COIL_TEST

#ifdef PWM_TEST
 #include "PWM.h"
#endif

#ifdef COIL_TEST
 #include "drive_coil.h"
#endif

int main(void)
{
    /* insert your hardware initialization here */
#ifdef PORT_TEST
	set(DDRB,0);
	set(DDRB,1);
	set(DDRB,2);
	set(DDRB,3);
	set(DDRB,4);
	set(DDRB,5);
	set(DDRB,6);
	set(DDRB,7);

	set(DDRD,0);
	set(DDRD,1);
	set(DDRD,2);
	set(DDRD,3);
	set(DDRD,4);
	set(DDRD,5);
	set(DDRD,6);

	set(DDRC,0);
	set(DDRC,1);
	set(DDRC,2);
	set(DDRC,3);
	set(DDRC,4);
	set(DDRC,5);
	set(DDRC,6);
	set(DDRC,7);

	set(PORTB,0);
	set(PORTB,1);
	set(PORTB,2);
	set(PORTB,3);
	set(PORTB,4);
	set(PORTB,5);
	set(PORTB,6);
	set(PORTB,7);

	set(PORTC,0);
	set(PORTC,1);
	set(PORTC,2);
	set(PORTC,3);
	set(PORTC,4);
	set(PORTC,5);
	set(PORTC,6);

	set(PORTD,0);
	set(PORTD,1);
	set(PORTD,2);
	set(PORTD,3);
	set(PORTD,4);
	set(PORTD,5);
	set(PORTD,6);
	set(PORTD,7);
#endif

#ifdef LED_TEST
    InitPeripherals();
#endif

#ifdef PWM_TEST
    PWM_Init COMMON_PLUS;
	PWM_Init COMMON_MINUS;

	unsigned int frequency = 166;
    float duty_cycle = 0.5;
    unsigned int pulse_number = 65535;
	//PWM_Init COMMON_PLUS;
    COMMON_PLUS.PWM_CHANNEL = OC1B;
    COMMON_PLUS.frequency = frequency;
    COMMON_PLUS.duty_cycle = duty_cycle;
    COMMON_PLUS.pulse_number = pulse_number;

    pwm_channel_init(COMMON_PLUS);

    //PWM_Init COMMON_MINUS;
    COMMON_MINUS.PWM_CHANNEL = OC1A;
    COMMON_MINUS.frequency = frequency;
    COMMON_MINUS.duty_cycle = duty_cycle;
    COMMON_MINUS.pulse_number = pulse_number;;

    pwm_channel_init(COMMON_MINUS);

    sei();

    START_PWM_OC1B;
    START_PWM_OC1A;
#endif

#ifdef COIL_TEST
    InitPeripherals();
    sei();
    unsigned int frequency = 166;
    float duty_cycle = 0.5;
    unsigned int pulse_number = 3;
    init_coil(frequency, duty_cycle, pulse_number);
    DIRECTION coil1_direction = STOP;
    DIRECTION coil2_direction = STOP;
    DIRECTION coil3_direction = STOP;
    DIRECTION coil4_direction = STOP;
#endif
    for(;;){
        /* insert your main loop code here */
#ifdef LED_TEST
    m_green(TOGGLE);
    m_blue(TOGGLE);
    m_yellow(TOGGLE);
    m_wait(1000);
#endif 

#ifdef COIL_TEST
    m_green(TOGGLE);
    coil1_direction = PLUS;
    drive_coil1(coil1_direction);
    m_wait(500);
    m_yellow(TOGGLE);
    coil2_direction = PLUS;
    drive_coil2(coil2_direction);
    m_wait(500);
    /* m_blue(TOGGLE); */
    coil3_direction = PLUS;
    drive_coil3(coil3_direction);
    m_wait(500);
    m_blue(TOGGLE);
    coil4_direction = PLUS;
    drive_coil4(coil4_direction);
    m_wait(500);
    drive_coil1(STOP);
    drive_coil2(STOP);
    drive_coil3(STOP);
    drive_coil4(STOP);    
    m_wait(2000);

    m_green(TOGGLE);
    coil1_direction = MINUS;
    drive_coil1(coil1_direction);
    m_wait(500);
    m_yellow(TOGGLE);
    coil2_direction = MINUS;
    drive_coil2(coil2_direction);
    m_wait(500); 
    m_blue(TOGGLE);
    coil3_direction = MINUS;
    drive_coil3(coil3_direction);
    m_wait(500);
    coil4_direction = MINUS;
    drive_coil4(coil4_direction);
    m_wait(500);
    drive_coil1(STOP);
    drive_coil2(STOP);
    drive_coil3(STOP);
    drive_coil4(STOP);
    m_wait(2000);
#endif
    }
    return 0;   /* never reached */
}
