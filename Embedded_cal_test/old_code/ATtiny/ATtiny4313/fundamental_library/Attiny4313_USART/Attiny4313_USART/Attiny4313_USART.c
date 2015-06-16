/*
 * Attiny4313_USART.c
 *
 * Created: 6/19/2014 7:36:18 AM
 *  Author: Chao Liu (chao.liu0307@gmail.com)
 */ 

#include "m_general.h"
#include "USART.h"

int main(void)
{
	m_clockdivide(0);
	//unsigned char data_char = 230;
	unsigned int data_int = 500;
	//unsigned char data_receive;
	
	m_green(ON);
	m_yellow(ON);
	m_white(ON);
	m_blue(ON);
	USART_Init();
	while(1)
	{
		//TODO:: Please write your application code	
		//USART_Transmit_char(data_char);
		USART_Transmit_int(data_int);	
		//USART_Transmit_string("Hello ModLab!\n\r");
		//data_receive = USART_Receive_char();
		//USART_Transmit_char(data_receive);
	}
}