// Pulsing Controller Message Definitions
#include "common_message_types.h"
#include <stdint.h>

#ifndef BRUSHLESS_CONTROLLER_MSGS_H
#define BRUSHLESS_CONTROLLER_MSGS_H

// This file must be included from common_message_types.h after appropriate 
// #defines are issued.  This file should not be included from ANY file other 
// than common_message_types.h.

#ifndef TYPE_BRUSHLESS_CONTROLLER_VAL
  #error "Define unique TYPE_BRUSHLESS_CONTROLLER_VAL for kTypeBrushlessController before inclusion."
#endif

#ifndef TYPE_BRUSHLESS_CONTROLLER_STATE_VAL
  #error "Define unique TYPE_BRUSHLESS_CONTROLLER_STATE_VAL for kTypeBrushlessControllerState before inclusion."
#endif

////////////////////////////////////////////////////////////////////////////////
// Type
const uint8_t kTypeBrushlessController = TYPE_BRUSHLESS_CONTROLLER_VAL;
// Subtypes
const uint8_t kSubPwmCmd              = 0;
const uint8_t kSubVoltageCmd          = 1;
const uint8_t kSubCurrentCmd          = 2;
const uint8_t kSubTorqueCmd           = 3;
const uint8_t kSubSpeedCmd            = 4;
const uint8_t kSubSpeedPosCmd         = 5;
const uint8_t kSubPosCmd              = 6;
const uint8_t kSubElecAngleCmd        = 8;
const uint8_t kSubCalibrateCmd        = 9;
const uint8_t kSubWriteToFlashCmd     = 10;
const uint8_t kSubCalibrationAngleMsg = 11;
const uint8_t kSubLeadPerVelocityMsg  = 12;
const uint8_t kSubMotorDataMsg        = 13;
const uint8_t kSubCurrentCtrlMsg      = 15;
const uint8_t kSubSpeedCtrlMsg        = 16;
const uint8_t kSubPosCtrlMsg          = 17;
const uint8_t kSubPermuteMsg          = 18;
const uint8_t kSubStateMsg            = 19;
const uint8_t kSubSpeedKpMsg          = 20;
const uint8_t kSubSpeedKiMsg          = 21;
const uint8_t kSubSpeedKdMsg          = 22;
const uint8_t kSubCommutationHzMsg    = 23;
const uint8_t kSubCmdVoltsMsg         = 24;
const uint8_t kSubSpeedFilterAlphaMsg = 25;

/// Brushless controller message
/// Sends commands to the controller or gets/sets values
/// The associated struct changes based on the message sent
// Struct types (data is float unless noted otherwise):

// X |  0 | PWM_CMD             (-1 to 1)
// X |  1 | Voltage_CMD         (-Supply Voltage to Supply Voltage), requires frequent set_voltage_supply calls
// X |  2 | Current_CMD         (-Amps to Amps) | regen (0 or 1 uint8_t)
// X |  3 | Torque_CMD          (-Nm to Nm), requires motor settings already set
// X |  4 | Speed_CMD           (-rad/s to rad/s)
// X |  5 | Speed_Pos_CMD       (-rad/s to rad/s) | Angle_CMD (0 to radians), angles overwrite, not sum
// X |  6 | Pos_CMD             (0 to 2*PI radians)
// X |  8 | ElecAngle_CMD       (-PI to PI radians)
// X |  9 | Calibration_PWM_CMD (0 to 1) | Calibration_Time_ms_CMD (0 to ms) (uint32_t)
// X |  X | Variable_To_Write_CMD (0 to 254 individual variables, 255 for all, N/A for CMDs uint8_t)
// X | 11 | access (uint8) | Calibration_Angle (-2*pi to 2*pi rad)    TODO: SRSLY?
// X | 12 | access (uint8) | Lead_Per_Velocity (0 to rad/(rad/s))
// X | 13 | access (uint8) | Motor_Pole_Pairs (0 to Pole Pairs (uint16_t)) | Motor_Data_Kv (0 to RPM/V) | Motor_Data_R (0 to ohms) | Motor_Data_I0 (0 to No Load Amps) | Motor_Data_V0 (0 to No Load Volts) | Motor_Bemf_Shape (0,1,2 = trapezoidal, sinusoidal, quadrature as defined in vector_control)
// X | 15 | access (uint8) | current_control_pid_Kp | current_control_pid_Ki | current_control_pid_Kd
// X | 16 | access (uint8) | speed_control_pid_Kp | speed_control_pid_Ki | speed_control_pid_Kd
// X | 17 | access (uint8) | pos_control_pid_Kp | pos_control_pid_Ki | pos_control_pid_Kd
// X | 18 | access (uint8) | permute_wires (0 for no, 1 for yes)
// X | 19 | access (uint8) | time | cmd_pwm | pwm | speed | position
// X | 20 | access (uint8) | speed_kp (float)
// X | 21 | access (uint8) | speed_ki (float)
// X | 22 | access (uint8) | speed_kd (float)
// X | 23 | access (uint8) | commutation_hz (float)
// X | 24 | get/set (uint8)| cmd_volts (float)            // command volts, the same parameter as Sub=1
// X | 25 | access (uint8) | speed_filter_alpha (float)   // speed filter coefficient in interval [0,1]
// 'access' is 0-get, 1-set, 2-save, 3-reply, 4-stream

