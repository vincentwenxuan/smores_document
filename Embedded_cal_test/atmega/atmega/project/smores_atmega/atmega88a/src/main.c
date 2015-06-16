/* Name: main.c
 * Author: <insert your name here>
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

#include "common_peripherals.h"
#include "TWI_slave.h"
#include "drive_coil.h"
#include "wiper_encoder.h"
#include "atm_cali.h"

//#define BOTTOM_FACE

unsigned char My_Address;

unsigned char messageBuf[TWI_BUFFER_SIZE];

DIRECTION coil1_direction;
DIRECTION coil2_direction;
DIRECTION coil3_direction;
DIRECTION coil4_direction;

void process_twi(void);

int main(void)
{
    /* insert your hardware initialization here */
    m_clockdivide(0);
    InitPeripherals();

    clear(DDRB, 6);
    clear(DDRB, 7);
    //unsigned char TWI_SlaveAddress = 0x0B;
    char slave_bit_0 = check(PINB, 6);
    char slave_bit_1 = check(PINB, 7);

    unsigned char TWI_SlaveAddress;
    if((slave_bit_0 == 0) && (slave_bit_1 == 0))
    {
        My_Address = BOTTOM_FACE_I2C_ADDRESS;
    }
    else if ((slave_bit_0 == 0) && (slave_bit_1 == 1))
    {
        My_Address = LEFT_FACE_I2C_ADDRESS;
    }
    else if ((slave_bit_0 == 1) && (slave_bit_1 == 0))
    {
        My_Address = RIGHT_FACE_I2C_ADDRESS;
    }
    else if ((slave_bit_0 == 1) && (slave_bit_1 == 1))
    {
        My_Address = TOP_FACE_I2C_ADDRESS;
    }

    TWI_SlaveAddress = My_Address;
    TWI_Slave_Initialise(TWI_SlaveAddress<<TWI_ADR_BITS);

    TWI_Start_Transceiver();
    
    init_wiper_encoder();
    
    unsigned int frequency = 166;
    float duty_cycle = 0.5;
    unsigned int pulse_number = 3;
    init_coil(frequency, duty_cycle, pulse_number);

    coil1_direction = STOP;
    coil2_direction = STOP;
    coil3_direction = STOP;
    coil4_direction = STOP;
    
    m_green(ON);m_yellow(ON);m_blue(ON);
    m_wait(1000);
    m_green(OFF);m_yellow(OFF);m_blue(OFF);
//    m_wait(2000);
    sei();
    for(;;){
        /* insert your main loop code here */
        process_twi();
    }
    return 0;   /* never reached */
}

void process_twi(void)
{
    unsigned char twi_data_state[7];
#ifndef BOTTOM_FACE
    unsigned char twi_data_adc[11];
#endif
#ifdef BOTTOM_FACE
    unsigned char twi_data_adc[9];
#endif
    int current_position = 0;
    long current_velocity = 0;
    int adc0 = 0;
    int adc1 = 0;
    if ( ! TWI_Transceiver_Busy() )
    {
        if ( TWI_statusReg.RxDataInBuf )
        {
            m_yellow(TOGGLE);
            TWI_Get_Data_From_Transceiver(messageBuf, 1);
            switch(messageBuf[0])
            {
            case MAGNETS_ON:
                m_green(ON);
                coil1_direction = PLUS;
                drive_coil1(coil1_direction);
                m_wait(20);
                coil2_direction = PLUS;
                drive_coil2(coil2_direction);
                m_wait(20);
                coil3_direction = PLUS;
                drive_coil3(coil3_direction);
                m_wait(20);
                coil4_direction = PLUS;
                drive_coil4(coil4_direction);
                m_wait(20);
                drive_coil1(STOP);
                drive_coil2(STOP);
                drive_coil3(STOP);
                drive_coil4(STOP);
                break;
            case MAGNETS_OFF:
                m_green(OFF);
                coil1_direction = MINUS;
                drive_coil1(coil1_direction);
                m_wait(20);
                coil2_direction = MINUS;
                drive_coil2(coil2_direction);
                m_wait(20);
                coil3_direction = MINUS;
                drive_coil3(coil3_direction);
                m_wait(20);
                coil4_direction = MINUS;
                drive_coil4(coil4_direction);
                m_wait(20);
                drive_coil1(STOP);
                drive_coil2(STOP);
                drive_coil3(STOP);
                drive_coil4(STOP);
                break;
            case FACE_PING:
                // send a response back
                ; // I need to put this in order to declare a variable next (?!?!!)
                m_blue(TOGGLE);
                unsigned char twi_data;
                twi_data = My_Address;
                TWI_Start_Transceiver_With_Data(&twi_data, 1);
                break;
            case SEND_STATES:
                current_position = return_current_position();
                current_velocity = return_current_velocity();
                twi_data_state[0]=(current_velocity & 0x000000FF);
                twi_data_state[1]=((current_velocity & 0x0000FF00)>>8);
                twi_data_state[2]=((current_velocity & 0x00FF0000)>>16);
                twi_data_state[3]=((current_velocity & 0xFF000000)>>24);
                twi_data_state[4]=((current_position) & 0x00FF);
                twi_data_state[5]=((current_position & 0xFF00)>>8);
                twi_data_state[6]=0xAA;
                TWI_Start_Transceiver_With_Data(twi_data_state, 7);
                break;

            case SEND_ADC_DATA:
                current_position = return_current_position();
                current_velocity = return_current_velocity();
#ifndef BOTTOM_FACE
                adc0 = return_adc0();
                adc1 = return_adc1();
                twi_data_adc[0]=(current_velocity & 0x000000FF);
                twi_data_adc[1]=((current_velocity & 0x0000FF00)>>8);
                twi_data_adc[2]=((current_velocity & 0x00FF0000)>>16);
                twi_data_adc[3]=((current_velocity & 0xFF000000)>>24);
                twi_data_adc[4]=((current_position) & 0x00FF);
                twi_data_adc[5]=((current_position & 0xFF00)>>8);
                twi_data_adc[6]=(adc0 & 0x00FF);
                twi_data_adc[7]=((adc0 & 0xFF00)>>8);
                twi_data_adc[8]=(adc1 & 0x00FF);
                twi_data_adc[9]=((adc1 & 0xFF00)>>8);
                /* twi_data_adc[2]=(adc_value[0] & 0x00FF); */
                /* twi_data_adc[3]=((adc_value[0] & 0xFF00)>>8); */
                /* twi_data_adc[4]=(adc_value[1] & 0x00FF); */
                /* twi_data_adc[5]=((adc_value[1] & 0xFF00)>>8); */
                twi_data_adc[10]=0xAA;
                TWI_Start_Transceiver_With_Data(twi_data_adc, 11);
#endif
#ifdef BOTTOM_FACE
                adc0 = return_adc0();
                twi_data_adc[0]=(current_velocity & 0x000000FF);
                twi_data_adc[1]=((current_velocity & 0x0000FF00)>>8);
                twi_data_adc[2]=((current_velocity & 0x00FF0000)>>16);
                twi_data_adc[3]=((current_velocity & 0xFF000000)>>24);
                twi_data_adc[4]=((current_position) & 0x00FF);
                twi_data_adc[5]=((current_position & 0xFF00)>>8);
                twi_data_adc[6]=(adc0 & 0x00FF);
                twi_data_adc[7]=((adc0 & 0xFF00)>>8);
                twi_data_adc[8]=0xAA;
                TWI_Start_Transceiver_With_Data(twi_data_adc, 9);
#endif
                break;
            }
        }
    }
}

