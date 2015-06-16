#ifndef PROJECT_MESSAGE_TYPES_H
#define PROJECT_MESSAGE_TYPES_H

#include "common_message_types.h"

const uint8_t kTypeBodeRequest = 191;
typedef struct __attribute__ ((__packed__)) {
  float corner_f;
  float sample_f;
  float test_f;
  float test_a;
} MsgBodeRequest;

const uint8_t kTypeBodeResult = 192;
typedef struct __attribute__ ((__packed__)) {
  float gain;
} MsgBodeResult;

#endif // PROJECT_MESSAGE_TYPES_H