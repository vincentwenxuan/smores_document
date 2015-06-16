/*
 * USART.h
 *
 * Created: 6/19/2014 8:08:22 AM
 *  Author: Chao Liu (chao.liu0307@gmail.com)
 */ 


#ifndef USART_H_
#define USART_H_

#ifdef	__cplusplus
extern "C" {
#endif

#include "m_general.h"
#include "USART_config.h"
#include <stdlib.h>

void USART_Init(void);
void USART_Transmit_char(unsigned char data);
void USART_Transmit_int(int data);
void USART_Transmit_uint(unsigned int data);
void USART_Transmit_long(long data);
void USART_Transmit_ulong(unsigned long data);
void USART_Transmit_string(char *str);
unsigned char USART_Receive_char(void);
void USART_Receive_flush(void);


#ifdef	__cplusplus
}
#endif

#endif /* USART_H_ */