#ifndef PROJECT_MESSAGE_TYPES_H
#define PROJECT_MESSAGE_TYPES_H

#include "common_message_types.h"

const uint8_t kTypeAnticogData = 128;
typedef struct __attribute__ ((__packed__)) {
  uint8_t cmd;
  uint16_t index;
  float value;
} MsgAnticogData;

const uint8_t kTypeAnticog = 129;
typedef struct __attribute__ ((__packed__)) {
  uint8_t cmd; // 0 = erase
} MsgAnticog;


#endif // PROJECT_MESSAGE_TYPES_H