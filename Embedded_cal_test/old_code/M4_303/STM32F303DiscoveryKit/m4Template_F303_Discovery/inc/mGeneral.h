//-----------------------------------------------------------------------------
// MAEVARM M4 STM32F373 general 
// version: 1.0
// date: February 24, 2013
// author: Neel Shah (neels@seas.upenn.edu)
// modified by Chao Liu (chao.liu0307@gmail.com)
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
//#include "stm32f30x_hrtim.h"
#include "stm32f30x_i2c.h"
#include "stm32f30x_iwdg.h"
#include "stm32f30x_misc.h"
#include "stm32f30x_opamp.h"
#include "stm32f30x_pwr.h"
#include "stm32f30x_rcc.h"
#include "stm32f30x_rtc.h"
#include "stm32f30x_syscfg.h"
#include "stm32f30x_spi.h"
#include "stm32f30x_tim.h"
#include "stm32f30x_usart.h"
#include "stm32f30x_wwdg.h"
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
//#define I2C_Timing_FastModePlus  0x00C71030
//#define I2C_Timing_FastMode      0x6043070E
//#define I2C_Timing_Standard      0x60C3283A
//-----------------------------------------------------------------------------


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




#endif