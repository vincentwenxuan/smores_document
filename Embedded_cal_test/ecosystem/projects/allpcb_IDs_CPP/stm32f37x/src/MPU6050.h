/*
 * MPU6050.h
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

#ifndef MPU6050_H_
#define MPU6050_H_

#include "mBus.h"
#include "system_clock.h"
#include "usb_interface.h"

#define MPU6050_DEBUG

class MPU6050
{
public:
  
  enum ErrorCode {
    NoError     = 0,       /// Operation succeeded
    ReadFailed  = 1,       /// Read operation failed
    WriteFailed = 2,       /// Write operation failed
    BadReply    = 3,       /// Incorret/malformed reply from device
    InvalidArg  = 4,       /// Invalid argument to function
    NoEffect    = 5,       /// Change did not affect register
  };
  
  enum FilterMode {
    LPF_A260_G256 = 0,  /// ~0ms delay
    LPF_A184_G188 = 1,  /// ~2ms delay
    LPF_A94_G98   = 2,  /// ~3ms delay
    LPF_A44_G42   = 3,  /// ~5ms delay
    LPF_A21_G20   = 4,  /// ~9ms delay
    LPF_A10_G10   = 5,  /// ~14ms delay
    LPF_A5_G5     = 6,  /// ~19ms delay
  };
  
  enum AccelRange {
    Accel2G   = 0,  /// +/- 2gs accel
    Accel4G   = 1,  /// +/- 4gs accel
    Accel8G   = 2,  /// +/- 8gs accel
    Accel16G  = 3,  /// +/- 16gs accel
  };
  
  enum GyroRange {
    Gyro250DPS  = 0,  /// 250 deg/s
    Gyro500DPS  = 1,  /// 500 deg/s
    Gyro1000DPS = 2,  /// 1000 deg/s
    Gyro2000DPS = 3,  /// 2000 deg/s
  };
  
  /**
   *  @brief Constructor
   *  @param deviceAddr Device I2C address.
   *  @param intf Interface for debug printf (if MPU6050_DEBUG is defined).
   *
   *  @note Initializes scale factors to zero.
   */
	MPU6050(uint8_t deviceAddr, UsbInterface * intf = 0);
  
  /**
   *  @brief Destructor
   */
	virtual ~MPU6050();

  /**
   *  @brief Set the number of attempts to make when reading/writing.
   *  @param numRetries After this many attempts, the driver will return failure.
   *  @note Defaults to 0.
   */
  void setCommRetries(uint8_t numRetries);
  
  /**
   *  @brief Set the standard delay to insert between subsequent reads/writes.
   *  @param delayUsec Delay in microseconds.
   *  @note Defaults to 0. Any command will take 2-3x this delay at at minimum, so keep
   *    the value small: 50-100us.
   */
  void setCommDelay(uint16_t delayUsec);
  
  /**
   *  @brief Is the I2C peripheral ready?
   */
  bool isReady() const;

  /**
   *  @brief Check if the I2C bus is connected to an MPU6050
   */
	ErrorCode testConnection() const;

  /**
   *  @brief Write the RESET bit high. On success, device reboots.
   *  @note It is recommended that you sleep 50-100ms after calling this method.
   */
  ErrorCode reset();
  
  /**
   *  @brief Turn sleep mode on or off.
   *  @param sleep If true, the device goes to sleep.
   *  @note The MPU6050 starts up in sleep mode.
   */
  ErrorCode setSleep(bool sleep);
  
  /**
   *  @brief Set the sample rate.
   *  @param rate_hz Rate in hz. Must satisfy: 0 < hz <= 8000
   */
  ErrorCode setSampleRate(unsigned int rate_hz);
  
  /**
   *  @brief Set the digital low pass filter mode.
   */
  ErrorCode setFilterMode(FilterMode mode);
  
  /**
   *  @brief Set the accelerometer range.
   */
  ErrorCode setAccelRange(AccelRange range);
  
  /**
   *  @brief Set the gyroscope range.
   */
  ErrorCode setGyroRange(GyroRange range);

  /**
   *  @brief Begin reading all data registers.
   */
  void startRead();
  
  /**
   *  @brief Finish reading all data registers.
   *  @note If this succeeds, accel, gyro and temp will all be set.
   */
  ErrorCode finishRead();
  
public:
  /*
   *  Public registers
   */
  
  float accel[3];   /// Acceleration in Gs
  float gyro[3];    /// Angular rate in rad/s
  float temp;       /// Device temperature in degrees celsius
  
private:
  
  //  read a single byte register
  ErrorCode read_reg(uint8_t addr, uint8_t& data) const;
  
  //  write to a single byte register
  ErrorCode write_reg(uint8_t addr, uint8_t data);
  
  //  check the value of a single byte register
  ErrorCode assert_reg(uint8_t addr, uint8_t value, uint8_t mask = 0xFF) const;
  
  //  change one of the range registers
  ErrorCode write_range(uint8_t addr, uint8_t value);
  
  uint8_t devAddr_;
  
  uint8_t numRetries_;
  uint16_t delayUsec_;
  
  UsbInterface * debugIntf_;
  
	float accelMultiplier_;
	float gyroMultiplier_;
};

#endif // MPU6050_H_
