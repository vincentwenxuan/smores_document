/*
 * SMORES_message_types.h
 *
 * Created: 9/10/2014 2:39:32 PM
 *  Author: Chaitanya Bhargava (cbhar@seas.upenn.edu)
 */

#ifndef SMORES_MESSAGE_TYPES_H_
#define SMORES_MESSAGE_TYPES_H_

#include "m_general.h"
#define msg_face_state	0x1A				//I2C command
typedef struct __attribute__ ((__packed__)){
	int16_t pos;
	int16_t vel;
	int16_t acc;
}face_state;

#define msg_conn_state	 0x1B				//only filled if told to become 
typedef struct __attribute__ ((__packed__)){//receiver earlier
	uint8_t isconnected;
	uint8_t face_num;
	uint8_t coil_num;
}connection_state;

#define msg_params		 0x1C	
typedef struct __attribute__ ((__packed__)){
	COIL_NUM receiver_coil;
	uint16_t nos_times_usart_send;
	uint16_t receiver_on_timeout;			//in seconds
	int filter_parameter1;
	int filter_parameter2;
}face_parameters;

#define msg_coils_onoff_params	 0x1D				
typedef struct __attribute__ ((__packed__)){
	uint8_t high_time; //in ms				//Sticking to uint8s for i2c
	uint8_t low_time;						//simplicity
	uint8_t nos_pulses;
}coils_onoff_parameters;
		
#define msg_coils_on	 0x1E	

#define msg_coils_off	 0x1F	

#define msg_usartsender	 0x20			//Send data over coils for neighbors to pick up

#define msg_usartlistener 0x21 

#endif /* SMORES_MESSAGE_TYPES_H_ */