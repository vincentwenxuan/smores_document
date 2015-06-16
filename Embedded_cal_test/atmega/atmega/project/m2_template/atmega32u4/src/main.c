/* Name: main.c
 * Author: <insert your name here>
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

#include "common_peripherals.h"

int main(void)
{
    InitPeripherals();
    m_clockdivide(0);
    m_red(ON);
    m_green(ON);
    m_wait(1000);
    /* insert your hardware initialization here */
    for(;;){
        /* insert your main loop code here */
        m_red(TOGGLE);
        m_green(TOGGLE);
        m_wait(1000);
    }
    return 0;   /* never reached */
}
