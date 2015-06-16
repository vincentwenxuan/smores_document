// Loop Monitor Message Definitions

#ifndef LOOP_MONITOR_MSGS_H
#define LOOP_MONITOR_MSGS_H

#ifndef TYPE_LOOP_MONITOR_VAL
  #error "Define unique TYPE_LOOP_MONITOR_VAL for kTypeLoopMonitor before inclusion."
#endif

#ifndef TYPE_LOOP_PROFILE_VAL
  #error "Define unique TYPE_LOOP_PROFILE_VAL for kTypeLoopProfile before inclusion."
#endif

#ifndef TYPE_LOOP_FREQ_VAL
  #error "Define unique TYPE_LOOP_FREQ_VAL for kTypeLoopFreq before inclusion."
#endif


////////////////////////////////////////////////////////////////////////////////
// Type
const uint8_t kTypeLoopMonitor = TYPE_LOOP_MONITOR_VAL;
// Subtypes
const uint8_t kSubtypeTargetLoopFreq = 0;

// type | subtype                 |       ---XXX---       |
// ------------------------------------------------------------------
// X    | 0 - TargetLoopFreq      | uint8 access | float  |
// 'access' is 0-get, 1-set, 2-save, 3-reply


////////////////////////////////////////////////////////////////////////////////
// Type
const uint8_t kTypeLoopProfile = TYPE_LOOP_PROFILE_VAL;
// Subtypes: None


////////////////////////////////////////////////////////////////////////////////
// Type
const uint8_t kTypeLoopFreq = TYPE_LOOP_FREQ_VAL;
// Struct Definition
typedef struct __attribute__ ((__packed__)) {
  float loop_freq;
} MsgLoopFreq;


#endif // LOOP_MONITOR_MSGS_H