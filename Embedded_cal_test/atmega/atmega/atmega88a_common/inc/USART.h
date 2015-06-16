/**
  *****************************************************************************************
  * @file    USART.h 
  * @author  Chao Liu (chao.liu0307@gmail.com)
  * @version V1
  * @date    06-August-2014
  * @brief   This header file is used for USART on ATmega88
  *****************************************************************************************
*/
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
	RECEIVER_DISABLE = 0,    ///< Disable Receiver Mode
	RECEIVER_ENABLE          ///< Enable Receiver Mode
}USART_Receiver_Status;
    
typedef enum usart_transmitter_status
{
	TRANSMITTER_DISABLE = 0,    ///< Disable Transmitter Mode
	TRANSMITTER_ENABLE          ///< Enable Transmitter Mode
}USART_Transmitter_Status;

typedef enum usart_parity
{
	PARITY_NONE = 0,     ///< None Parity
	PARITY_EVEN,         ///< Even Parity
	PARITY_ODD           ///< Odd Parity
}USART_Parity;

typedef struct USART_Parameter
{
	USART_Receiver_Status USART_RECEIVER_STATUS;        ///< USART Receiver Mode
	USART_Transmitter_Status USART_TRANSMITTER_STATUS;  ///< USART Transmitter Mode
	unsigned int USART_BAUDRATE;                        ///< USART Baudrate
	unsigned char USART_STOP_BIT;                       ///< USART Stop Bit
	USART_Parity USART_PARITY;                          ///< USART Parity
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
