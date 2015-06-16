// Pulsing Coax Controller Euler Message Definitions

#ifndef PULSING_COAX_CONTROLLER_QUAT_MSGS_H
#define PULSING_COAX_CONTROLLER_QUAT_MSGS_H

#ifndef TYPE_PULSING_COAX_CONTROLLER_QUAT_VAL
  #error "Define unique TYPE_PULSING_COAX_CONTROLLER_QUAT_VAL for kTypePulsingCoaxControllerQuat before inclusion."
#endif

#ifndef TYPE_PULSING_COAX_PARAM_VAL
  #error "Define unique TYPE_PULSING_COAX_PARAM_VAL for kTypePulsingCoaxParam before inclusion."
#endif

#ifndef TYPE_ROLL_REPORT_VAL
  #error "Define unique TYPE_ROLL_REPORT_VAL for kTypeRollReport before inclusion."
#endif

#ifndef TYPE_YAW_REPORT_VAL
  #error "Define unique TYPE_ROLL_REPORT_VAL for kTypeRollReport before inclusion."
#endif

#ifndef TYPE_CONTROL_CALC_VAL
  #error "Define unique TYPE_CONTROL_CALC_VAL for kTypeControlCalc before inclusion."
#endif


////////////////////////////////////////////////////////////////////////////////
// Type
const uint8_t kTypePulsingCoaxControllerQuat = TYPE_PULSING_COAX_CONTROLLER_QUAT_VAL;
// Subtypes
const uint8_t kSubtypeAdvanceAngle      = 0;
const uint8_t kSubtypePulseThresh       = 2;
const uint8_t kSubtypePulseMin          = 3;
const uint8_t kSubtypePulseSat          = 4;
const uint8_t kSubtypeYawSat            = 5;
const uint8_t kSubtypeThrustSat         = 6;
const uint8_t kSubtypeEnableBottom      = 7;

// type | subtype                             |           ---XXX---           |
// ----------------------------------------------------------------------------
// X    | 0 - AdvanceAngle            | access  (uint8) |     (float) |
// X    | 2 - PulseThresh             | access  (uint8) |     (float) |
// X    | 3 - PulseMin                | access  (uint8) |     (float) |
// X    | 4 - PulseSat                | access  (uint8) |     (float) |
// X    | 5 - YawSat                  | access  (uint8) |     (float) |
// X    | 6 - ThrustSat               | access  (uint8) |     (float) |
// X    | 7 - EnableBottom            | access  (uint8) | 0/1 (uint8) |
// access is 0-get, 1-set, 2-save, 3-reply


////////////////////////////////////////////////////////////////////////////////
// Type
const uint8_t kTypePulsingCoaxParam = TYPE_PULSING_COAX_PARAM_VAL;
// Struct Definition
typedef struct __attribute__ ((__packed__)) {
  float advance_angle;
  float pulse_thresh;
  float pulse_min;
  float pulse_sat;
  float yaw_sat;
  float thrust_sat;
  uint8_t enable_bottom;
} MsgPulsingCoaxParam;


////////////////////////////////////////////////////////////////////////////////
// Type
const uint8_t kTypeRollReport = TYPE_ROLL_REPORT_VAL;
// Struct Definition
typedef struct __attribute__ ((__packed__)) {
  float t;
  float wx_meas;
  float wx_est;
  float wx_filt;
  float ex_est;
  float ex_des;
  float ux_p;
  float ux_d;
  float top_mean;
  float bottom_mean;
  float u_amp;
  float u_phase;
} MsgRollReport;
// 48 + 5 = 53 bytes


////////////////////////////////////////////////////////////////////////////////
// Type
const uint8_t kTypeControlCalc = TYPE_CONTROL_CALC_VAL;
// Struct Definition
typedef struct __attribute__ ((__packed__)) {
  float t;
  float ux_p;
  float uy_p;
  float uz_p;
  float ux_d;
  float uy_d;
  float uz_d;
  float uz_f;
  float ux_g;
  float uy_g;
  float thrust_des;
  float u_amp;
  float u_phase;
} MsgControlCalc;
// 56 + 5 = 61 bytes


////////////////////////////////////////////////////////////////////////////////
// Type
const uint8_t kTypeYawReport = TYPE_YAW_REPORT_VAL;
// Struct Definition
typedef struct __attribute__ ((__packed__)) {
  float t;
  float wz_meas;
  float wz_est;
  float wz_filt;
  float ez_est;
  float ez_des;
  float uz_p;
  float uz_d;
  float uz_f;
  float uz;
  float top_mean;
  float bottom_mean;
} MsgYawReport;
// 44 + 5 = 49 bytes


#endif // PULSING_COAX_CONTROLLER_QUAT_MSGS_H