#ifndef COAX_OPEN_CONTROLLER_MSGS_H
#define COAX_OPEN_CONTROLLER_MSGS_H

#ifndef TYPE_COAX_OPEN_CONTROLLER_VAL
  #error "Define unique TYPE_COAX_OPEN_CONTROLLER_VAL for kTypeCoaxOpenController before inclusion."
#endif

#ifndef TYPE_COAX_OPEN_CONTROLLER_PARAM_VAL
  #error "Define unique TYPE_COAX_OPEN_CONTROLLER_PARAM_VAL for kTypeCoaxOpenControllerParam before inclusion."
#endif


////////////////////////////////////////////////////////////////////////////////
// Type
const uint8_t kTypeCoaxOpenController = TYPE_COAX_OPEN_CONTROLLER_VAL;
// Subtypes
const uint8_t kSubtypeCoaxOpenControllerF0Yaw                   =  0;
const uint8_t kSubtypeCoaxOpenControllerF1Yaw                   =  1;
const uint8_t kSubtypeCoaxOpenControllerF2Yaw                   =  2;

// type | subtype                    |       ---XXX---             |
// -----------------------------------------------------------------
// X    |  0 - F0Yaw                  | uint8 access | float        |
// X    |  1 - F1Yaw                  | uint8 access | float        |
// X    |  2 - F2Yaw                  | uint8 access | float        |
// 'access' is 0-get, 1-set, 2-save, 3-reply

////////////////////////////////////////////////////////////////////////////////
// Type
const uint8_t kTypeCoaxOpenControllerParam = TYPE_COAX_OPEN_CONTROLLER_PARAM_VAL;
// Struct Definition
typedef struct __attribute__ ((__packed__)) {
  float f0_yaw;     // model feedforward polynomial (volts)
  float f1_yaw;     // model feedforward polynomial (volts/volt)
  float f2_yaw;     // model feedforward polynomial (volts/volts^2)
} MsgCoaxOpenControllerParam;

#endif // COAX_OPEN_CONTROLLER_MSGS_H