/*
 * USART.c
 *
 * Created: 7/24/2014 
 *  Author: Chao Liu (chao.liu0307@gmail.com)
 */ 

#include "USART.h"

void USART_Init(USART USARTx)
{
	/* Set baud rate */
	set(UCSR0A, U2X0);
	set(UCSR1A, U2X1);
	if(USARTx.USART_channel == USART0)
	{
		switch (USARTx.USART_BAUDRATE)
		{
		case 2400:
			UBRR0 = 416;
			break;
		case 4800:
			UBRR0H = 0x00;
			UBRR0L = 207;
			break;
		case 9600:
			UBRR0H = 0x00;
			UBRR0L = 103;
			break;
		case 14400:
			UBRR0H = 0x00;
			UBRR0L = 68;
			break;
		case 19200:
			UBRR0H = 0x00;
			UBRR0L = 51;
			break;
		case 28800:
			UBRR0H = 0x00;
			UBRR0L = 34;
			break;
		case 38400:
			UBRR0H = 0x00;
			UBRR0L = 25;
			break;
		case 57600:
			UBRR0H = 0x00;
			UBRR0L = 16;
			break;	
		default:
			UBRR0H = 0x00;
			UBRR0L = 103;
			break;	
		}

		/* Enable receiver and transmitter */
		UCSR0B = (USARTx.USART_RECEIVER_STATUS<<RXEN0)|(USARTx.USART_TRANSMITTER_STATUS<<TXEN0);
		/* Set frame format: 8data, 1stop bit */
		//UCSRC = (0<<USBS)|(3<<UCSZ0);
		UCSR0C = ((USARTx.USART_STOP_BIT-1)<<USBS0)|(3<<UCSZ00);
	}
	else if (USARTx.USART_channel == USART1)
	{	
		switch (USARTx.USART_BAUDRATE)
		{
		case 2400:
			UBRR1 = 416;
			break;
		case 4800:
			UBRR1H = 0x00;
			UBRR1L = 207;
			break;
		case 9600:
			UBRR1H = 0x00;
			UBRR1L = 103;
			break;
		case 14400:
			UBRR1H = 0x00;
			UBRR1L = 68;
			break;
		case 19200:
			UBRR1H = 0x00;
			UBRR1L = 51;
			break;
		case 28800:
			UBRR1H = 0x00;
			UBRR1L = 34;
			break;
		case 38400:
			UBRR1H = 0x00;
			UBRR1L = 25;
			break;
		case 57600:
			UBRR1H = 0x00;
			UBRR1L = 16;
			break;
		default:
			UBRR1H = 0x00;
			UBRR1L = 103;
			break;
		}
		/* Enable receiver and transmitter */
		UCSR1B = (USARTx.USART_RECEIVER_STATUS<<RXEN1)|(USARTx.USART_TRANSMITTER_STATUS<<TXEN1);
		/* Set frame format: 8data, 1stop bit */
		UCSR1C = ((USARTx.USART_STOP_BIT-1)<<USBS1)|(3<<UCSZ10);
	}
}

void USART0_Transmit_char(unsigned char data)
{
	/* Wait for empty transmit buffer */
	while ( !( UCSR0A & (1<<UDRE0)) );
	/* Put data into buffer, sends the data */
	UDR0 = data;
}

void USART1_Transmit_char(unsigned char data)
{
	/* Wait for empty transmit buffer */
	while ( !( UCSR1A & (1<<UDRE1)) );
	/* Put data into buffer, sends the data */
	UDR1 = data;
}

void USART0_Transmit_int(int i)
{
	char string[7] = {0,0,0,0,0,0,0};
	itoa(i, string, 10);
	for(i=0; i<7; i++)
	{
		if(string[i])
		{
			USART0_Transmit_char(string[i]);
		}
	}
}

void USART1_Transmit_int(int i)
{
	char string[7] = {0,0,0,0,0,0,0};
	itoa(i, string, 10);
	for(i=0; i<7; i++)
	{
		if(string[i])
		{
			USART1_Transmit_char(string[i]);
		}
	}
}

void USART0_Transmit_uint(unsigned int i)
{
	char string[6] = {0,0,0,0,0,0};
	utoa(i,string,10);
	for(i=0; i<5; i++)
	{
		if(string[i])
		{
			USART0_Transmit_char(string[i]);
		}
	}
}

void USART1_Transmit_uint(unsigned int i)
{
	char string[6] = {0,0,0,0,0,0};
	utoa(i,string,10);
	for(i=0; i<5; i++)
	{
		if(string[i])
		{
			USART1_Transmit_char(string[i]);
		}
	}
}

void USART0_Transmit_long(long i)
{
	char string[11] = {0,0,0,0,0,0,0,0,0,0,0};
	ltoa(i, string, 10);
	for(i=0; i<11; i++)
	{
		if(string[i])
		{
			USART0_Transmit_char(string[i]);
		}
	}
}

void USART1_Transmit_long(long i)
{
	char string[11] = {0,0,0,0,0,0,0,0,0,0,0};
	ltoa(i, string, 10);
	for(i=0; i<11; i++)
	{
		if(string[i])
		{
			USART1_Transmit_char(string[i]);
		}
	}
}

void USART0_Transmit_ulong(unsigned long i)
{
	char string[11] = {0,0,0,0,0,0,0,0,0,0,0};
	ltoa(i, string, 10);
	for(i=0; i<10; i++)
	{
		if(string[i])
		{
			USART0_Transmit_char(string[i]);
		}
	}
}

void USART1_Transmit_ulong(unsigned long i)
{
	char string[11] = {0,0,0,0,0,0,0,0,0,0,0};
	ltoa(i, string, 10);
	for(i=0; i<10; i++)
	{
		if(string[i])
		{
			USART1_Transmit_char(string[i]);
		}
	}
}

void USART0_Transmit_string(char *str)
{
	while(*str)
	{
		USART0_Transmit_char(*str++);
	}
}

void USART1_Transmit_string(char *str)
{
	while(*str)
	{
		USART1_Transmit_char(*str++);
	}
}

unsigned char USART0_Receive_char(void)
{
	while(!(UCSR0A & (1<<RXC0)));
	return UDR0;
}

unsigned char USART1_Receive_char(void)
{
	while(!(UCSR1A & (1<<RXC1)));
	return UDR1;
}

void USART0_Receive_flush(void)
{
	unsigned char dummy;
	while ( UCSR0A & (1<<RXC0) ) dummy = UDR0;
}

void USART1_Receive_flush(void)
{
	unsigned char dummy;
	while ( UCSR1A & (1<<RXC1) ) dummy = UDR1;
}
