// -----------------------------------------------------------------------------
// MAEVARM general macros
// version: 2.1
// date: Sept 29, 2011
// author: J. Fiene
// edited for ATmega88 by: Chao Liu(chao.liu0307@gmail.com)
// -----------------------------------------------------------------------------
#ifndef m_general__
#define m_general__

#include "project_peripherals.h"

// -----------------------------------------------------------------------------
// Useful pre-compile constants
// -----------------------------------------------------------------------------

#define TRUE	1
#define FALSE	0

#define OFF		0
#define ON		1
#define TOGGLE	2

// -----------------------------------------------------------------------------
// Useful string replacement macros
// -----------------------------------------------------------------------------
#define PASTER(x,y) x ## y
#define MAKENAME(x,y) PASTER(x,y)


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

#define m_clockdivide(val)	CLKPR = (1<<CLKPCE); CLKPR=val
// "val" must be an integer from 0 to 8
// this will divide the 8MHz(for internal RC oscillator) or 16MHz(for M2) system clock by 2^val:
// 0 = 8 MHz | 16MHz
// 1 = 4 MHz | 8MHz
// 2 = 2 MHz | 4MHz
// 3 = 1 MHz | 2MHz
// 4 = 500 MHz | 1MHz
// 5 = 250 kHz | 500kHz
// 6 = 125 kHz | 250kHz
// 7 = 62.5 kHz | 125kHz
// 8 = 31.25 kHz | 62.5kHz


// -----------------------------------------------------------------------------
// Wait for a specified number of milliseconds:
// -----------------------------------------------------------------------------

#define m_wait(val)			_delay_ms(val)
// "val" must be an integer from 1 to 65535
// this function assumes the clockdivider is 0

// -----------------------------------------------------------------------------
// Change the state of the green on-board LED:
// -----------------------------------------------------------------------------

#if defined(LED_GREEN_PORT) && defined(LED_GREEN_PIN)
  #define LED_GREEN_PORT_FULL MAKENAME(PORT,LED_GREEN_PORT)
  #define LED_GREEN_DDR_FULL MAKENAME(DDR,LED_GREEN_PORT)
  #ifdef LED_GREEN_SET_ON
    #define m_green(val)		if(val==OFF){clear(LED_GREEN_PORT_FULL,LED_GREEN_PIN);}else if(val==ON){set(LED_GREEN_PORT_FULL,LED_GREEN_PIN);}else if(val==TOGGLE){toggle(LED_GREEN_PORT_FULL,LED_GREEN_PIN);}
  #else
    #define m_green(val)		if(val==OFF){set(LED_GREEN_PORT_FULL,LED_GREEN_PIN);}else if(val==ON){clear(LED_GREEN_PORT_FULL,LED_GREEN_PIN);}else if(val==TOGGLE){toggle(LED_GREEN_PORT_FULL,LED_GREEN_PIN);}
  #endif
#endif
// "val" must be either OFF, ON, or TOGGLE, as defined above

// -----------------------------------------------------------------------------
// Change the state of the yellow on-board LED:
// -----------------------------------------------------------------------------
#if defined(LED_YELLOW_PORT) && defined(LED_YELLOW_PIN)
  #define LED_YELLOW_PORT_FULL MAKENAME(PORT,LED_YELLOW_PORT)
  #define LED_YELLOW_DDR_FULL MAKENAME(DDR,LED_YELLOW_PORT)
  #ifdef LED_YELLOW_SET_ON
    #define m_yellow(val)		if(val==OFF){clear(LED_YELLOW_PORT_FULL,LED_YELLOW_PIN);}else if(val==ON){set(LED_YELLOW_PORT_FULL,LED_YELLOW_PIN);}else if(val==TOGGLE){toggle(LED_YELLOW_PORT_FULL,LED_YELLOW_PIN);}
  #else
    #define m_yellow(val)		if(val==OFF){set(LED_YELLOW_PORT_FULL,LED_YELLOW_PIN);}else if(val==ON){clear(LED_YELLOW_PORT_FULL,LED_YELLOW_PIN);}else if(val==TOGGLE){toggle(LED_YELLOW_PORT_FULL,LED_YELLOW_PIN);}
  #endif
#endif

// "val" must be either OFF, ON, or TOGGLE, as defined above

// -----------------------------------------------------------------------------
// Change the state of the blue on-board LED:
// -----------------------------------------------------------------------------
#if defined(LED_BLUE_PORT) && defined(LED_BLUE_PIN)
  #define LED_BLUE_PORT_FULL MAKENAME(PORT,LED_BLUE_PORT)
  #define LED_BLUE_DDR_FULL MAKENAME(DDR,LED_BLUE_PORT)
  #ifdef LED_BLUE_SET_ON
    #define m_blue(val)			if(val==OFF){clear(LED_BLUE_PORT_FULL,LED_BLUE_PIN);}else if(val==ON){set(LED_BLUE_PORT_FULL,LED_BLUE_PIN);}else if(val==TOGGLE){toggle(LED_BLUE_PORT_FULL,LED_BLUE_PIN);}
  #else
    #define m_blue(val)			if(val==OFF){set(LED_BLUE_PORT_FULL,LED_BLUE_PIN);}else if(val==ON){clear(LED_BLUE_PORT_FULL,LED_BLUE_PIN);}else if(val==TOGGLE){toggle(LED_BLUE_PORT_FULL,LED_BLUE_PIN);}
  #endif
#endif

// "val" must be either OFF, ON, or TOGGLE, as defined above

// -----------------------------------------------------------------------------
// Change the state of the red on-board LED:
// -----------------------------------------------------------------------------
#if defined(LED_RED_PORT) && defined(LED_RED_PIN)
  #define LED_RED_PORT_FULL MAKENAME(PORT,LED_RED_PORT)
  #define LED_RED_DDR_FULL MAKENAME(DDR,LED_RED_PORT)
  #ifdef LED_RED_SET_ON
    #define m_red(val)		if(val==OFF){clear(LED_RED_PORT_FULL,LED_RED_PIN);}else if(val==ON){set(LED_RED_PORT_FULL,LED_RED_PIN);}else if(val==TOGGLE){toggle(LED_RED_PORT_FULL,LED_RED_PIN);}
  #else
    #define m_red(val)		if(val==OFF){set(LED_RED_PORT_FULL,LED_RED_PIN);}else if(val==ON){clear(LED_RED_PORT_FULL,LED_RED_PIN);}else if(val==TOGGLE){toggle(LED_RED_PORT_FULL,LED_RED_PIN);}
  #endif
#endif

// "val" must be either OFF, ON, or TOGGLE, as defined above

#endif
