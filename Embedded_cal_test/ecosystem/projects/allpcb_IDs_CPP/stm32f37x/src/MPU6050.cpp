/*
 * MPU6050.cpp
 *
 * Copyright (c) 2013 Gareth Cross, MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 *  Created on: Sep 21, 2013
 *      Author: gareth
 *
 *  Updated for PPR on: June 30, 2014
 */

#include "MPU6050.h"
#include <string.h>

/*
 *  Register IDs
 */

#define MPU6050_SMPRT_DIV     (0x19)
#define MPU6050_CONFIG        (0x1A)
#define MPU6050_GYRO_CONFIG		(0x1B)
#define MPU6050_ACCEL_CONFIG	(0x1C)
#define	MPU6050_PWR_MGMT_1		(0x6B)

#define MPU6050_ACCEL_XOUT_H	(0x3B)
#define MPU6050_ACCEL_XOUT_L	(0x3C)
#define MPU6050_ACCEL_YOUT_H	(0x3D)
#define MPU6050_ACCEL_YOUT_L	(0x3E)
#define MPU6050_ACCEL_ZOUT_H	(0x3F)
#define MPU6050_ACCEL_ZOUT_L	(0x40)

#define MPU6050_TEMP_OUT_H		(0x41)
#define MPU6050_TEMP_OUT_L		(0x42)

#define MPU6050_GYRO_XOUT_H		(0x43)
#define MPU6050_GYRO_XOUT_L		(0x44)
#define MPU6050_GYRO_YOUT_H		(0x45)
#define MPU6050_GYRO_YOUT_L		(0x46)
#define MPU6050_GYRO_ZOUT_H		(0x47)
#define MPU6050_GYRO_ZOUT_L		(0x48)

#define MPU6050_WHO_AM_I      (0x75)

#define MPU6050_DEG2RAD       (0.017453f)

#define HLTOSHORT(h, l) ((short)((h) << 8) | (short)(l))

MPU6050::MPU6050(uint8_t devAddr, UsbInterface * debugInft) : devAddr_(devAddr)
{
	numRetries_ = 0;
  delayUsec_ = 0;
  debugIntf_ = debugInft;
  gyroMultiplier_=0;
  accelMultiplier_=0;
  
  memset(accel,0,sizeof(float)*3);
  memset(gyro,0,sizeof(float)*3);
  temp = 0;
}

MPU6050::~MPU6050()
{
}

MPU6050::ErrorCode MPU6050::read_reg(uint8_t addr, uint8_t& data) const
{
  uint8_t count=0;
  uint8_t status;

  //  initiate a read
  DelayMicroseconds(delayUsec_);
  mBusReadBurstStartNB(devAddr_, addr, 1, IMU_MBUS);
  
  do {
    //  always delay before readback
    DelayMicroseconds(delayUsec_);
    status = mBusReadBurstDataNB(1, &data, IMU_MBUS);
  } while (status == ERROR && count++ < numRetries_);
  
  if (status == ERROR) {
    return ReadFailed;
  }
  
  return NoError;
}

MPU6050::ErrorCode MPU6050::write_reg(uint8_t addr, uint8_t data)
{
  uint8_t count=0;
  uint8_t status;
  do {
    DelayMicroseconds(delayUsec_);
    status = mBusWriteBurstNB(devAddr_, addr, 1, &data, IMU_MBUS);
  } while (status == ERROR && count++ < numRetries_);
  
  if (status == ERROR) {
    return WriteFailed;
  }
  
  return NoError;
}

MPU6050::ErrorCode MPU6050::assert_reg(uint8_t addr, uint8_t value, uint8_t mask) const
{
  //  wait after last write before checking
  DelayMicroseconds(delayUsec_);
  
  uint8_t result;
  ErrorCode code = read_reg(addr, result);
  if (code) {
    return code;
  }
  
  value &= mask;  //  compare only masked bits
  result &= mask;
  
#ifdef MPU6050_DEBUG
  if (debugIntf_) {
    if (value != result) {
      debugIntf_->printf("Tried to write %u to %u, but got back %u\n", value, addr, result);
    }
  }
#endif
  
  
  if (result != value) {
    return NoEffect;
  }
  return NoError;
}

MPU6050::ErrorCode MPU6050::write_range(uint8_t addr, uint8_t range)
{
  uint8_t value;
  
  ErrorCode code = read_reg(addr, value);
  if (code) {
    return code;
  }
  
  //  disable self test, we do not support this
  value &= ~(0xE0);
  value |= (range << 3);
  
  code = write_reg(addr, value);
  if (code) {
    return code;
  }
  
  return assert_reg(addr, value, 0x18); //  mask only range bits
}

void MPU6050::setCommRetries(uint8_t numRetries) {
  numRetries_ = numRetries;
}

void MPU6050::setCommDelay(uint16_t delayUsec) {
  delayUsec_ = delayUsec;
}

bool MPU6050::isReady() const
{
  //  check if peripheral is ready
  if (mBusPeriphReady(devAddr_, IMU_MBUS) == SUCCESS) {
    return true;
  }
  return false;
}