typedef struct __attribute__ ((__packed__)) {
  uint8_t sub_command; 
  uint8_t data;
} MsgBrushlessController;
typedef struct __attribute__ ((__packed__)) {
  float pwm_cmd; 
} MsgBrushlessControllerPwmCmd;
typedef struct __attribute__ ((__packed__)) {
  float voltage_cmd; 
} MsgBrushlessControllerVoltageCmd;
typedef struct __attribute__ ((__packed__)) {
  float current_cmd; 
  uint8_t regen;
} MsgBrushlessControllerCurrentCmd;
typedef struct __attribute__ ((__packed__)) {
  float torque_cmd; 
} MsgBrushlessControllerTorqueCmd;
typedef struct __attribute__ ((__packed__)) {
  float speed_cmd; 
} MsgBrushlessControllerSpeedCmd;
typedef struct __attribute__ ((__packed__)) {
  float speed_pos_cmd; 
} MsgBrushlessControllerSpeedPosCmd;
typedef struct __attribute__ ((__packed__)) {
  float pos_cmd; 
} MsgBrushlessControllerPosCmd;
typedef struct __attribute__ ((__packed__)) {
  float pwm_cmd; 
  uint32_t time_cmd;
} MsgBrushlessControllerCalibrationCmd;
typedef struct __attribute__ ((__packed__)) {
  uint8_t variable_cmd; 
} MsgBrushlessControllerWriteCmd;
typedef struct __attribute__ ((__packed__)) {
  uint8_t cmd;
  float calibration_angle;
} MsgBrushlessControllerCalibration;
typedef struct __attribute__ ((__packed__)) {
  uint8_t cmd;
  float lead_rate;
} MsgBrushlessControllerLead;
typedef struct __attribute__ ((__packed__)) {
  uint8_t cmd;
  uint16_t pole_pairs;
  float Kv;
  float R;
  float I0;
  float V0;
  float I_max;
  uint8_t bemf_shape;
} MsgBrushlessControllerMotor;
typedef struct __attribute__ ((__packed__)) {
  uint8_t cmd;
  float Kp;
  float Ki;
  float Kd;
} MsgBrushlessControllerCurrentControl;
typedef struct __attribute__ ((__packed__)) {
  uint8_t cmd;
  float Kp;
  float Ki;
  float Kd;
} MsgBrushlessControllerSpeedControl;
typedef struct __attribute__ ((__packed__)) {
  uint8_t cmd;
  float Kp;
  float Ki;
  float Kd;
} MsgBrushlessControllerPositionControl;
typedef struct __attribute__ ((__packed__)) {
  uint8_t cmd;
  uint8_t permute;
} MsgBrushlessControllerPermute;
typedef struct __attribute__ ((__packed__)) {
  uint8_t cmd;
  uint32_t commutation_hz;
} MsgBrushlessControllerCommutationHz;

////////////////////////////////////////////////////////////////////////////////
// Type
const uint8_t kTypeBrushlessControllerState = TYPE_BRUSHLESS_CONTROLLER_STATE_VAL;
// Struct Definition
typedef struct __attribute__ ((__packed__)) {
  uint8_t cmd;                // access (get, reply, stream)
  float t;                    // system clock time
  float cmd_pwm;              // commanded baseline pwm
  float pwm;                  // instantaneous output pwm
  float speed;                // filtered measured speed
  float position;             // instantaneous position
} MsgBrushlessControllerState;

#endif // PULSING_CONTROLLER_MSGS_H