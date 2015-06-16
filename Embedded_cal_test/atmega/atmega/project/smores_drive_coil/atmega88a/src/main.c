/* Name: main.c
 * Author: <insert your name here>
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

#include "common_peripherals.h"
#include "drive_coil.h"

int main(void)
{
    /* insert your hardware initialization here */
    InitPeripherals();
    unsigned int frequency = 166;
    float duty_cycle = 0.5;
    unsigned int pulse_number = 3;
    init_coil(frequency, duty_cycle, pulse_number);
    DIRECTION coil1_direction = STOP;
    DIRECTION coil2_direction = STOP;
    DIRECTION coil3_direction = STOP;
    DIRECTION coil4_direction = STOP;
    sei();
    for(;;){
        /* insert your main loop code here */
        m_green(TOGGLE);
        coil1_direction = MINUS;
        drive_coil1(coil1_direction);
        m_wait(500);
        m_yellow(TOGGLE);
        coil2_direction = MINUS;
        drive_coil2(coil2_direction);
        m_wait(500);
        /* m_blue(TOGGLE); */
        coil3_direction = MINUS;
        drive_coil3(coil3_direction);
        m_wait(500);
        m_blue(TOGGLE);
        coil4_direction = MINUS;
        drive_coil4(coil4_direction);
        m_wait(2000);

        /* m_yellow(TOGGLE); */
        /* coil1_direction = PLUS; */
        /* drive_coil1(coil1_direction); */
        /* m_wait(2000); */
        /* coil2_direction = PLUS; */
        /* drive_coil2(coil2_direction); */
        /* m_wait(2000); */
        /* coil3_direction = PLUS; */
        /* drive_coil3(coil3_direction); */
        /* m_wait(2000); */
        /* coil4_direction = PLUS; */
        /* drive_coil4(coil4_direction); */
        /* m_wait(2000); */
    }
    return 0;   /* never reached */
}
