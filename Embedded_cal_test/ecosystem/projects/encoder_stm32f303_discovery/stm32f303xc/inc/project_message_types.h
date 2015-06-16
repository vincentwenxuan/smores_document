#ifndef PROJECT_MESSAGE_TYPES_H
#define PROJECT_MESSAGE_TYPES_H

#include "common_message_types.h"

#define msg_muscle_state  0x0A// stm 32 state 
typedef struct __attribute__ ((__packed__))
{
	int16_t face1_pos; 
	int16_t face1_vel; 
	int16_t face1_acc; 
	int16_t face2_pos; 
	int16_t face2_vel; 
	int16_t face2_acc; 
	int16_t face3_pos; 
	int16_t face3_vel;
	int16_t face3_acc; 
	int16_t face4_pos; 
	int16_t face4_vel; 
	int16_t face4_acc;  

}muscle_state;

#define msg_muscle_connection_state 0x0B
typedef struct __attribute__ ((__packed__))
{
	uint8_t face1_connection; // either true or false
	int16_t face1_connection_orientation; 
	uint8_t face2_connection;// true or false 
	int16_t face2_connection_orientation; 
	uint8_t face3_connection;
	int16_t face3_connection_orientation; 
	uint8_t face4_connection; 
	int16_t face4_connection_orienation;
} muscle_connection_state;

#define msg_connection_command  0x0C
typedef struct __attribute__ ((__packed__))
{
	uint8_t face1_con_discon; // 1 to connect 0 to disconnect 
	uint8_t face2_con_discon;
	uint8_t face3_con_discon;
	uint8_t face4_con_discon;

}connection_command;

#define msg_pos_command 0x0D
typedef struct __attribute__ ((__packed__))
{
	int16_t muscle_pos; 
}pos_command;


#define msg_face_state	0x1A				
typedef struct __attribute__ ((__packed__))
{
	int16_t pos;
	int16_t vel;
	int16_t acc;
}face_state;

#define msg_conn_state	 0x1B				
typedef struct __attribute__ ((__packed__)){
	uint8_t face_num;
	uint8_t coil_num;
}connection_state;




#endif // PROJECT_MESSAGE_TYPES_H