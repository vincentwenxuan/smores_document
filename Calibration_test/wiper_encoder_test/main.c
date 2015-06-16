/* Name: main.c
 * Author: Chao Liu (chao.liu0307@gmail.com)
 * Date: August 12, 2014
 */

//ATmega88 TWI I2C Slave Code

#include "common_peripherals.h"
#include "TWI_slave.h"
#include "ADC.h"

#define I2C_ADDR (0x0D)

// other:
#define I2C_UNDEFINED        0xF2  // no state information available
#define I2C_ERROR            0xF1  // an error occurred on the I2C bus
#define M_BLUE_TOGGLE        0xA0  // command to toggle the blue led and send back some data
#define SEND_ADC_DATA       0xA1  // command to toggle the green led and send back some data
#define M_BLUE_ON            0xA2  // command to turn on the blue led
#define M_GREEN_ON           0xA3  // command to turn on green led

int adc_value[2];

void process_command(void);
unsigned char messageBuf[TWI_BUFFER_SIZE];

int main (void)
{
    InitPeripherals();
    m_clockdivide(0);

    TWI_Slave_Initialise(I2C_ADDR<<TWI_ADR_BITS);
    sei();
    TWI_Start_Transceiver();
    
    ADC_Init ADC_test;
    ADC_test.ADC_CHANNEL = ADC0;
    ADC_test.ADC_REF = VCC;
    ADC_test.ADC_PRESCALAR = 64;
    ADC_test.ADC_MODE = FREE_RUN;
    
    adc_init(ADC_test);
    
    adcInterruptEnable;
    adcEnable;
    adcAutoTriggerEnable;
    adcStart;
	
    while(1)
    {
        
        process_command();
    }
}


void process_command(void)
{
    if ( ! TWI_Transceiver_Busy() )
    {
        if ( TWI_statusReg.RxDataInBuf )
        {
            TWI_Get_Data_From_Transceiver(messageBuf, 1);
            switch (messageBuf[0])
            { 
            case M_BLUE_TOGGLE:
                m_blue(TOGGLE);
                unsigned char twi_data_1[7];
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
                unsigned char twi_data_2[7];
                twi_data_2[0]=SEND_ADC_DATA;
                twi_data_2[1]=5;
                twi_data_2[2]=(adc_value[0] & 0x00FF);
                twi_data_2[3]=((adc_value[0] & 0xFF00)>>8);
                twi_data_2[4]=(adc_value[1] & 0x00FF);
                twi_data_2[5]=((adc_value[1] & 0xFF00)>>8);
                twi_data_2[6]=0xAA;
                TWI_Start_Transceiver_With_Data(twi_data_2, 7);
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

ISR(ADC_vect)
{
    static ADC_Channel ADC_channel = ADC0;
    if (ADC_channel == ADC0) {
        adc_value[0] = ADC;
    } else
    {
        adc_value[1] = ADC;
    }
    int i = 0;
    for (i = 0; i < 375; i++) {
        m_yellow(TOGGLE);
    }

    ADC_channel = !ADC_channel;
    adc_set_channel(ADC_channel);
    set(ADCSRA, ADIF);
}



