/*
 * ATtiny1634_USART.c
 *
 * Created: 7/23/2014 1:56:40 PM
 *  Author: Chao Liu(chao.liu0307@gmail.com)
 */ 

# define F_CPU 8000000UL
#include "m_general.h"
#include "USART.h"

int main(void)
{
	m_clockdivide(0);
	//unsigned char data_char = 230;
	unsigned int data_int = 500;

	m_yellow(ON);
	m_white(ON);
	m_blue(ON);
	USART_Channel usart1 = USART1;
	USART USART_CHANNEL;
	USART_CHANNEL.USART_channel = usart1;
	USART_CHANNEL.USART_BAUDRATE = 9600;
	USART_CHANNEL.USART_RECEIVER_STATUS = ENABLE;
	USART_CHANNEL.USART_TRANSMITTER_STATUS = ENABLE;
	USART_CHANNEL.USART_STOP_BIT = 1;

	
	USART_Init(USART_CHANNEL);
	
	while(1)
	{
		//TODO:: Please write your application code
		//USART1_Transmit_char(data_char);
		USART1_Transmit_int(data_int);
		USART1_Transmit_string("Hello ModLab!\n\r");
		//data_receive = USART1_Receive_char();
		//USART1_Transmit_char(data_receive);
	}
}