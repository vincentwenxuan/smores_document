/*
 * USART.c
 *
 * Created: 6/19/2014 8:18:43 AM
 *  Author: Chao Liu (chao.liu0307@gmail.com)
 */ 

#include "USART.h"
#include "USART_config.h"

void USART_Init(void)
{
	/* Set baud rate */
	switch (USART_BAUD)
	{
	case 2400:
		UBRRH = 0x00;
		UBRRL = 207;
		break;
	case 4800:
		UBRRH = 0x00;
		UBRRL = 103;
		break;
	case 9600:
		UBRRH = 0x00;
		UBRRL = 51;
		break;
	case 14400:
		UBRRH = 0x00;
		UBRRL = 34;
		break;
	case 19200:
		UBRRH = 0x00;
		UBRRL = 25;
		break;
	case 28800:
		UBRRH = 0x00;
		UBRRL = 16;
		break;
	case 38400:
		UBRRH = 0x00;
		UBRRL = 12;
		break;
	case 57600:
		UBRRH = 0x00;
		UBRRL = 8;
		break;	
	default:
		UBRRH = 0x00;
		UBRRL = 51;
		break;	
	}

	/* Enable receiver and transmitter */
	UCSRB = (USART_RECEIVER_STATUS<<RXEN)|(USART_TRANSMITTER_STATUS<<TXEN);
	/* Set frame format: 8data, 1stop bit */
	//UCSRC = (0<<USBS)|(3<<UCSZ0);
	switch (USART_CHARACTER_SIZE)
	{
	case 5:
		UCSRC = ((USART_STOP_BIT-1)<<USBS)|(0<<UCSZ0);
		break;
	case 6:
		UCSRC = ((USART_STOP_BIT-1)<<USBS)|(1<<UCSZ0);
		break;
	case 7:
		UCSRC = ((USART_STOP_BIT-1)<<USBS)|(2<<UCSZ0);
		break;
	case 8:
		UCSRC = ((USART_STOP_BIT-1)<<USBS)|(3<<UCSZ0);
		break;	
	default:
		UCSRC = ((USART_STOP_BIT-1)<<USBS)|(3<<UCSZ0);
		break;		
	}
}

void USART_Transmit_char(unsigned char data)
{
	/* Wait for empty transmit buffer */
	while ( !( UCSRA & (1<<UDRE)) );
	/* Put data into buffer, sends the data */
	UDR = data;
}

void USART_Transmit_int(int i)
{
	char string[7] = {0,0,0,0,0,0,0};
	itoa(i, string, 10);
	for(i=0; i<7; i++)
	{
		if(string[i])
		{
			USART_Transmit_char(string[i]);
		}
	}
}

void USART_Transmit_uint(unsigned int i)
{
	char string[6] = {0,0,0,0,0,0};
	utoa(i,string,10);
	for(i=0; i<5; i++)
	{
		if(string[i])
		{
			USART_Transmit_char(string[i]);
		}
	}
}

void USART_Transmit_long(long i)
{
	char string[11] = {0,0,0,0,0,0,0,0,0,0,0};
	ltoa(i, string, 10);
	for(i=0; i<11; i++)
	{
		if(string[i])
		{
			USART_Transmit_char(string[i]);
		}
	}
}

void USART_Transmit_ulong(unsigned long i)
{
	char string[11] = {0,0,0,0,0,0,0,0,0,0,0};
	ltoa(i, string, 10);
	for(i=0; i<10; i++)
	{
		if(string[i])
		{
			USART_Transmit_char(string[i]);
		}
	}
}

void USART_Transmit_string(char *str)
{
	while(*str)
	{
		USART_Transmit_char(*str++);
	}
}

unsigned char USART_Receive_char(void)
{
	while(!(UCSRA & (1<<RXC)));
	return UDR;
}

void USART_Receive_flush(void)
{
	unsigned char dummy;
	while ( UCSRA & (1<<RXC) ) dummy = UDR;
}
