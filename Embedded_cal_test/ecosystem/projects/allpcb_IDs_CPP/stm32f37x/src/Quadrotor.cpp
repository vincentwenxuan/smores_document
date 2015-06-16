/*
 * Quadrotor.cpp
 *
 *  Copyright (c) 2014 Kumar Robotics. All rights reserved. Proprietary License.
 *
 *	Created on: 2014-07-01
 *      Author: gareth
 */

#include "Quadrotor.h"
#include "project_message_types.h"
#include "pwm.h"
#include "battery_monitor_it.h"

static float normalize_angle(float angle)
{
  static const float pi = static_cast<float>(M_PI);
  
  while(angle <= -pi) {
    angle += 2*pi;
  }
  
  while(angle > pi) {
    angle -= 2*pi;
  }
  
  return angle;
}

Quadrotor::Quadrotor(uint8_t ID, MPU6050 * mpu, UsbInterface * usb, ZigbeeInterface * zb) :
  quadID_(ID), mpu_(mpu), usb_(usb), zb_(zb), eskf_()
{
  state_ = Boot;
  bootTime_ = 0;
  loopTime_ = 0;
  loopDiff_ = 0;
  commTime_ = 0;
  ledTime_ = 0;
  lastPacket_ = 0;
}

Quadrotor::~Quadrotor() {}

void Quadrotor::init()
{
  bootTime_ = GetMicroseconds();
  
  accel_.setZero();
  gyro_.setZero();
  
  eskf_.setUsesMagnetometer(false);
  eskf_.setGyroBiasThreshold(0.03f);
  eskf_.setEstimatesBias(true);
  eskf_.setIgnoresZUpdate(true);
  
  //  gyro covariance, units of (rad/s)^2
  gyroCov_ << 0.01f, 0, 0,
   0, 0.01f, 0,
   0, 0, 0.01f;
  
  //  accel covariance, units of (m/s^2)^2
  accelCov_ << 4.0f, 0, 0,
   0, 4.0f, 0,
   0, 0, 4.0f;
  
  //  controller values
  memset(pwms_, 0, sizeof(pwms_));
  motorsEnabled_ = false;
  useExtYaw_ = false;
  
  memset(&control_, 0, sizeof(control_));
  
  clearErrno();
}

void Quadrotor::run()
{
  while (true)
  {
    //  check if communications are initialized
    if (!(state_ & CommsInit)) {
      if (usb_) {
        usb_->Init();
      }
      DelayMilliseconds(5);
      
      if (zb_) {
        //  add this back when transitioning to new C++ zigbee
        //zb_->Init();
      }
      DelayMilliseconds(5);
      
      state_ |= CommsInit;
    }
    
    //  check if the IMU is prepared
    if (!(state_ & MpuReady)) {
      
      if (mpu_->isReady())
      {
        //  reset to clean state and wait 100ms for reboot
        mpu_->reset();
        DelayMicroseconds(1e5);
        state_ |= MpuReady;
      }
      //  TODO: handle possible failure here properly
    } else {
      //  MPU peripheral is ready, are settings ok?
      if (!(state_ & MpuInit))
      {
        if (initIMU()) {
          state_ |= MpuInit;
        }
      }
    }
    
    if ((state_ & MpuInit) && (state_ & CommsInit)) {
      //  while system is running, clear error every pass
      clearErrno();
    }
    
    //  query sensors
    if (state_ & MpuInit) {
      processSensors();
    }
    
    //  do state estimation
    processState();
    
    //  do safety check (TODO: move this elsewhere)
    if (fabsf(control_.roll) > 1.57f || fabsf(control_.pitch) > 1.57f) {
     // motorsEnabled_ = false;
    }
    
    //  PD attitude + motor update
    processControl();
    
    //  transmit state
    if (state_ & CommsInit) {
      processComms();
    }
    
    updateLEDs();
    
    //  regulate the runloop to 1000hz
    do {
      loopDiff_ = MicrosecondsElapsed(loopTime_);
    } while (loopDiff_ < 1000);
    loopTime_ = GetMicroseconds();
  }
}

