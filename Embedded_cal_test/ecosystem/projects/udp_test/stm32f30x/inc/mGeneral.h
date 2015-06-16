//-----------------------------------------------------------------------------
// MAEVARM M4 STM32F373 general 
// version: 1.0
// date: February 24, 2013
// author: Neel Shah (neels@seas.upenn.edu)
//-----------------------------------------------------------------------------

#ifndef __m_general
#define __m_general

//-----------------------------------------------------------------------------
// Standard Peripheral Libraries that we'll need at times:
//-----------------------------------------------------------------------------
#include "stm32f30x_adc.h"
#include "stm32f30x_can.h"
#include "stm32f30x_crc.h"
#include "stm32f30x_comp.h"
#include "stm32f30x_dac.h"
#include "stm32f30x_dbgmcu.h"
#include "stm32f30x_dma.h"
#include "stm32f30x_exti.h"
#include "stm32f30x_flash.h"
#include "stm32f30x_gpio.h"
// #include "stm32f30x_hrtim.h" // SOMETHING WRONG WITH THIS
#include "stm32f30x_syscfg.h"
#include "stm32f30x_i2c.h"
#include "stm32f30x_iwdg.h"
#include "stm32f30x_opamp.h"
#include "stm32f30x_pwr.h"
#include "stm32f30x_rcc.h"
#include "stm32f30x_rtc.h"
#include "stm32f30x_spi.h"
#include "stm32f30x_tim.h"
#include "stm32f30x_usart.h"
#include "stm32f30x_wwdg.h"
#include "stm32f30x_misc.h"  /* High level functions for NVIC and SysTick (add-on to CMSIS functions) */
#include "stm32f30x_i2c_cpal.h" 
#include "stm32f30x_i2c_cpal_hal.h" 
#include "stm32f30x_i2c_cpal_conf.h" 
#include "arm_math.h"

//-----------------------------------------------------------------------------
// Useful pre-compile constants
//-----------------------------------------------------------------------------

#define ERROR	0
#define SUCCESS	1
#define OFF		0
#define ON		1
#define TOGGLE	2
#ifndef NULL
#define NULL ((void *)0)
#endif

//-----------------------------------------------------------------------------
// Bit manipulation and validation on registers:
//-----------------------------------------------------------------------------

#define set(reg,bit)		(reg |=  (bit))
#define clear(reg,bit)		(reg &= ~(bit))
#define toggle(reg,bit)		(reg ^=  (bit))
#define check(reg,bit)		(reg & (bit))
#define clearset(reg, clearmask, setmask)  (reg = reg & (~(clearmask)) | (setmask))

//-----------------------------------------------------------------------------
// I2C Timing constants:
//-----------------------------------------------------------------------------

//FastMode+ -  1 MHz speed (Rise time = 26ns, Fall time = 2ns, Analog Filter OFF)
//FastMode - 400 KHz speed (Rise time = 100ns, Fall time = 10ns, Analog Filter ON)
//Standard - 100 KHz speed (Rise time = 100ns, Fall time = 10ns, Analog Filter ON)
#define I2C_Timing_FastModePlus  0x00C71030
#define I2C_Timing_FastMode      0x6043070E
#define I2C_Timing_Standard      0x60C3283A
//-----------------------------------------------------------------------------
// Change state of on-board LEDs: 
//-----------------------------------------------------------------------------
// For efficient code, use the macros without arguments defined like mRedON
// Older M2 style function definitions : "val" must be either OFF, ON, or TOGGLE
// green=PC13, yellow=PC15, red=PC14, Purple=PB15 , Pink=PD8
// #undef mGreenON
// #undef mGreenOFF
// #undef mGreenTOGGLE
// #undef mYellowON
// #undef mYellowOFF
// #undef mYellowTOGGLE
// #undef mRedON
// #undef mRedOFF
// #undef mRedTOGGLE
// #undef mPurpleON
// #undef mPurpleOFF
// #undef mPurpleTOGGLE
// #undef mPinkON
// #undef mPinkOFF
// #undef mPinkTOGGLE
/*
#undef mGreen(val)
#undef mYellow(val)
#undef mRed(val)
#undef mPurple(val)
#undef mPink(val)
// */
// #define mGreenOFF		GPIOC->BRR  = (1<<13)
// #define mGreenON		GPIOC->BSRR = (1<<13)
// #define mGreenTOGGLE	GPIOC->ODR ^= (1<<13)
// #define mYellowOFF		GPIOC->BRR  = (1<<15)
// #define mYellowON		GPIOC->BSRR = (1<<15)
// #define mYellowTOGGLE	GPIOC->ODR ^= (1<<15)
// #define mRedOFF			GPIOC->BRR  = (1<<14)
// #define mRedON			GPIOC->BSRR = (1<<14)
// #define mRedTOGGLE		GPIOC->ODR ^= (1<<14)
// #define mPurpleON	    GPIOB->BSRR = (1<<12)
// #define mPurpleOFF		GPIOB->BRR  = (1<<12)
// #define mPurpleTOGGLE	GPIOB->ODR ^= (1<<12)

// #define mPinkOFF		GPIOB->BRR  = (1<<13)
// #define mPinkON			GPIOB->BSRR = (1<<13)
// #define mPinkTOGGLE		GPIOB->ODR ^= (1<<13)

/*
#define mGreen(val)		if(val==OFF){mGreenOFF;}\
						else if(val==ON){mGreenON;}\
						else if(val==TOGGLE){mGreenTOGGLE;}
#define mYellow(val) 	if(val==OFF){mYellowOFF;}\
						else if(val==ON){mYellowON;}\
						else if(val==TOGGLE){mYellowTOGGLE;}
#define mRed(val)		if(val==OFF){mRedOFF;}\
						else if(val==ON){mRedON;}\
						else if(val==TOGGLE){mRedTOGGLE;}
#define mPurple(val)		if(val==ON){mPurpleON;}\
						else if(val==OFF){mPurpleOFF;}\
						else if(val==TOGGLE){mPurpleTOGGLE;}
#define mPink(val)		if(val==ON){mPinkON;}\
						else if(val==OFF){mPinkOFF;}\
						else if(val==TOGGLE){mPinkTOGGLE;}
*/
// -----------------------------------------------------------------------------
// Wait for a specified number of clock cycles: 
// -----------------------------------------------------------------------------
// val can between 1 to (2^32-1) 
// and the number clock cycles wasted will be 4 * val
#define mWait(val)   do{register uint32_t i;for(i=0;i<val;++i)\
		__asm__ __volatile__ ("nop\n\t":::"memory");} while(0);
#define mWaitus(val) do{register uint32_t i;for(i=0;i<(val*18);++i)\
		__asm__ __volatile__ ("nop\n\t":::"memory");} while(0);
#define mWaitms(val) do{register uint32_t i;for(i=0;i<(val*18000);++i)\
		__asm__ __volatile__ ("nop\n\t":::"memory");} while(0);						

// -----------------------------------------------------------------------------
// M4 Board Initialization and mBus Setup:
// -----------------------------------------------------------------------------
void mInit(void); 



#endif