MPU6050::ErrorCode MPU6050::testConnection() const
{
  uint8_t magic;
  ErrorCode code = read_reg(MPU6050_WHO_AM_I, magic);
  if (code) {
    return code;
  }
  
  //  this is the magic number for an MPU6050
  if (magic != 0x68) {
    return BadReply;
  }
  
	return NoError;
}

MPU6050::ErrorCode MPU6050::reset()
{
  uint8_t reset_code=0x80;
  ErrorCode code = write_reg(MPU6050_PWR_MGMT_1, reset_code);
  return code;
}

MPU6050::ErrorCode MPU6050::setSleep(bool sleep)
{
  uint8_t value;
  ErrorCode code = read_reg(MPU6050_PWR_MGMT_1, value);
  if (code) {
    return code;
  }
  
  if (sleep) {
    value |= 0x40;  //  bit 6
  } else {
    value &= ~(0x40);
  }
  
  //  apply setting and check result
  code = write_reg(MPU6050_PWR_MGMT_1, value);
  if (code) {
    return code;
  }
  
  return assert_reg(MPU6050_PWR_MGMT_1, value);
}

MPU6050::ErrorCode MPU6050::setSampleRate(unsigned int rate_hz)
{
  //  calculate div factor
  const int div = (8000 / rate_hz) - 1;
  if (div < 0 || div > 255) {
    return InvalidArg;
  }
  
  uint8_t arg = static_cast<uint8_t>(div);
  ErrorCode code = write_reg(MPU6050_SMPRT_DIV, arg);
  if (code) {
    return code;
  }
    
  //  check result
  return assert_reg(MPU6050_SMPRT_DIV, arg);
}

MPU6050::ErrorCode MPU6050::setFilterMode(MPU6050::FilterMode mode)
{
  if (mode < 0 || mode > 6) {
    return InvalidArg;
  }
  
  uint8_t arg = static_cast<uint8_t>(mode);
  uint8_t value;
  
  ErrorCode code = read_reg(MPU6050_CONFIG, value);
  if (code) {
    return code;
  }
  
  //  disable EXT_SYNC_SET, we do not support this
  value &= ~(0x38);
  value |= arg;
  
  code = write_reg(MPU6050_CONFIG, value);
  if (code) {
    return code;
  }
  
  return assert_reg(MPU6050_CONFIG, value, 0x07); //  mask to bottom 3 bits
}

MPU6050::ErrorCode MPU6050::setAccelRange(MPU6050::AccelRange range)
{
  if (range < 0 || range > 3) {
    return InvalidArg;
  }
  
  ErrorCode code = write_range(MPU6050_ACCEL_CONFIG, range);
  if (code) {
    return code;
  }
  
  //  change multiplier
  const float accel_sens[] = {16384.0f, 8192.0f, 4096.0f, 2048.0f};
  accelMultiplier_ = 1.0f / accel_sens[range];
  
  return NoError;
}

MPU6050::ErrorCode MPU6050::setGyroRange(MPU6050::GyroRange range)
{
  if (range < 0 || range > 3) {
    return InvalidArg;
  }
  
  ErrorCode code = write_range(MPU6050_GYRO_CONFIG, range);
  if (code) {
    return code;
  }
  
  const float gyro_sens[] = {131.0f, 65.5f, 32.8f, 16.4f};
  gyroMultiplier_ = 1.0f / gyro_sens[range];
  
  return NoError;
}

void MPU6050::startRead()
{
  //  start a burst read
  //  14 bytes = 6 accel, 2 temp, 6 gyro
  DelayMicroseconds(delayUsec_);
  mBusReadBurstStartNB(devAddr_, MPU6050_ACCEL_XOUT_H, 14, IMU_MBUS);
}

MPU6050::ErrorCode MPU6050::finishRead()
{
  uint8_t data[14];
  uint8_t status, count=0;
  
  do {
    DelayMicroseconds(delayUsec_);
    status = mBusReadBurstDataNB(14, &data[0], IMU_MBUS);
  } while(status == ERROR && count++ < numRetries_);
  
  if (status == ERROR) {
    return ReadFailed;
  }
  
  //  otherwise, decode data
  accel[0] = HLTOSHORT(data[0], data[1]) * accelMultiplier_;
  accel[1] = HLTOSHORT(data[2], data[3]) * accelMultiplier_;
  accel[2] = HLTOSHORT(data[4], data[5]) * accelMultiplier_;
  
  temp = HLTOSHORT(data[6], data[7]) / 340.0f + 36.53f;
  
  gyro[0] = HLTOSHORT(data[8], data[9]) * gyroMultiplier_ * MPU6050_DEG2RAD;
  gyro[1] = HLTOSHORT(data[10], data[11]) * gyroMultiplier_ * MPU6050_DEG2RAD;
  gyro[2] = HLTOSHORT(data[12], data[13]) * gyroMultiplier_ * MPU6050_DEG2RAD;
  
  return NoError;
}
