/* Name: main.c
 * Author: <insert your name here>
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

#include "common_peripherals.h"
#include "TWI_slave.h"
#include "wiper_encoder.h"

#define I2C_ADDR (0x0A)   // top 0x0D, left 0x0A, right 0x0C

// other:
#define I2C_UNDEFINED        0xF2  // no state information available
#define I2C_ERROR            0xF1  // an error occurred on the I2C bus
#define M_BLUE_TOGGLE        0xA0  // command to toggle the blue led and send back some data
#define SEND_ADC_DATA       0xA6  // command to toggle the green led and send back some data
#define M_BLUE_ON            0xA2  // command to turn on the blue led
#define M_GREEN_ON           0xA3  // command to turn on green led
#define SEND_STATES          0xA4


void process_command(void);
unsigned char messageBuf[TWI_BUFFER_SIZE];


int main(void)
{
    /* insert your hardware initialization here */
    m_clockdivide(0);
    InitPeripherals();
    
    TWI_Slave_Initialise(I2C_ADDR<<TWI_ADR_BITS);
    sei();
    TWI_Start_Transceiver();

    init_wiper_encoder();
    
    m_green(ON);m_yellow(ON);
    m_wait(1000);
    m_green(OFF);m_yellow(OFF);
    
    for(;;){
        /* insert your main loop code here */
        process_command();
    }
    return 0;   /* never reached */
}


void process_command(void)
{
    unsigned char twi_data_1[7];
    unsigned char twi_data_2[11];
    unsigned char twi_data_3[7];
    int current_position = 0;
    long current_velocity = 0;
    int adc0 = 0;
    int adc1 = 0;
    if ( ! TWI_Transceiver_Busy() )
    {
        if ( TWI_statusReg.RxDataInBuf )
        {
            TWI_Get_Data_From_Transceiver(messageBuf, 1);
            switch (messageBuf[0])
            { 
            case M_BLUE_TOGGLE:
                m_blue(TOGGLE);
                twi_data_1[0]=M_BLUE_TOGGLE;
                twi_data_1[1]=5;
                twi_data_1[2]=1;
                twi_data_1[3]=2;
                twi_data_1[4]=3;
                twi_data_1[5]=4;
                twi_data_1[6]=5;
                TWI_Start_Transceiver_With_Data(twi_data_1, 7);
                break;
                
            case SEND_ADC_DATA:
                m_green(TOGGLE);
                current_position = return_current_position();
                current_velocity = return_current_velocity();
                adc0 = return_adc0();
                adc1 = return_adc1();
                twi_data_2[0]=(current_velocity & 0x000000FF);
                twi_data_2[1]=((current_velocity & 0x0000FF00)>>8);
                twi_data_2[2]=((current_velocity & 0x00FF0000)>>16);
                twi_data_2[3]=((current_velocity & 0xFF000000)>>24);
                twi_data_2[4]=((current_position) & 0x00FF);
                twi_data_2[5]=((current_position & 0xFF00)>>8);
                twi_data_2[6]=(adc0 & 0x00FF);
                twi_data_2[7]=((adc0 & 0xFF00)>>8);
                twi_data_2[8]=(adc1 & 0x00FF);
                twi_data_2[9]=((adc1 & 0xFF00)>>8);
                /* twi_data_2[2]=(adc_value[0] & 0x00FF); */
                /* twi_data_2[3]=((adc_value[0] & 0xFF00)>>8); */
                /* twi_data_2[4]=(adc_value[1] & 0x00FF); */
                /* twi_data_2[5]=((adc_value[1] & 0xFF00)>>8); */
                twi_data_2[10]=0xAA;
                TWI_Start_Transceiver_With_Data(twi_data_2, 11);
                break;

            case SEND_STATES:
                current_position = return_current_position();
                current_velocity = return_current_velocity();
                twi_data_3[0]=(current_velocity & 0x000000FF);
                twi_data_3[1]=((current_velocity & 0x0000FF00)>>8);
                twi_data_3[2]=((current_velocity & 0x00FF0000)>>16);
                twi_data_3[3]=((current_velocity & 0xFF000000)>>24);
                twi_data_3[4]=((current_position) & 0x00FF);
                twi_data_3[5]=((current_position & 0xFF00)>>8);
                twi_data_3[6]=0xAA;
                TWI_Start_Transceiver_With_Data(twi_data_3, 7);
                break;
            
            case M_GREEN_ON:
                m_green(ON);
                break;
	   
            case M_BLUE_ON:
                m_blue(ON);
                break;
            }
        }
    }
}


