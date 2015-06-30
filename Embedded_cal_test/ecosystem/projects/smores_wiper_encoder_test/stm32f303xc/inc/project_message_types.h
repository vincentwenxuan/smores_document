#ifndef PROJECT_MESSAGE_TYPES_H
#define PROJECT_MESSAGE_TYPES_H

#include "common_message_types.h"

const uint8_t MsgCmdType = 101;
typedef struct __attribute__ ((__packed__)) {
    uint16_t cmd;
} MsgCmd;

const uint8_t MsgStatesType = 102;
typedef struct __attribute__ ((__packed__)) {
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
    uint16_t cmd;
    int16_t goal_position;
    int32_t goal_velocity;
} MsgControl;

const uint8_t MsgCurrentStateType = 104;
typedef struct __attribute__ ((__packed__)) {
    int16_t current_position;
    int32_t current_velocity;
    int8_t  motor_cmd;
} MsgCurrentState;

#endif // PROJECT_MESSAGE_TYPES_H
