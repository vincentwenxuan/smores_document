//-----------------------------------------------------------------------------
// MAEVARM M4 STM32F373 USART
// date: May 12, 2013
// author: Chao Liu (chao.liu0307@gmail.com)
//-----------------------------------------------------------------------------


#ifndef __SPI_H
#define __SPI_H

#include "stm32f37x_spi.h"
#include "stm32f37x_rcc.h"
#include "stm32f37x_gpio.h"

#define SPIenable   SPI_Cmd(SPI3, ENABLE)
#define SPIdisable  SPI_Cmd(SPI3, DISABLE)

#ifdef __cplusplus
extern "C" {
#endif

void SPI3_Init(void);

u16 SPIx_ReadWriteByte(u16 TxData);
    
#ifdef __cplusplus
}
#endif


#endif 


