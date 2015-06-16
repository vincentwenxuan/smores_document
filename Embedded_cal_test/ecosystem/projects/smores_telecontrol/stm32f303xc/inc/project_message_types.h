#ifndef PROJECT_MESSAGE_TYPES_H
#define PROJECT_MESSAGE_TYPES_H

#include "common_message_types.h"

const uint8_t MsgCmdType = 101;
typedef struct __attribute__ ((__packed__)) {
    uint16_t cmd;
} MsgCmd;

const uint8_t MsgDofType = 104;
typedef struct __attribute__ ((__packed__)) {
    uint8_t DoF;
    uint8_t direction;
    uint8_t speed;
}MsgDof;

const uint8_t MsgMagnetType = 105;
typedef struct __attribute__ ((__packed__)) {
    uint8_t face;
    uint8_t state;
}MsgMagnet;

const uint8_t MsgModuleType = 102;
typedef struct __attribute__ ((__packed__)) {
  uint16_t data;
} MsgModule;

#endif // PROJECT_MESSAGE_TYPES_H
