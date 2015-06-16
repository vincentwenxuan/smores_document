/* Name: main.c
 * Author: <insert your name here>
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

#include "common_peripherals.h"
#include "TWI_slave.h"

#define SEND_SLAVE_DATA       0xA1  // command to toggle the green led and send back some data
#define M_BLUE_TOGGLE            0xA2  // command to turn on the blue led
#define M_GREEN_TOGGLE           0xA3  // command to turn on green led
#define GET_MASTER_DATA      0xA4

unsigned char messageBuf[TWI_BUFFER_SIZE];
unsigned char twi_receive[4];

void process_twi(void);

int main(void)
{
	/* insert your hardware initialization here */
    InitPeripherals();
	m_clockdivide(0);

	unsigned char TWI_SlaveAddress = 0x0A;
	TWI_Slave_Initialise(TWI_SlaveAddress<<TWI_ADR_BITS);

	sei();
	TWI_Start_Transceiver();
	
	m_green(ON);m_yellow(ON);m_blue(ON);
	m_wait(1000);
	m_green(OFF);m_yellow(OFF);m_blue(OFF);
	m_wait(2000);
	for(;;){
		/* insert your main loop code here */
		process_twi();
		/* m_wait(10); */  // this wait affect the IIC communication
	}
	return 0;   /* never reached */
}

void process_twi(void)
{
	if ( ! TWI_Transceiver_Busy() )
	{
		if ( TWI_statusReg.RxDataInBuf )
		{
			m_yellow(TOGGLE);
			TWI_Get_Data_From_Transceiver(messageBuf, 1);
			switch(messageBuf[0])
			{
			case M_GREEN_TOGGLE:
				m_green(TOGGLE);
				break;
			case M_BLUE_TOGGLE:
				m_blue(TOGGLE);
				break;
			case GET_MASTER_DATA:
				TWI_Get_Data_From_Transceiver(twi_receive,4);
				m_blue(TOGGLE);
				break;
			case SEND_SLAVE_DATA:
				m_green(TOGGLE);
				unsigned char twi_data[6];
				twi_data[0]=SEND_SLAVE_DATA;
				twi_data[1]=4;
				twi_data[2]=twi_receive[0];
				twi_data[3]=twi_receive[1];
				twi_data[4]=twi_receive[2];
				twi_data[5]=twi_receive[3];
				/* twi_data[2] = 1; */
				/* twi_data[3] = 2; */
				/* twi_data[4] = 3; */
				/* twi_data[5] = 4; */
				TWI_Start_Transceiver_With_Data(twi_data, 6);
				break;
			}
		}
	}
}

