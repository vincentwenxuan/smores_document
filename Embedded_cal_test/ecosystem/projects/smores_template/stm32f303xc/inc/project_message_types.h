#ifndef PROJECT_MESSAGE_TYPES_H
#define PROJECT_MESSAGE_TYPES_H

#include "common_message_types.h"
const uint8_t MsgMyDataSpecType = 101;
typedef struct __attribute__ ((__packed__)) {
  uint16_t cmd;
} MsgMyDataSpec;

const uint8_t MsgMyDataType = 102;
typedef struct __attribute__ ((__packed__)) {
  uint16_t data;
} MsgMyData;

#endif // PROJECT_MESSAGE_TYPES_H
