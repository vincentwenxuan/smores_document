// Polling Reporter Message Definitions
#include <stdint.h>

#ifndef POLLING_REPORTER_MSGS_H
#define POLLING_REPORTER_MSGS_H

// This file must be included from project_message_types.h after appropriate 
// #defines are issued.  This file should not be included from ANY file other 
// than project_message_types.h.

#ifndef TYPE_POLLING_REPORTER_VAL
  #error "Define unique TYPE_POLLING_REPORTER_VAL for kTypePollingReporter in project_message_types.h before inclusion."
#endif

#ifndef TYPE_POLLING_REPORTER_PARAM_VAL
  #error "Define unique TYPE_POLLING_REPORTER_PARAM_VAL for kTypePollingReporterParam before inclusion."
#endif

#ifndef TYPE_POLLING_REPORTER_REQUEST_VAL
  #error "Define unique TYPE_POLLING_REPORTER_REQUEST_VAL for kTypePollingReporterRequest before inclusion."
#endif

#ifndef TYPE_MOTOR_STATE_VAL
  #error "Define unique TYPE_MOTOR_STATE_VAL for kTypeMotorState before inclusion."
#endif

#ifndef TYPE_TEST_STAND_VAL
  #error "Define unique TYPE_TEST_STAND_VAL for kTypeTestStand before inclusion."
#endif


////////////////////////////////////////////////////////////////////////////////
// Type
const uint8_t kTypePollingReporter = TYPE_POLLING_REPORTER_VAL;
// Subtypes
const uint8_t kSubtypeStreamFreq 				= 0;
const uint8_t kSubtypeStreamEnabled 			= 1;
const uint8_t kSubtypeTypeTrigger               = 2;
const uint8_t kSubtypeSendMotorReport           = 3;
const uint8_t kSubtypeSendBatteryReport         = 4;
const uint8_t kSubtypeSendTestStand 			= 5;


// type | subtype                 |          ---XXX---         |
// -------------------------------------------------------------
// X  |  0 - StreamFreq           | access | float (Hz)  |
// X  |  1 - StreamEnabled        | access | uint8 {0,1} |
// X  |  2 - TypeTrigger          | access | uint8 {0,1} |
// X  |  3 - MotorReport          | access | uint8 {0,1} |
// X  |  4 - BatteryReport        | access | uint8 {0,1} |
// X  |  5 - TestStand            | access | uint8 {0,1} |
// 'access' is 0-get, 1-set, 2-save, 3-reply, format uint8


////////////////////////////////////////////////////////////////////////////////
// Type
const uint8_t kTypePollingReporterParam = TYPE_POLLING_REPORTER_PARAM_VAL;
// Struct Definition
typedef struct __attribute__ ((__packed__)) {
  float   stream_freq;
  uint8_t stream_enabled;
  uint8_t type_trigger;
  uint8_t motor;
  uint8_t battery;
  uint8_t test_stand;
} MsgPollingReporterParam;


////////////////////////////////////////////////////////////////////////////////
// Type
const uint8_t kTypePollingReporterRequest = TYPE_POLLING_REPORTER_REQUEST_VAL;
// Subtypes: None


////////////////////////////////////////////////////////////////////////////////
// Type
const uint8_t kTypeTestStand = TYPE_TEST_STAND_VAL;
// Struct Definition
typedef struct __attribute__ ((__packed__)) {
  float t;
  float volts;
  float amps;
  float watts;
  float joules;
  float pwm;
  float speed;
  float position;
} MsgTestStand;


#endif // POLLING_REPORTER_MSGS_H