#ifndef PROJECT_MESSAGE_TYPES_H
#define PROJECT_MESSAGE_TYPES_H

#include "common_message_types.h"


const uint8_t MsgBrainIpType = 112; 
typedef struct __attribute__ ((__packed__)){
    uint8_t type; 
    uint32_t brain_ip; 
    uint16_t brain_socket; 
}MsgBrainIp; 

const uint8_t MsgCmdType = 101;
typedef struct __attribute__ ((__packed__)) {
    uint8_t type; 
    uint16_t cmd;
} MsgCmd;

const uint8_t MsgMagnetType = 109;
typedef struct __attribute__ ((__packed__)) {
    uint8_t type; 
    uint8_t face;
    uint8_t state;
}MsgMagnet;

const uint8_t MsgModuleType = 110;
typedef struct __attribute__ ((__packed__)) {
  uint8_t type; 
  uint16_t data;
} MsgModule;

const uint8_t MsgStatesType = 102;
typedef struct __attribute__ ((__packed__)) {
    uint8_t type; 
    uint16_t time_sec;
    uint16_t time_nsec;
    int16_t encoder_counter;
    int32_t velocity;
    int16_t position;
    int16_t adc0;
    int16_t adc1;
    int8_t motor_cmd;
} MsgStates;


const uint8_t MsgControlType = 103;
typedef struct __attribute__ ((__packed__)) {
    uint8_t type; 
    uint8_t face_id;
    uint16_t cmd;
    int16_t goal_position;
    int32_t goal_velocity;
} MsgControl;

const uint8_t MsgCurrentStateType = 104;
typedef struct __attribute__ ((__packed__)) {
    uint8_t type;
    int16_t goal_position;
    int32_t goal_velocity;
    int16_t current_position;
    int32_t current_velocity;
    int8_t  motor_cmd;
} MsgCurrentState;

const uint8_t MsgTrajectoryPositionType = 105;
typedef struct __attribute__ ((__packed__)) {
    uint8_t type; 
    uint8_t face_id;
    uint16_t cmd;
    float coeff_1;
    float coeff_2;
    float coeff_3;
    float coeff_4;
    float coeff_5;
    float coeff_6;
    float duration;
} MsgTrajectoryPosition;

const uint8_t MsgTrajectoryPositionTXType = 107;
typedef struct __attribute__ ((__packed__)) {
    uint8_t type; 
    float coeff_1;
    float coeff_2;
    float coeff_3;
    float coeff_4;
    float coeff_5;
    float coeff_6;
    float duration;
} MsgTrajectoryPositionTX;

const uint8_t MsgTrajectoryVelocityType = 106;
typedef struct __attribute__ ((__packed__))  {
    uint8_t type; 
    uint8_t face_id;
    uint16_t cmd;
    float coeff_1;
    float coeff_2;
} MsgTrajectoryVelocity;

const uint8_t MsgTrajectoryVelocityTXType = 108;
typedef struct __attribute__ ((__packed__)) {
    uint8_t type; 
    float coeff_1;
    float coeff_2;
} MsgTrajectoryVelocityTX;

const uint8_t MsgTorqueType = 111;
typedef struct __attribute__ ((__packed__)) {
    uint8_t type;
    uint8_t DoF;
    uint8_t direction;
    uint8_t speed;
} MsgTorque;
 
#endif // PROJECT_MESSAGE_TYPES_H
