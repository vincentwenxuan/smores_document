// UAV Reporter Message Definitions

#ifndef UAV_REPORTER_MSGS_H
#define UAV_REPORTER_MSGS_H

#ifndef TYPE_UAV_REPORTER_VAL
  #error "Define unique TYPE_UAV_REPORTER_VAL for kTypeUavReporter before inclusion."
#endif

#ifndef TYPE_UAV_REPORTER_PARAM_VAL
  #error "Define unique TYPE_UAV_REPORTER_PARAM_VAL for kTypeUavReporterParam before inclusion."
#endif

#ifndef TYPE_UAV_REPORTER_REQUEST_VAL
  #error "Define unique TYPE_UAV_REPORTER_REQUEST_VAL for kTypeUavReporterRequest before inclusion."
#endif

#ifndef TYPE_UAV_REPORTER_POWER_VAL
  #error "Define unique TYPE_UAV_REPORTER_POWER_VAL for kTypeUavReporterPower before inclusion."
#endif

#ifndef TYPE_UAV_REPORTER_ESTIMATOR_VAL
  #error "Define unique TYPE_UAV_REPORTER_ESTIMATOR_VAL for kTypeUavReporterEstimator before inclusion."
#endif

#ifndef TYPE_CONTROL_EST_VAL
  #error "Define unique TYPE_CONTROL_EST_VAL for kTypeControlEst before inclusion."
#endif

#ifndef TYPE_EFFICIENCY_VAL
  #error "Define unique TYPE_EFFICIENCY_VAL for kTypeEfficiency before inclusion."
#endif


////////////////////////////////////////////////////////////////////////////////
// Type
const uint8_t kTypeUavReporter = TYPE_UAV_REPORTER_VAL;
// Subtypes
const uint8_t kSubtypeSendRollReport            =  0; // 48 byte payload
const uint8_t kSubtypeSendYawReport             =  1;
const uint8_t kSubtypeSendControlCalc           =  2;
const uint8_t kSubtypeSendControlEst            =  3; // 56 byte payload
const uint8_t kSubtypeSendImuRawReport          =  4;
const uint8_t kSubtypeSendImuMetricReport       =  5;
const uint8_t kSubtypeSendEstQuatReport         =  6;
const uint8_t kSubtypeSendMotorReport           =  7; // 28 byte payload
const uint8_t kSubtypeSendBatteryReport         =  8; // 28 byte payload
const uint8_t kSubtypeSendLoopProfile           =  9;
const uint8_t kSubtypeSendLoopFreq              = 10; //  4 byte payload
const uint8_t kSubtypeSendEstimator             = 11; // 56 byte payload
const uint8_t kSubtypeSendEfficiency            = 12; // 56 byte payload
const uint8_t kSubtypeSendOpenAttitudeDebug     = 13; // 56 byte payload


// type | subtype                 |          ---XXX---         |
// -------------------------------------------------------------
// X  |  0 - RollReport           | uint8 access | uint8 {0,1} |
// X  |  1 - YawReport            | uint8 access | uint8 {0,1} |
// X  |  2 - ControlCalc          | uint8 access | uint8 {0,1} |
// X  |  3 - ControlEst           | uint8 access | uint8 {0,1} |
// X  |  4 - ImuRawReport         | uint8 access | uint8 {0,1} |
// X  |  5 - ImuMetricReport      | uint8 access | uint8 {0,1} |
// X  |  6 - EstQuatReport        | uint8 access | uint8 {0,1} |
// X  |  7 - MotorReport          | uint8 access | uint8 {0,1} |
// X  |  8 - BatteryReport        | uint8 access | uint8 {0,1} |
// X  |  9 - LoopProfile          | uint8 access | uint8 {0,1} |
// X  | 10 - LoopFreq             | uint8 access | uint8 {0,1} |
// X  | 11 - Estimator            | uint8 access | uint8 {0,1} |
// X  | 12 - Efficiency           | uint8 access | uint8 {0,1} |
// X  | 13 - OpenAttitudeDebug    | uint8 access | uint8 {0,1} |
// 'access' is 0-get, 1-set, 2-save, 3-reply


////////////////////////////////////////////////////////////////////////////////
// Type
const uint8_t kTypeUavReporterParam = TYPE_UAV_REPORTER_PARAM_VAL;
// Struct Definition
typedef struct __attribute__ ((__packed__)) {
  uint8_t roll;
  uint8_t yaw;
  uint8_t control_calc;
  uint8_t control_est;
  uint8_t imu_raw;
  uint8_t imu_metric;
  uint8_t est_quat;
  uint8_t motor;
  uint8_t battery;
  uint8_t loop_profile;
  uint8_t loop_freq;
  uint8_t estimator;
  uint8_t efficiency;
  uint8_t open_attitude_debug;
} MsgUavReporterParam;


////////////////////////////////////////////////////////////////////////////////
// Type
const uint8_t kTypeUavReporterRequest = TYPE_UAV_REPORTER_REQUEST_VAL;
// Subtypes: None

////////////////////////////////////////////////////////////////////////////////
// Type
const uint8_t kTypeUavReporterPower = TYPE_UAV_REPORTER_POWER_VAL;
// Struct Definition
typedef struct __attribute__ ((__packed__)) {
  float t;
  float top_volts;
  float top_amps;
  float top_joules;
  float bottom_volts;
  float bottom_amps;
  float bottom_joules;
} MsgUavReporterPower;
// 28 + 5 = 33 bytes


////////////////////////////////////////////////////////////////////////////////
// Type
const uint8_t kTypeUavReporterEstimator = TYPE_UAV_REPORTER_ESTIMATOR_VAL;
// Struct Definition
typedef struct __attribute__ ((__packed__)) {
  float t;
  float wx_meas;
  float wy_meas;
  float wz_meas;
  float ax_meas;
  float ay_meas;
  float az_meas;
  float wx_est;
  float wy_est;
  float wz_est;
  float qw_est;
  float qx_est;
  float qy_est;
  float qz_est;
} MsgUavReporterEstimator;
// 56 + 5 = 61 bytes


////////////////////////////////////////////////////////////////////////////////
// Type
const uint8_t kTypeControlEst = TYPE_CONTROL_EST_VAL;
// Struct Definition
typedef struct __attribute__ ((__packed__)) {
  float t;
  float wx_est;
  float wy_est;
  float wz_est;
  float qw_est;
  float qx_est;
  float qy_est;
  float qz_est;
  float wx_filt;
  float wy_filt;
  float wz_filt;
  float top_mean;
  float bottom_mean;
  float top_volts;
} MsgControlEst;
// 56 + 5 = 61 bytes


////////////////////////////////////////////////////////////////////////////////
// Type
const uint8_t kTypeEfficiency = TYPE_EFFICIENCY_VAL;
// Struct Definition
typedef struct __attribute__ ((__packed__)) {
  float t;
  float top_volts;
  float top_amps;
  float top_joules;
  float top_mean;
  float top_speed;
  float bottom_volts;
  float bottom_amps;
  float bottom_joules;
  float bottom_mean;
  float bottom_speed;
  float qx_est;   
  float qy_est;
  float qz_est;
} MsgEfficiency;
// 56 + 5 = 61 bytes

#endif // UAV_REPORTER_MSGS_H