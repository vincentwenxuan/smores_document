/*
 * SMORES_platform_config.h
 *
 * Created: 9/8/2014 2:39:32 PM
 *  Author: Chaitanya Bhargava (cbhar@seas.upenn.edu)
 */ 


#ifndef SMORES_PLATFORM_CONFIG_H_
#define SMORES_PLATFORM_CONFIG_H_

#include "m_general.h"
/*-------------------------USART----------------------*/
#define SMORES_baudrate			9600 
#define SMORES_stop_bit			1
#define SMORES_parity			PARITY_EVEN
/*-------------------------TWI------------------------*/
#define I2C_add_config_DDR				DDRB
#define I2C_add_config_PIN				PINB	
#define I2C_add_config_PORT				PORTB
#define I2C_add_config_PIN_num1			6
#define I2C_add_config_PIN_num2			7

#define Base_face_add					0x0A //Face address will be 0A, 0B, 0C, 0D depending 
											//on the status of I2C address config pins	

/*-------------------------ADC------------------------*/
#define SMORES_ADC0						ADC0
#define SMORES_ADC1						ADC1
#define SMORES_ADC_PRESCALER			64			
/*-------------------------PWM------------------------*/
#define SMORES_carrier_PWM				OC1A			
/*--------------------coil_comm-----------------------*/
#define Shut_down_RF_DDR				DDRB
#define Shut_down_RF_PORT				PORTB
#define Shut_down_RF_PIN				3					

#endif /* SMORES_PLATFORM_CONFIG_H_ */