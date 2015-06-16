/*
 * ATtiny4313_USI_TWI_Slave.c
 *
 * Created: 6/22/2014 3:56:58 PM
 *  Author: Chao Liu(chao.liu0307@gmail.com)
 */ 


#include "m_general.h"
#include "USI_TWI_Slave.h"

int main(void)
{
	m_clockdivide(0);
	sei(); // enable interupts
	const uint8_t slaveAddress = 0x50;
	USI_TWI_Slave_Init(slaveAddress);
	while(1)
	{
		if (USI_TWI_DataIn_ReceiveBuffer()) {
			m_blue(ON);
			uint8_t flag = USI_TWI_Receive_Byte();

			if (flag != 0) {
				m_green(ON);	
			} else {
				m_yellow(ON);
			}
		}
		
		//USI_TWI_Transmit_Byte(0x20);
		
	}
}