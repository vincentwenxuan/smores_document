#ifndef TYPES_H
#define TYPES_H

#define TYPE_OUTPUT_DATA 't'
struct OUTPUT_DATA
{
  uint32_t time;
  int16_t tx_quadID;
  int16_t accelX;
  int16_t accelY;
  int16_t accelZ;
  //int16_t gyroX;
  //int16_t gyroY;
  //int16_t gyroZ;
};

#define TYPE_TRPY_CMD 'p'
struct TRPY_CMD
{
  int16_t quad_id;
  int16_t thrust;
  int16_t roll;
  int16_t pitch;
  int16_t yaw;
  int16_t current_yaw;
  uint8_t enable_motors;
  uint8_t use_external_yaw;
  uint8_t recovery_mode;
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