void Quadrotor::setErrno(Error::Code code, int32_t subcode) {
  errno_.code = code;
  errno_.subcode = subcode;
}

void Quadrotor::clearErrno() {
  errno_.code = Error::None;
  errno_.subcode = 0;
}

bool Quadrotor::initIMU() {
  MPU6050::ErrorCode code;
  
  code = mpu_->testConnection();
  if (code) {
    setErrno(Error::IMUConnection, code);
    return false;
  }
  
  //  MPU6050 must be woken up before configuration
  code = mpu_->setSleep(false);
  if (code) {
    setErrno(Error::IMUState, code);
    return false;
  }
  
  //  hardcode these settings for the PPR pico for now
  code = mpu_->setSampleRate(1000);
  if (code) {
    setErrno(Error::IMUOption, code);
    return false;
  }
  
  code = mpu_->setFilterMode(MPU6050::LPF_A260_G256);
  if (code) {
    setErrno(Error::IMUOption, code);
    return false;
  }
  
  code = mpu_->setGyroRange(MPU6050::Gyro1000DPS);
  if (code) {
    setErrno(Error::IMUOption, code);
    return false;
  }
  
  code = mpu_->setAccelRange(MPU6050::Accel4G);
  if (code){
    setErrno(Error::IMUOption, code);
    return false;
  }
  
  //  start our first read
  mpu_->startRead();
  
  DelayMilliseconds(1);
  return true;
}

