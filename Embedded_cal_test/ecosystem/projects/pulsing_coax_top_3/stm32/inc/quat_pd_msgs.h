// Quat PD Message Definitions

#ifndef QUAT_PD_MSGS_H
#define QUAT_PD_MSGS_H

#ifndef TYPE_QUAT_PD_VAL
  #error "Define unique TYPE_QUAT_PD_VAL for kTypeQuatPD before inclusion."
#endif

#ifndef TYPE_QUAT_PD_PARAM_VAL
  #error "Define unique TYPE_QUAT_PD_PARAM_VAL for kTypeQuatPDParam before inclusion."
#endif


////////////////////////////////////////////////////////////////////////////////
// Type
const uint8_t kTypeQuatPD = TYPE_QUAT_PD_VAL;
// Subtypes
const uint8_t kSubtypeF0Yaw                   =  0;
const uint8_t kSubtypeF1Yaw                   =  1;
const uint8_t kSubtypeF2Yaw                   =  2;
const uint8_t kSubtypePYaw                    =  3;
const uint8_t kSubtypeDYaw                    =  4;
const uint8_t kSubtypePRoll                   =  5;
const uint8_t kSubtypeDRoll                   =  6;
const uint8_t kSubtypeWxyF0                   =  7;
const uint8_t kSubtypeWzF0                    =  8;
const uint8_t kSubtypeG                       =  9;

// type | subtype                    |       ---XXX---             |
// -----------------------------------------------------------------
// X    |  0 - F0Yaw                  | uint8 access | float        |
// X    |  1 - F1Yaw                  | uint8 access | float        |
// X    |  2 - F2Yaw                  | uint8 access | float        |
// X    |  3 - PYaw                   | uint8 access | float        |
// X    |  4 - DYaw                   | uint8 access | float        |
// X    |  5 - PRoll                  | uint8 access | float        |
// X    |  6 - DRoll                  | uint8 access | float        |
// X    |  7 - WxyF0                  | uint8 access | float        |
// X    |  8 - WzF0                   | uint8 access | float        |
// X    |  9 - g                      | uint8 access | float        |
// 'access' is 0-get, 1-set, 2-save, 3-reply

////////////////////////////////////////////////////////////////////////////////
// Type
const uint8_t kTypeQuatPDParam = TYPE_QUAT_PD_PARAM_VAL;
// Struct Definition
typedef struct __attribute__ ((__packed__)) {
  float f0_yaw;     // model feedforward polynomial (volts)
  float f1_yaw;     // model feedforward polynomial (volts/volt)
  float f2_yaw;     // model feedforward polynomial (volts/volts^2)
  float p_yaw;      // proportional gain  (volts/rad)
  float d_yaw;      // deriative gain     (volts/(rad/s))
  float p_roll;     // proportional gain  (volts/rad)
  float d_roll;     // derivative gain    (volts/(rad/s))
  float wxy_f0;
  float wz_f0;
  float g;
} MsgQuatPDParam;

#endif // QUAT_PD_MSGS_H