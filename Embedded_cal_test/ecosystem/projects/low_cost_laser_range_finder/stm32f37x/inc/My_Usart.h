//-----------------------------------------------------------------------------
// MAEVARM M4 STM32F373 USART TX
// date: May 12, 2014
// author: Chaitty Bhaergava (Cbhar@seas.upenn.edu)
// modified by Chao Liu (chao.liu0307@gmail.com)
//-----------------------------------------------------------------------------

#ifndef _MY_USART_H
#define _MY_USART_H

#include "stm32f37x.h"
#include "stm32f37x_usart.h"
#include "stm32f37x_gpio.h"
#include "stm32f37x_rcc.h"

#ifdef __cplusplus
extern "C" {
#endif

void USART_Config(void);
    
#ifdef __cplusplus
}
#endif

#endif
