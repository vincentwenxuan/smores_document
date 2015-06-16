/*
 * USART.h
 *
 * Created: 7/24/2014 
 *  Author: Chao Liu (chao.liu0307@gmail.com)
 */ 


#ifndef USART_H_
#define USART_H_

#ifdef	__cplusplus
extern "C" {
#endif

#include "m_general.h"
#include <stdlib.h>

#define ENABLE 1
#define DISABLE 0

typedef enum usart_channel
{
	USART0 = 0,
	USART1
}USART_Channel;


typedef struct USART_Parameter
{
	USART_Channel USART_channel;
	unsigned char USART_RECEIVER_STATUS;
	unsigned char USART_TRANSMITTER_STATUS;
	unsigned int USART_BAUDRATE;
	unsigned char USART_STOP_BIT;
}USART;


void USART_Init(USART USARTx);
void USART0_Transmit_char(unsigned char data);
void USART0_Transmit_int(int data);
void USART0_Transmit_uint(unsigned int data);
void USART0_Transmit_long(long data);
void USART0_Transmit_ulong(unsigned long data);
void USART0_Transmit_string(char *str);
unsigned char USART0_Receive_char(void);
void USART0_Receive_flush(void);
void USART1_Transmit_char(unsigned char data);
void USART1_Transmit_int(int data);
void USART1_Transmit_uint(unsigned int data);
void USART1_Transmit_long(long data);
void USART1_Transmit_ulong(unsigned long data);
void USART1_Transmit_string(char *str);
unsigned char USART1_Receive_char(void);
void USART1_Receive_flush(void);




#ifdef	__cplusplus
}
#endif

#endif /* USART_H_ */