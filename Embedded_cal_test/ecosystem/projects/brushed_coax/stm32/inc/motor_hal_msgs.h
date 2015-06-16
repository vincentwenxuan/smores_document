#ifndef MOTOR_HAL_MSGS_H
#define MOTOR_HAL_MSGS_H

#ifndef TYPE_COAX_OPEN_CONTROLLER_VAL
  #error "Define unique TYPE_MOTOR_HAL_VAL for kTypeMotorHal before inclusion."
#endif

#ifndef TYPE_MOTOR_HAL_PARAM_VAL
  #error "Define unique TYPE_COAX_OPEN_CONTROLLER_PARAM_VAL for kTypeCoaxOpenControllerParam before inclusion."
#endif

#ifndef TYPE_MOTOR_HAL_REPORT_VAL
  #error "Define unique TYPE_MOTOR_HAL_REPORT_VAL for kTypeMotorHalReport before inclusion."
#endif


////////////////////////////////////////////////////////////////////////////////
// Type
const uint8_t kTypeMotorHal = TYPE_MOTOR_HAL_VAL;
// Subtypes
const uint8_t kSubtypeMotorHalTopMechZero           =  0;
const uint8_t kSubtypeMotorHalCalibrateTopMechZero  =  1;

// type | subtype                    |       ---XXX---             |
// -----------------------------------------------------------------
// X    |  0 - mech_zero             | uint8 access | float        |
// X    |  0 - calibrate_mech_zero   | uint8 set    | float        |
// 'access' is 0-get, 1-set, 2-save, 3-reply

////////////////////////////////////////////////////////////////////////////////
// Type
const uint8_t kTypeMotorHalParam = TYPE_MOTOR_HAL_PARAM_VAL;
// Struct Definition
typedef struct __attribute__ ((__packed__)) {
  float top_mech_zero;     // model feedforward polynomial (volts)
} MsgMotorHalParam;

////////////////////////////////////////////////////////////////////////////////
// Type
const uint8_t kTypeMotorHalReport = TYPE_MOTOR_HAL_REPORT_VAL;
// Struct Definition
typedef struct __attribute__ ((__packed__)) {
  float t;
  float top_mean;
  float bottom_mean;
  float u_amp;
  float u_phase;
  float top_speed;
  float bottom_speed;
} MsgMotorHalReport;
// 28 + 5 = 33 bytes

#endif // MOTOR_HAL_MSGS_H