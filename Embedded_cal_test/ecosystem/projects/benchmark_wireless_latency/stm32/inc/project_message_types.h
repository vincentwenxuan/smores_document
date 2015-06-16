#ifndef PROJECT_MESSAGE_TYPES_H
#define PROJECT_MESSAGE_TYPES_H

#include "common_message_types.h"
#include <stdint.h>

////////////////////////////////////////////////////////////////////////////////
// Type
const uint8_t kTypeStampedCounterMOSI = 201;
// Struct Definition
typedef struct __attribute__ ((__packed__)) {
  uint32_t count;
} MsgStampedCounterMOSI;

////////////////////////////////////////////////////////////////////////////////
// Type
const uint8_t kTypeStampedCounterMISO = 202;
// Struct Definition
typedef struct __attribute__ ((__packed__)) {
  int32_t sec;
  int32_t nsec;
  uint32_t count;
} MsgStampedCounterMISO;


#endif // PROJECT_MESSAGE_TYPES_H