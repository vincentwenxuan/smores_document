#ifndef TYPES_H
#define TYPES_H

#define TYPE_OUTPUT_DATA 't'
struct OUTPUT_DATA
{
  uint16_t time;
  int16_t des_thrust;
  int16_t des_roll;
  int16_t des_pitch;
  int16_t des_yaw;
  int16_t est_roll;
  int16_t est_pitch;
  int16_t est_yaw;
  int16_t est_angvel_x;
  int16_t est_angvel_y;
  int16_t est_angvel_z;
  int16_t est_acc_x;
  int16_t est_acc_y;
  int16_t est_acc_z;
  uint16_t pwm1;
  uint16_t pwm2;
  uint16_t pwm3;
  uint16_t pwm4;
};

#define TYPE_TRPY_CMD 'p'
struct TRPY_CMD
{
  int16_t thrust;
  int16_t roll;
  int16_t pitch;
  int16_t yaw;
  int16_t current_yaw;
  uint8_t enable_motors:1;
  uint8_t use_external_yaw:1;
};

#define TYPE_GAINS_CMD 'g'
struct GAINS_CMD
{
  int16_t Kp;
  int16_t Kd;
  int16_t Kp_yaw;
  int16_t Kd_yaw;
};

struct IMU_DATA
{
  float roll;
  float pitch;
  float yaw;
  float angvel_x;
  float angvel_y;
  float angvel_z;
  float acc_x;
  float acc_y;
  float acc_z;
};

#endif
