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
    int16_t velocity;
    int16_t position;
    int16_t adc0;
    int16_t adc1;
} MsgStates;



#endif // PROJECT_MESSAGE_TYPES_H
