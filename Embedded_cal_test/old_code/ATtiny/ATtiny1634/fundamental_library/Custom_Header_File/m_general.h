// -----------------------------------------------------------------------------
// MAEVARM general macros
// version: 2.1
// date: Sept 29, 2011
// author: J. Fiene
// modified by Chao Liu(chao.liu0307@gmail.com) to work on ATtiny1634
// -----------------------------------------------------------------------------
#ifndef m_general__
#define m_general__

// -----------------------------------------------------------------------------
// Useful pre-compile constants
// -----------------------------------------------------------------------------

#define TRUE	1
#define FALSE	0

#define OFF		0
#define ON		1
#define TOGGLE	2

// -----------------------------------------------------------------------------
// General AVR libraries that we'll need at times:
// -----------------------------------------------------------------------------

#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <math.h>


// -----------------------------------------------------------------------------
// Bit manipulation and validation:
// -----------------------------------------------------------------------------

#define set(reg,bit)		reg |= (1<<(bit))
#define clear(reg,bit)		reg &= ~(1<<(bit))
#define toggle(reg,bit)		reg ^= (1<<(bit))

#define check(reg,bit)		(bool)(reg & (1<<(bit)))
// As of version 2.0, this will return either 1 (TRUE) or 0 (FALSE)

// -----------------------------------------------------------------------------
// Set the system clock:
// -----------------------------------------------------------------------------

// In order to make the m_clockdivide() function work, when flashing the code, disable CKDIV8 bit in fuse, or the default clock is 1 MHz.
#define m_clockdivide(val)	CCP = 0xD8; CLKPR=val; set(CLKSR, CSTR); clear(CLKSR, CKSEL3); clear(CLKSR, CKSEL2); set(CLKSR, CKSEL1); clear(CLKSR, CKSEL0)  // clock divide is not working now
// "val" must be an integer from 0 to 8
// this will divide the 16MHz system clock by 2^val:
// 0 = 8 MHz
// 1 = 4 MHz
// 2 = 2 MHz
// 3 = 1 MHz
// 4 = 500 kHz
// 5 = 250 kHz
// 6 = 125 kHz
// 7 = 62.5 kHz


// -----------------------------------------------------------------------------
// Wait for a specified number of milliseconds:
// -----------------------------------------------------------------------------

#define m_wait(val)			_delay_ms(val)
// "val" must be an integer from 1 to 65535
// this function assumes a 16MHz clock


// -----------------------------------------------------------------------------
// Change the state of the green on-board LED:
// -----------------------------------------------------------------------------

// green LED port cannot work properly
// #define m_green(val)		set(DDRC,3); if(val==OFF){clear(PORTC,3);}else if(val==ON){set(PORTC,3);}else if(val==TOGGLE){toggle(PORTC,3}
// "val" must be either OFF, ON, or TOGGLE, as defined above

// -----------------------------------------------------------------------------
// Change the state of the yellow on-board LED:
// -----------------------------------------------------------------------------

#define m_yellow(val)		set(DDRA,5); if(val==OFF){clear(PORTA,5);}else if(val==ON){set(PORTA,5);}else if(val==TOGGLE){toggle(PORTA,5);}
// "val" must be either OFF, ON, or TOGGLE, as defined above

// -----------------------------------------------------------------------------
// Change the state of the white on-board LED:
// -----------------------------------------------------------------------------

#define m_white(val)		set(DDRA,1); if(val==OFF){clear(PORTA,1);}else if(val==ON){set(PORTA,1);}else if(val==TOGGLE){toggle(PORTA,1);}
// "val" must be either OFF, ON, or TOGGLE, as defined above

// -----------------------------------------------------------------------------
// Change the state of the blue on-board LED:
// -----------------------------------------------------------------------------

#define m_blue(val)			set(DDRA,0); if(val==OFF){clear(PORTA,0);}else if(val==ON){set(PORTA,0);}else if(val==TOGGLE){toggle(PORTA,0);}
// "val" must be either OFF, ON, or TOGGLE, as defined above

#endif