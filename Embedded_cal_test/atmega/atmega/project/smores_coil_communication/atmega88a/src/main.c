/* Name: main.c
 * Author: <insert your name here>
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

#include "common_peripherals.h"
#include "USART.h"

#define MASTER
//#define SLAVE

/* #ifdef SLAVE */
/*     #include "drive_coil.h" */
/* #endif */

int main(void)
{
    /* insert your hardware initialization here */
    InitPeripherals();
    
#ifdef MASTER
    USART USART_CHANNEL;
    USART_CHANNEL.USART_BAUDRATE = 9600;
    USART_CHANNEL.USART_RECEIVER_STATUS = RECEIVER_ENABLE;
    USART_CHANNEL.USART_TRANSMITTER_STATUS = TRANSMITTER_ENABLE;
    USART_CHANNEL.USART_STOP_BIT = 1;
    USART_CHANNEL.USART_PARITY = PARITY_EVEN;
    
    USART_Init(USART_CHANNEL);
    
    clear(TCCR1B, CS12);
    clear(TCCR1B, CS11);
    set(TCCR1B, CS10);  // prescalar is 1
    
    set(TCCR1B, WGM13);
    set(TCCR1B, WGM12);
    set(TCCR1A, WGM11);
    clear(TCCR1A, WGM10);
    
    set(TCCR1A, COM1A1);
    set(TCCR1A, COM1A0);
    
    set(DDRB, 1);
    
    ICR1 = (int)(8000000/500000) - 1;
    
    OCR1A = (int)((float)ICR1 * 0.5);   // common minus output 500KHz signals
    
    set(DDRD, 5);
    set(PORTD, 5);    // COIL3 PLUS on
    
//    unsigned long frequency = 500000;
//    float duty_cycle = 0.5;
//    unsigned int pulse_number = 3;
//    init_coil(frequency, duty_cycle, pulse_number);
//    
//    COIL3_PLUS_ON;
//    
//    drive_coil_communication();

    sei();
    
#endif
    
#ifdef SLAVE
//    unsigned long frequency = 500000;
//    float duty_cycle = 0.5;
//    unsigned int pulse_number = 3;
//    init_coil(frequency, duty_cycle, pulse_number);
//    COIL1_MINUS_ON;
    set(DDRD, 4);
    set(PORTD, 4);  // COIL2_MINUS on

    set(DDRD, 1);
    set(PORTD, 1);  // USART TX is high


    set(DDRB, 3);
    set(PORTB, 3);    // turn on receiver
#endif
    
    m_green(ON);m_yellow(ON);m_blue(ON);
    m_wait(1000);
    m_green(OFF);m_yellow(OFF);m_blue(OFF);
    m_wait(2000);
    for(;;){
        /* insert your main loop code here */
#ifdef MASTER
        set(PORTD, 5);
        USART_Transmit_string("MODLAB!\n\r");
        clear(PORTD, 5);
        m_wait(1000);
#endif
#ifdef SLAVE
        m_blue(TOGGLE);
#endif
    }
    return 0;   /* never reached */
}
