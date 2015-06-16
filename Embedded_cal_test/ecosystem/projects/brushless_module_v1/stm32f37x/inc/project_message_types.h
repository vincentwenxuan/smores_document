#ifndef PROJECT_MESSAGE_TYPES_H
#define PROJECT_MESSAGE_TYPES_H

#include "common_message_types.h"
#define PWM_CMD 0x50 //'P'
typedef struct __attribute__ ((__packed__)) {
  float pwm_duty;
} MsgPwm;

#define CAL_CMD 0x43 //'C'
typedef struct __attribute__ ((__packed__)) {
  float cal_duty;
  uint16_t cal_time_ms;
} MsgCal;

#define CAL_ANG 0x63 //'c'
typedef struct __attribute__ ((__packed__)) {
  float cal_ang;
} MsgCalAng;

#define LEAD_FACT 0x6c //'l'
typedef struct __attribute__ ((__packed__)) {
  float lead_fact;
} MsgLead;

#define ROT_FBK 0x52 //'R'
typedef struct __attribute__ ((__packed__)) {
  float motor_ang_vel;
  float body_ang_vel;
} MsgRotFbk;

#define TAPY_CMD 0x54 //'T'
typedef struct __attribute__ ((__packed__)) {
  uint8_t id;
  float thrust;
  float amplitude;
  float phase;
  float yaw;
} MsgTAPY;

#endif // PROJECT_MESSAGE_TYPES_H
