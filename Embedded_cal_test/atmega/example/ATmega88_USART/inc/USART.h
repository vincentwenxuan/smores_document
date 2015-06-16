/*
 * USART.h
 *
 * Created: 8/6/2014
 *  Author: Chao Liu (chao.liu0307@gmail.com)
 */ 


#ifndef USART_H_
#define USART_H_

#ifdef	__cplusplus
extern "C" {
#endif

#include "m_general.h"
#include <stdlib.h>

//#define ENABLE 1
//#define DISABLE 0
    
typedef enum usart_receiver_status
    {
        RECEIVER_DISABLE = 0,
        RECEIVER_ENABLE
    }USART_Receiver_Status;
    
typedef enum usart_transmitter_status
    {
        TRANSMITTER_DISABLE = 0,
        TRANSMITTER_ENABLE
    }USART_Transmitter_Status;

typedef enum usart_parity
    {
        PARITY_NONE = 0,
        PARITY_EVEN,
        PARITY_ODD
    }USART_Parity;

typedef struct USART_Parameter
    {
        USART_Receiver_Status USART_RECEIVER_STATUS;
        USART_Transmitter_Status USART_TRANSMITTER_STATUS;
        unsigned int USART_BAUDRATE;
        unsigned char USART_STOP_BIT;
        USART_Parity USART_PARITY;
    }USART;


void USART_Init(USART USARTx);
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