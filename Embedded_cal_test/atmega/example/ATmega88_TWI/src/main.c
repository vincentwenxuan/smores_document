/* Name: main.c
 * Author: Chao Liu (chao.liu0307@gmail.com)
 * Date: August 12, 2014
 */

//ATmega88 TWI I2C Slave Code

#include "m_general.h"
#include "TWI.h"

#define I2C_ADDR (0x0A)


// other:
#define I2C_UNDEFINED        0xF2  // no state information available
#define I2C_ERROR            0xF1  // an error occurred on the I2C bus
#define M_BLUE_TOGGLE        0xA0  // command to toggle the blue led and send back some data
#define M_GREEN_TOGGLE       0xA1  // command to toggle the green led and send back some data
#define M_BLUE_ON            0xA2  // command to turn on the blue led
#define M_GREEN_ON           0xA3  // command to turn on green led

void process_command(void);

int main (void)
{
    m_clockdivide(0);
    twi_init(I2C_ADDR);
    while(1)
    {
        
        if (receive_done)
        {
            process_command();
            receive_done = false;
            TWI_ACK();
        }
    }
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
            
        case M_GREEN_ON:
            m_green(ON);
            break;
            
        case M_BLUE_ON:
            m_blue(ON);
            break;
    }
}



