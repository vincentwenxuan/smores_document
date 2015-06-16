#ifndef PROJECT_MESSAGE_TYPES_H
#define PROJECT_MESSAGE_TYPES_H

#include "common_message_types.h"

#define TYPE_PULSING_COAX_CONTROLLER_QUAT_VAL   140
#define TYPE_PULSING_COAX_PARAM_VAL             141
#define TYPE_ROLL_REPORT_VAL                    142
#define TYPE_YAW_REPORT_VAL                     143
#define TYPE_CONTROL_CALC_VAL                   145
#include "pulsing_coax_controller_quat_msgs.h"

#define TYPE_QUAT_PD_VAL                        150
#define TYPE_QUAT_PD_PARAM_VAL                  151
#include "quat_pd_msgs.h"

#define TYPE_LOOP_MONITOR_VAL                   160
#define TYPE_LOOP_PROFILE_VAL                   161
#define TYPE_LOOP_FREQ_VAL                      162
#include "loop_monitor_msgs.h"

#define TYPE_UAV_REPORTER_VAL                   170
#define TYPE_UAV_REPORTER_PARAM_VAL             171
#define TYPE_UAV_REPORTER_REQUEST_VAL           172
#define TYPE_UAV_REPORTER_POWER_VAL             173
#define TYPE_UAV_REPORTER_ESTIMATOR_VAL         174
#define TYPE_CONTROL_EST_VAL                    175
#define TYPE_EFFICIENCY_VAL                     176
#include "uav_reporter_msgs.h"

#define TYPE_COAX_OPEN_CONTROLLER_VAL           180
#define TYPE_COAX_OPEN_CONTROLLER_PARAM_VAL     181
#include "coax_open_controller_msgs.h"

#define TYPE_COAX_OPEN_ATTITUDE_CONTROLLER_VAL           190
#define TYPE_COAX_OPEN_ATTITUDE_CONTROLLER_PARAM_VAL     191
#define TYPE_COAX_OPEN_ATTITUDE_CONTROLLER_DEBUG_VAL     192
#include "coax_open_attitude_controller_msgs.h"

#define TYPE_MOTOR_HAL_VAL           200
#define TYPE_MOTOR_HAL_PARAM_VAL     201
#define TYPE_MOTOR_HAL_REPORT_VAL    202
#include "motor_hal_msgs.h"

const uint8_t kTypeKill                       = 128;

const uint8_t kTypeQuatPilot                  = 199;
typedef struct __attribute__ ((__packed__)) {
  float qw;
  float qx;
  float qy;
  float qz;
  float thrust;     // volts
  float obs_yaw;    // rad
} MsgQuatPilot;

const uint8_t kTypeQuatFullObsPilot           = 198;
typedef struct __attribute__ ((__packed__)) {
  float thrust;   // volts
  float qw;       // quat
  float qx;       // quat
  float qy;       // quat
  float qz;       // quat
  float wx;       // rad/s
  float wy;       // rad/s
  float wz;       // rad/s
  float obs_qw;   // quat
  float obs_qx;   // quat
  float obs_qy;   // quat
  float obs_qz;   // quat
} MsgQuatFullObsPilot;

const uint8_t kTypeOpenAttitudePilot          = 131;
typedef struct __attribute__ ((__packed__)) {
  float thrust;     // volts
  float amplitude;  // volts
  float phase;      // radians, response direction, 0 radians = x+ = roll right
  float yaw;        // radians, desired yaw
  float obs_yaw;    // radians, observed yaw
} MsgOpenAttitudePilot;

const uint8_t kTypeOpenPilot                  = 132;
typedef struct __attribute__ ((__packed__)) {
  float thrust;     // volts
  float amplitude;  // volts
  float phase;      // radians, response direction, 0 radians = x+ = roll right
  float yaw_delta;  // volts
} MsgOpenPilot;

#endif // PROJECT_MESSAGE_TYPES_H