/*
 * USART.c
 *
 * Created: 8/6/2014 
 *  Author: Chao Liu (chao.liu0307@gmail.com)
 */ 

#include "USART.h"

void USART_Init(USART USARTx)
{
	/* Set baud rate */
	set(UCSR0A, U2X0);
	/* Set baud rate */
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
	UCSR0C = ((USARTx.USART_STOP_BIT-1)<<USBS0)|(3<<UCSZ00);
    /*
	switch (USARTx.USART_CHARACTER_SIZE)
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
    */
    UCSR0C &= 0xCF;
    switch (USARTx.USART_PARITY) {
        case PARITY_NONE:
            UCSR0C |= (0 << UPM00);
            break;
        case PARITY_EVEN:
            UCSR0C |= (2 << UPM00);
            break;
        case PARITY_ODD:
            UCSR0C |= (3 << UPM00);
            break;
        default:
            UCSR0C |= (0 << UPM00);
            break;
    }
    
}

void USART_Transmit_char(unsigned char data)
{
	/* Wait for empty transmit buffer */
	while ( !( UCSR0A & (1<<UDRE0)) );
	/* Put data into buffer, sends the data */
	UDR0 = data;
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
	while(!(UCSR0A & (1<<RXC0)));
	return UDR0;
}

void USART_Receive_flush(void)
{
	unsigned char dummy;
	while ( UCSR0A & (1<<RXC0) ) dummy = UDR0;
}