void Quadrotor::processComms() {
  if (usb_) {
    //  print debug...
  }
  
  //  TODO: This is copied from the original allPCB.
  //  Re-write it with newer Zigbee code later.
  if (zb_) {
    uint8_t tx_buf[150], rx_buf[150];
    uint8_t rx_length=0;
    
    uint8_t irq = ZigBee_ReadIrq();
    if ((irq & 0x08) == 0x08)
    {
      ZigBee_ReadFrame(rx_buf, &rx_length);
      if (rx_length > 2) {
        if (rx_buf[rx_length-1] & 0x80) {
          const uint8_t len = rx_length - 6;  //  6=frame overhead
          
          if (rx_buf[0]==TYPE_TRPY_CMD && len==15) {
            //  new thrust, roll, pitch, yaw
            TRPY_CMD * trpy = reinterpret_cast<TRPY_CMD*>(&rx_buf[1]);
            
            //  TODO: why is this scale factor 1e4??
            uint8_t quad_id = trpy->quad_id;
            if (quad_id == quadID_) {
              control_.desThrust = trpy->thrust / 10000.0f;
              control_.desRoll = trpy->roll / 10000.0f;
              control_.desPitch = trpy->pitch / 10000.0f;
              control_.desYaw = trpy->yaw / 10000.0f;
              control_.extYaw = trpy->current_yaw / 10000.0f;
              
              motorsEnabled_ = static_cast<bool>(trpy->enable_motors);
              useExtYaw_ = static_cast<bool>(trpy->use_external_yaw);
              
              if (!recoveryMode_ && trpy->recovery_mode) {
                recovered_ = false;
                recoveryMode_ = true;
              } else if (!trpy->recovery_mode) {
                recoveryMode_ = false;
              }
              //recoveryMode_ = static_cast<bool>(trpy->recovery_mode);
              
              //if (lastPacket_ != 0) {
                //usb_->printf("delta: %i\n", GetMicroseconds() - lastPacket_);
              //}
              //lastPacket_ = GetMicroseconds();
              
              //usb_->printf("Kp: %li, Kd: %li\n", (long int)control_.Kp, (long int)control_.Kd);
              //usb_->printf("Act roll: %f, Des roll: %f\n", control_.roll, control_.desRoll);
              //usb_->printf("Act pitch: %f, Des pitch: %f\n", control_.pitch, control_.desPitch);
              
              //usb_->printf("Received TRPY: %f, %f, %f\n", control_.desRoll, control_.desPitch, control_.desYaw);
              //usb_->printf("est: %f, ext: %f\n", control_.yaw, trpy->current_yaw / 10000.0f);
              
              //usb_->printf("rpy: %f, %f, %f\n", control_.roll, control_.pitch, control_.yaw);
              mBlueTOGGLE;
            }
          }
          else if (rx_buf[0]==TYPE_GAINS_CMD && len==sizeof(GAINS_CMD)) {
            //  new controller gains
            GAINS_CMD * gains = reinterpret_cast<GAINS_CMD*>(&rx_buf[1]);
            control_.Kp = gains->Kp;
            control_.Kd = gains->Kd;
            control_.KpYaw = gains->Kp_yaw;
            control_.KdYaw = gains->Kd_yaw;
            //control_.KdTumbleYaw = gains->Kd_tumble_yaw;
            
            //usb_->printf("Received GAINS\n");
            //mBlueTOGGLE;
          }
        }
      }
    }
    
    //  due to send a packet back to base station
    if (MicrosecondsElapsed(commTime_) > 10000) {
      ZigBee_SetModeTx();
      
      tx_buf[0] = TYPE_OUTPUT_DATA;
      
      OUTPUT_DATA * out = reinterpret_cast<OUTPUT_DATA*>(&tx_buf[1]);
      out->time = GetMicroseconds();
      out->tx_quadID = quadID_;
      out->accelX = gyro_[0] * 100;
      out->accelY = gyro_[1] * 100;
      out->accelZ = gyro_[2] * 100;
      //out->gyroX = gyro_[0] * 100;
      //out->gyroY = gyro_[1] * 100;
      //out->gyroZ = gyro_[2] * 100;
      
//      
//      out->time = GetMicroseconds() / 1000000;
//      out->des_thrust = control_.desThrust*10000;
//      out->des_roll = control_.desRoll*10000;
//      out->des_pitch = control_.desPitch*10000;
//      out->des_yaw = control_.desYaw*10000;
//      
//      out->est_roll = control_.roll*10000;
//      out->est_pitch = control_.pitch*10000;
//      out->est_yaw = control_.yaw*10000;
//      
//      out->est_angvel_x = control_.wx*1000;
//      out->est_angvel_y = control_.wy*1000;
//      out->est_angvel_z = control_.wz*1000;
//      
//      out->est_acc_x = accel_[0]*10000;
//      out->est_acc_y = accel_[1]*10000;
//      out->est_acc_z = accel_[2]*10000;
//      
//      out->pwm1 = pwms_[0];
//      out->pwm2 = pwms_[1];
//      out->pwm3 = pwms_[2];
//      out->pwm4 = pwms_[3];
//      
//      out->diff = loopDiff_;
//      out->errorCode = static_cast<uint16_t>(errno_.code);
//      out->errorSubcode = static_cast<uint16_t>(errno_.subcode);
      
      //  TODO: This may hang, change the structure
      uint8_t ret=0;
      do {
        ret = ZigBee_ReadState();
      } while (ret != STATUS_PLL_ON);
      ZigBee_WriteFrame(tx_buf, 1+sizeof(OUTPUT_DATA));
      
      do {
        ret = ZigBee_ReadIrq();
      } while ((ret & 0x08) == 0);
      ZigBee_SetModeRx();
//
      commTime_ = GetMicroseconds();
      
      //mYellowON;  //  TEMP: for debugging, remove me later...
    }
  }
}

bool Quadrotor::processSensors() {
  MPU6050::ErrorCode code;
 
  //  get result from MPU6050 and start next read
  code = mpu_->finishRead();
  mpu_->startRead();
  
  if (code == 0) {
    //  read without issue
    for (int i=0; i < 3; i++) {
      accel_[i] = mpu_->accel[i] * kr::AttitudeESKF::kOneG;
      gyro_[i] = mpu_->gyro[i];
    }
    return true;
  }
  
  setErrno(Error::IMURead, 0);
  return false;
}

