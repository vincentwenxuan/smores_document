/*
 * Name: main.c
 * Created: 8/6/2014
 * Author: Chao Liu (chao.liu0307@gmail.com)
 */

#include "m_general.h"
//#include "USART.h"
#include "drive_coil.h"
#include "TWI.h"



#define I2C_ADDR (0x0A)

// other:
#define I2C_UNDEFINED        0xF2  // no state information available
#define I2C_ERROR            0xF1  // an error occurred on the I2C bus
#define M_BLUE_TOGGLE        0xA0  // command to toggle the blue led
#define M_GREEN_TOGGLE       0xA1  // command to toggle the green led 
#define M_YELLOW_TOGGLE      0xA2  // command to toggle the yellow led

void process_command(void);

int main(void)
{
    /* insert your hardware initialization here */

	/* m_yellow(ON); */
	
	DIRECTION coil1_direction;
	DIRECTION coil2_direction;
	DIRECTION coil3_direction;
	DIRECTION coil4_direction;
	
	/* USART USART_CHANNEL; */
	/* USART_CHANNEL.USART_BAUDRATE = 4800; */
	/* USART_CHANNEL.USART_RECEIVER_STATUS = RECEIVER_ENABLE; */
	/* USART_CHANNEL.USART_TRANSMITTER_STATUS = TRANSMITTER_ENABLE; */
	/* USART_CHANNEL.USART_STOP_BIT = 1; */
    /* USART_CHANNEL.USART_PARITY = PARITY_EVEN; */
	
	//USART_Init(USART_CHANNEL);

    
    // drive coil part
	unsigned int drive_coil_frequency = 167;
	float drive_coil_duty_cycle = 0.5;
	unsigned int drive_coil_pulse_number = 3;
	
	init_coil(drive_coil_frequency, drive_coil_duty_cycle, drive_coil_pulse_number);    // initial the coil
	coil1_direction = STOP;
	coil2_direction = STOP;
	coil3_direction = STOP;
	coil4_direction = STOP;
	drive_coil1(coil1_direction);
	drive_coil2(coil2_direction);
	drive_coil3(coil3_direction);
	drive_coil4(coil4_direction);   // stop all the coils at the begining

	twi_init(I2C_ADDR);
	
    for(;;){
		/* m_green(TOGGLE); */
		/* drive_coil3(MINUS); */
		/* m_wait(4000); */

		if(receive_done)
		{
			process_command();
			receive_done = false;
			TWI_ACK();
		}
		
    }
    return 0;   /* never reached */
}


void process_command(void)
{
    if (twi_data.data_length > 255) // in the m_twi.h, the buffer size is defined as 255, you can change this value
    {
        twi_data.command = I2C_ERROR;
        twi_data.data_length = 0;
        return;
    }
    
    switch (twi_data.command)
    {
            
        case M_BLUE_TOGGLE:
            m_blue(TOGGLE);
            twi_data.command=M_BLUE_TOGGLE;
            twi_data.data_length=5;
            twi_data.data[0]=1;
            twi_data.data[1]=2;
            twi_data.data[2]=3;
            twi_data.data[3]=4;
            twi_data.data[4]=5;
            break;
            
        case M_GREEN_TOGGLE:
            m_green(TOGGLE);
            twi_data.command=M_GREEN_TOGGLE;
            twi_data.data_length=4;
            twi_data.data[0]=5;
            twi_data.data[1]=6;
            twi_data.data[2]=7;
            twi_data.data[3]=8;
            break;
            
        /* case M_YELLOW_TOGGLE: */
        /*     m_yellow(TOGGLE); */
        /*     break; */

    }
}

