/*
 * Quadrotor.h
 *
 *  Copyright (c) 2014 Kumar Robotics. All rights reserved. Proprietary License.
 *
 *	Created on: 2014-07-01
 *      Author: gareth
 */

#ifndef QUADROTOR_H_
#define QUADROTOR_H_

#include "mBus.h"
#include "usb_interface.h"
#include "zigbee_interface.h"

#include "MPU6050.h"
#include "CommonCodes.h"
#include "AttitudeESKF.hpp"

class Quadrotor {
public:
    
  /**
   *
   */
  Quadrotor(uint8_t ID, MPU6050 * mpu, UsbInterface * usb, ZigbeeInterface * zb);
  
  /**
   *
   */
  virtual ~Quadrotor();
  
  /**
   *  @brief Initialize the quadrotor object.
   *  @note Call this before run().
   */
  void init();
  
  /**
   *  @brief Run the main loop.
   *  @note This function blocks indefinitely.
   */
  void run();
  
private:
  
  //  anonymous enumeration of system state flags
  enum {
    Boot        = 0,
    CommsInit   = (1 << 0),
    MpuReady    = (1 << 1),
    MpuInit     = (1 << 2),
    PwmInit     = (1 << 3),
  };
  
  uint8_t quadID_;
  
  uint32_t state_;
  uint32_t bootTime_;
  uint32_t loopTime_;
  uint32_t loopDiff_;
  uint32_t commTime_;
  uint32_t ledTime_;
  
  //  hardware drivers
  MPU6050 * mpu_;
  UsbInterface * usb_;
  ZigbeeInterface * zb_;
  
  //  sensor readings
  Eigen::Matrix<float,3,1> accel_;
  Eigen::Matrix<float,3,1> gyro_;
  Eigen::Matrix<float,3,3> accelCov_;
  Eigen::Matrix<float,3,3> gyroCov_;
  
  //  state estimator
  kr::AttitudeESKF eskf_;
  
  //  control state (to be refactored...)
  struct {
    float Kp, Kd;
    float KpYaw, KdYaw;
    float KdTumbleYaw;
    
    float desThrust;
    float desRoll;
    float desPitch;
    float desYaw;
    
    float roll;
    float pitch;
    float yaw;
    
    float extYaw;
    
    float wx;
    float wy;
    float wz;
    
    float deltaRoll;
    float deltaPitch;
    float deltaYaw;
  } control_;
  
  uint16_t pwms_[4];
  bool motorsEnabled_;
  bool useExtYaw_;
  bool recoveryMode_;
  bool recovered_;
  uint32_t lastPacket_;
  
  //  system error status
  struct {
    Error::Code code;
    int32_t subcode;
  } errno_;
  
  /*
   *  Private functions
   */
  
  void setErrno(Error::Code code, int32_t subcode);
  
  void clearErrno();
  
  bool initIMU();
  
  //  transmit/receive data
  void processComms();
  
  //  read from sensors
  bool processSensors();
  
  //  perform state estimation
  void processState();
  
  //  perform control
  void processControl();
  
  //  update LEDs
  void updateLEDs();
};

#endif // QUADROTOR_H_
