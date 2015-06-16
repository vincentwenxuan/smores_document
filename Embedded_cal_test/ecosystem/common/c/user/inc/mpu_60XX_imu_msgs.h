#include "common_message_types.h"
#include <stdint.h>

#ifndef MPU_60XX_IMU_MSGS_H
#define MPU_60XX_IMU_MSGS_H

// This file must be included from common_message_types.h after appropriate 
// #defines are issued.  This file should not be included from ANY file other 
// than project_message_types.h.

#ifndef TYPE_IMU_VAL
  #error "Define unique TYPE_IMU_VAL for kTypeImu before inclusion."
#endif


////////////////////////////////////////////////////////////////////////////////
// Type
const uint8_t kTypeImu = TYPE_IMU_VAL;
// Subtypes
const uint8_t kSubtypeWxScale        = 0;

// type | sub | direction    | value
// -------------------------------------------------------------
// X    |  0  | get/set/reply | 
// 'access' is 0-get, 1-set, 2-save, 3-reply

////////////////////////////////////////////////////////////////////////////////
// IMU Commands
const uint8_t kTypeImuCommand = TYPE_IMU_COMMAND_VAL;
typedef struct __attribute__ ((__packed__)) {
  uint8_t command;
} MsgImuCommand;

////////////////////////////////////////////////////////////////////////////////
// Raw IMU data fields, bit values. Not scaled or biased.
const uint8_t kTypeImuRaw = TYPE_IMU_RAW_VAL;
typedef struct __attribute__ ((__packed__)) {
  uint32_t t; // microseconds
  int16_t wx; // raw
  int16_t wy; // raw
  int16_t wz; // raw
  int16_t ax; // raw
  int16_t ay; // raw
  int16_t az; // raw
} MsgImuRaw;

////////////////////////////////////////////////////////////////////////////////
// Metric scaled IMU values, rad/s and m/s^2. Scaled and biased.
const uint8_t kTypeImuMetric = TYPE_IMU_METRIC_VAL; 
typedef struct __attribute__ ((__packed__)) {
  uint32_t t; // microseconds
  float wx;   // rad/s
  float wy;   // rad/s
  float wz;   // rad/s
  float ax;   // m/s^2
  float ay;   // m/s^2
  float az;   // m/s^2
} MsgImuMetric;

////////////////////////////////////////////////////////////////////////////////
// IMU sensor offset values.
const uint8_t kTypeImuOffset = TYPE_IMU_OFFSET_VAL;
typedef struct __attribute__ ((__packed__)) {
  int16_t ax; // raw
  int16_t ay; // raw
  int16_t az; // raw
  int16_t wx; // raw
  int16_t wy; // raw
  int16_t wz; // raw
} MsgImuOffset;

////////////////////////////////////////////////////////////////////////////////
// IMU sensor scale values.
const uint8_t kTypeImuScale = TYPE_IMU_SCALE_VAL;
typedef struct __attribute__ ((__packed__)) {
  float ax; // m/s^2
  float ay; // m/s^2
  float az; // m/s^2
  float wx; // rad/s
  float wy; // rad/s
  float wz; // rad/s
} MsgImuScale;

#endif // MPU_60XX_IMU_MSGS_H