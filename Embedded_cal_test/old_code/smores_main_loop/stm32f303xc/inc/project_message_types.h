#ifndef PROJECT_MESSAGE_TYPES_H
#define PROJECT_MESSAGE_TYPES_H

#include "common_message_types.h"
// const uint8_t MsgMyDataSpecType = 101;
// typedef struct __attribute__ ((__packed__)) {
//   uint16_t cmd;
// } MsgMyDataSpec;
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

const uint8_t MsgMyDataType = 102;
typedef struct __attribute__ ((__packed__)) {
  uint16_t data;
} MsgMyData;

#endif // PROJECT_MESSAGE_TYPES_H
