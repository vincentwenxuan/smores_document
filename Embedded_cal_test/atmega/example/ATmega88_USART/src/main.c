/*
 * Name: main.c
 * Created: 8/6/2014
 * Author: Chao Liu (chao.liu0307@gmail.com)
 */

#include "m_general.h"
#include "USART.h"

int main(void)
{
    /* insert your hardware initialization here */

	USART USART_CHANNEL;
	USART_CHANNEL.USART_BAUDRATE = 9600;
	USART_CHANNEL.USART_RECEIVER_STATUS = RECEIVER_ENABLE;
	USART_CHANNEL.USART_TRANSMITTER_STATUS = TRANSMITTER_ENABLE;
	USART_CHANNEL.USART_STOP_BIT = 1;
    USART_CHANNEL.USART_PARITY = PARITY_EVEN;
	
	USART_Init(USART_CHANNEL);
    
    unsigned int data_int = 500;
    
    for(;;){
        /* insert your main loop code here */
		//USART_Transmit_char(data_char);
		USART_Transmit_int(data_int);
		USART_Transmit_string("Hello ModLab!\n\r");
		//data_receive = USART_Receive_char();
		//USART_Transmit_char(data_receive);
    }
    return 0;   /* never reached */
}