void Quadrotor::processState() {
  eskf_.predict(gyro_, loopDiff_ / 1e6f, gyroCov_, false);
  eskf_.update(accel_, accelCov_);
  
  //usb_->printf("%u\n", loopDiff_);
  
  if ( !eskf_.isStable() ) {
    setErrno(Error::FilterSingular, 0);
  }
  
  if (useExtYaw_) {
    //  merge vicon yaw with our estimate
    eskf_.externalYawUpdate(control_.extYaw, 0.5f);
    useExtYaw_ = false;
  }
  
  //  calculate RPY and send to controller
  const Eigen::Vector3f rpy =
    kr::AttitudeESKF::getRPY(eskf_.getQuat().matrix());
  
  control_.roll = rpy[0];
  control_.pitch = rpy[1];
  control_.yaw = rpy[2];
    
  //  rates with bias subtracted
  const Eigen::Vector3f w = eskf_.getAngularVelocity();
  control_.wx = w[0];
  control_.wy = w[1];
  control_.wz = w[2];
}

void Quadrotor::processControl() {
  
  const uint32_t pwmPeriod = 36000;//GetPWMPeriod();  //  frequency of the motor PWM
  //usb_->printf("%i pwm\n", pwmPeriod);
  
  control_.deltaRoll = normalize_angle(control_.desRoll - control_.roll);
  control_.deltaPitch = normalize_angle(control_.desPitch - control_.pitch);
  control_.deltaYaw = normalize_angle(control_.desYaw - control_.yaw);
  
  float dox = control_.Kp * control_.deltaRoll + control_.Kd * -control_.wx;
  float doy = control_.Kp * control_.deltaPitch + control_.Kd * -control_.wy;
  float doz = control_.KpYaw * control_.deltaYaw + control_.KdYaw * -control_.wz;
  if (recoveryMode_) {
    doz = control_.KdTumbleYaw * -control_.wz;
    if (std::abs(control_.wz) < 0.175) {
      //  assume we have recovered
      recovered_ = true;
    }
  }
  
  //  scale up to increase useful gains
  dox *= 10;
  doy *= 10;
  doz *= 10;
  
  bool shutoffRegion = false;
  if (control_.desThrust < 0.02f) {
    shutoffRegion = true;
  }
  
  float pwmBase = control_.desThrust * pwmPeriod;
  
  //  clamp PWM signals to reasonable values
  float pwmOut[4];
  pwmOut[0] = pwmBase - doy + doz;
  pwmOut[1] = pwmBase + dox - doz;
  pwmOut[2] = pwmBase + doy + doz;
  pwmOut[3] = pwmBase - dox - doz;
  
  for (int i=0; i < 4; i++) {
    if (pwmOut[i] > pwmPeriod) {
      pwmOut[i] = static_cast<float>(pwmPeriod);
    } else if (pwmOut[i] < 0.0f) {
      pwmOut[i] = 0.0f;
    }
    pwms_[i] = static_cast<uint16_t>(pwmOut[i]);
  }
  
  //  order: 1 4 2 3
  const uint8_t perm[] = {1,4,2,3};
  
  if (motorsEnabled_ && !shutoffRegion) {
    for (int i=0; i < 4; i++) {
      SetPWM(perm[i], pwmOut[i] / pwmPeriod);
    }
  } else {
    for (int i=0; i < 4; i++) {
      SetPWM(perm[i], 0.0f);
    }
  }
}

void Quadrotor::updateLEDs()
{
  //  toggle status LED
  if ( MicrosecondsElapsed(ledTime_) > 1e6 ) {
    mGreenTOGGLE;
    ledTime_ = GetMicroseconds();
  }
  
  mRed(!motorsEnabled_);
}
