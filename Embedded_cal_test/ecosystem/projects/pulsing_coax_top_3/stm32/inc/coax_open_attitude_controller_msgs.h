#ifndef COAX_OPEN_ATTITUDE_CONTROLLER_MSGS_H
#define COAX_OPEN_ATTITUDE_CONTROLLER_MSGS_H

#ifndef TYPE_COAX_OPEN_ATTITUDE_CONTROLLER_VAL
  #error "Define unique TYPE_COAX_OPEN_ATTITUDE_CONTROLLER_VAL for kTypeCoaxOpenAttitudeController before inclusion."
#endif

#ifndef TYPE_COAX_OPEN_ATTITUDE_CONTROLLER_PARAM_VAL
  #error "Define unique TYPE_COAX_OPEN_ATTITUDE_CONTROLLER_PARAM_VAL for kTypeCoaxOpenAttitudeControllerParam before inclusion."
#endif

#ifndef TYPE_COAX_OPEN_ATTITUDE_CONTROLLER_DEBUG_VAL
  #error "Define unique TYPE_COAX_OPEN_ATTITUDE_CONTROLLER_DEBUG_VAL for kTypeCoaxOpenAttitudeControllerDebug before inclusion."
#endif


////////////////////////////////////////////////////////////////////////////////
// Type
const uint8_t kTypeCoaxOpenAttitudeController = TYPE_COAX_OPEN_ATTITUDE_CONTROLLER_VAL;
// Subtypes
const uint8_t kSubtypeCoaxOpenAttitudeControllerF0Yaw         =  0;
const uint8_t kSubtypeCoaxOpenAttitudeControllerF1Yaw         =  1;
const uint8_t kSubtypeCoaxOpenAttitudeControllerF2Yaw         =  2;
const uint8_t kSubtypeCoaxOpenAttitudeControllerPYaw          =  3;
const uint8_t kSubtypeCoaxOpenAttitudeControllerDYaw          =  4;
const uint8_t kSubtypeCoaxOpenAttitudeControllerWzF0          =  5;
const uint8_t kSubtypeCoaxOpenAttitudeControllerPulseSat      =  6;
const uint8_t kSubtypeCoaxOpenAttitudeControllerYawSat        =  7;
const uint8_t kSubtypeCoaxOpenAttitudeControllerThrustSat     =  8;
const uint8_t kSubtypeCoaxOpenAttitudeControllerAdvanceAngle  =  9;

// type | subtype                    |       ---XXX---             |
// -----------------------------------------------------------------
// X    |  0 - F0Yaw                  | uint8 access | float        |
// X    |  1 - F1Yaw                  | uint8 access | float        |
// X    |  2 - F2Yaw                  | uint8 access | float        |
// X    |  3 - PYaw                   | uint8 access | float        |
// X    |  4 - DYaw                   | uint8 access | float        |
// X    |  5 - WzF0                   | uint8 access | float        |
// X    |  6 - PulseSat               | uint8 access | float        |
// X    |  7 - YawSat                 | uint8 access | float        |
// X    |  8 - ThrustSat              | uint8 access | float        |
// X    |  9 - AdvanceAngle           | uint8 access | float        |
// 'access' is 0-get, 1-set, 2-save, 3-reply

////////////////////////////////////////////////////////////////////////////////
// Type
const uint8_t kTypeCoaxOpenAttitudeControllerParam = TYPE_COAX_OPEN_ATTITUDE_CONTROLLER_PARAM_VAL;
// Struct Definition
typedef struct __attribute__ ((__packed__)) {
  float f0_yaw;     // model feedforward polynomial (volts)
  float f1_yaw;     // model feedforward polynomial (volts/volt)
  float f2_yaw;     // model feedforward polynomial (volts/volts^2)
  float p_yaw;      // proportional gain  (volts/rad)
  float d_yaw;      // deriative gain     (volts/(rad/s))
  float wz_f0;
  float pulse_sat;
  float yaw_sat;
  float thrust_sat;
  float advance_angle;
} MsgCoaxOpenAttitudeControllerParam;

////////////////////////////////////////////////////////////////////////////////
// Type
const uint8_t kTypeCoaxOpenAttitudeControllerDebug = TYPE_COAX_OPEN_ATTITUDE_CONTROLLER_DEBUG_VAL;
// Struct Definition
typedef struct __attribute__ ((__packed__)) {
  float t;
  float uz_p;
  float uz_d;
  float uz_f;
  float pilot_thrust;
  float pilot_pulse_phase;
  float top_mean;
  float bottom_mean;
  float top_pulse_amp;
  float top_pulse_phase;
} MsgCoaxOpenAttitudeControllerDebug;
// 56 + 5 = 61 bytes

#endif // COAX_OPEN_ATTITUDE_CONTROLLER_MSGS_H