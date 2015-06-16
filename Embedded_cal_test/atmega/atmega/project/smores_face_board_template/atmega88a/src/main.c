/* Name: main.c
 * Author: <insert your name here>
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

#include "common_peripherals.h"

int main(void)
{
    /* insert your hardware initialization here */
    InitPeripherals();
    float global_time;
    m_green(ON);m_yellow(ON);m_blue(ON);
    m_wait(1000);
    m_green(OFF);m_yellow(OFF);m_blue(OFF);
    m_wait(2000);
    for(;;){
        /* insert your main loop code here */
        global_time = GetTime();
        m_green(TOGGLE);
        m_blue(TOGGLE);
        m_yellow(TOGGLE);
        m_wait(1000);
    }
    return 0;   /* never reached */
}
