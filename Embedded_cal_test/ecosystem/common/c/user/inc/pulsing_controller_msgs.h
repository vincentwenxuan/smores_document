// Pulsing Controller Message Definitions
#include "common_message_types.h"
#include <stdint.h>

#ifndef PULSING_CONTROLLER_MSGS_H
#define PULSING_CONTROLLER_MSGS_H

// This file must be included from common_message_types.h after appropriate 
// #defines are issued.  This file should not be included from ANY file other 
// than project_message_types.h.

#ifndef TYPE_PULSING_CONTROLLER_VAL
  #error "Define unique TYPE_PULSING_CONTROLLER_VAL for kTypePulsingController before inclusion."
#endif

#ifndef TYPE_PULSING_CONTROLLER_STATE_VAL
  #error "Define unique TYPE_PULSING_CONTROLLER_STATE_VAL for kTypePulsingControllerState before inclusion."
#endif


////////////////////////////////////////////////////////////////////////////////
// Type
const uint8_t kTypePulsingController = TYPE_PULSING_CONTROLLER_VAL;
// Subtypes
const uint8_t kSubtypeCmdPulsePhase     = 0;
const uint8_t kSubtypeCmdVoltsPulseAmp  = 1;

// type | sub | direction    | value
// -------------------------------------------------------------
// X    |  0  | get/set/reply | pulse_phase (float)
// X    |  1  | get/set/reply | volts_pulse_amp (float)
// 'access' is 0-get, 1-set, 2-save, 3-reply


////////////////////////////////////////////////////////////////////////////////
// Type
const uint8_t kTypePulsingControllerState = TYPE_PULSING_CONTROLLER_STATE_VAL;
// Struct Definition
typedef struct __attribute__ ((__packed__)) {
  float t;                    // system clock time
  float cmd_volts;            // commanded baseline pwm
  float cmd_volts_pulse_amp;  // commanded sin pulse amplitude
  float cmd_pulse_phase;      // commanded sin pulse phase
  float pwm;                  // instantaneous output pwm
  float speed;                // filtered measured speed
  float position;             // instantaneous position
} MsgPulsingControllerState;

#endif // PULSING_CONTROLLER_MSGS_H