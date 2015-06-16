// Supply Monitor Message Definitions
#include "common_message_types.h"
#include <stdint.h>

#ifndef SUPPLY_MONITOR_MSGS_H
#define SUPPLY_MONITOR_MSGS_H

// This file must be included from project_message_types.h after appropriate 
// #defines are issued.  This file should not be included from ANY file other 
// than common_message_types.h.

#ifndef TYPE_SUPPLY_MONITOR_VAL
  #error "Define unique TYPE_SUPPLY_MONITOR_VAL for kTypeSupplyMonitor before inclusion."
#endif

#ifndef TYPE_SUPPLY_REPORT_VAL
  #error "Define unique TYPE_SUPPLY_REPORT_VAL for kTypeSupplyReport before inclusion."
#endif

////////////////////////////////////////////////////////////////////////////////
// Type
const uint8_t kTypeSupplyMonitor = TYPE_SUPPLY_MONITOR_VAL;
// Subtypes
const uint8_t kSubtypeVoltsRaw        = 0;
const uint8_t kSubtypeAmpsRaw         = 1;

const uint8_t kSubtypeVolts           = 2;
const uint8_t kSubtypeAmps            = 3;
const uint8_t kSubtypeWatts           = 4;
const uint8_t kSubtypeJoules          = 5;

const uint8_t kSubtypeVoltsFilt       = 6;
const uint8_t kSubtypeAmpsFilt        = 7;
const uint8_t kSubtypeWattsFilt       = 8;
//const uint8_t kSubtypeJoulesFilt      = 9;  // unused

//const uint8_t kSubtypeVoltsBias       = 10; // unused
const uint8_t kSubtypeVoltsGain       = 11;
const uint8_t kSubtypeAmpsBias        = 12;
const uint8_t kSubtypeAmpsGain        = 13;


const uint8_t kSubtypeResetJoules     = 15;

// type | sub | direction    | value
// -------------------------------------------------------------
// X    |  0   | 0            | volts_raw     (int32)
// X    |  1   | 0            | amps_raw      (int32)
// X    |  2   | 0            | volts         (float)
// X    |  3   | 0            | amps          (float)
// X    |  4   | 0            | watts         (float)
// X    |  5   | 0            | joules        (float)
// X    |  6   | 0            | volts_filt    (float)
// X    |  7   | 0            | amps_filt     (float)
// X    |  8   | 0            | watts_filt    (float)
// X    |  9   | 0            | joules_filt   (float)   // unused
// X    | 10   | uint8 access | volts_bias    (float)   // unused
// X    | 11   | uint8 access | volts_gain    (float)
// X    | 12   | uint8 access | amps_bias     (float)
// X    | 13   | uint8 access | amps_gain     (float)
// X    | 14   | ?            | ????????????  (float)   // Unused
// X    | 15   | 1            | N/A                     // reset joule counter to zero
// 'access' is 0-get, 1-set, 2-save, 3-reply

////////////////////////////////////////////////////////////////////////////////
// Type
const uint8_t kTypeSupplyReport = TYPE_SUPPLY_REPORT_VAL;
// Struct Definition
typedef struct __attribute__ ((__packed__)) {
  float t;
  float volts;
  float amps;
  float watts;
  float joules;
} MsgSupplyReport;

#endif // SUPPLY_MONITOR_MSGS_H