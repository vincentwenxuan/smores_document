#ifndef MPU_60XX_IMU_H
#define MPU_60XX_IMU_H

#include "common_peripherals.h"
#include "persistent_memory.h"
#include "common_message_types.h"
#include "system_clock.h"

#include <stdint.h>

#ifdef USING_MPU_60XX_IMU

class Mpu60XXImu {

  public:
    struct Time time;
    float w[3];         // rotation rate xyz, rad/s
    float a[3];         // accelleration xyz, m/s^2
    int16_t w_raw[3];   // rotation rate xyz, bits
    int16_t a_raw[3];   // accelleration xyz, bits
    uint8_t flip_z; // 0 normally, 1 if flip z-axis

    ////////////////////////////////////////////////////////////////////////////
    /// BASIC API

    /// Constructors without memory.
    /// One-shot object initialization and attached hardware configuration.
    Mpu60XXImu();
    Mpu60XXImu(uint8_t w_fs_sel, uint8_t a_fs_sel);
    
    #ifdef USING_PERSISTENT_MEMORY
    /// Constructor with memory.
    /// Requires call to InitFromMemory() after Freeze()
    Mpu60XXImu(PersistentMemory& mem);
    #endif
    
    #ifdef USING_PERSISTENT_MEMORY
    /// Initialize object and configure attached hardware from memory.
    void InitFromMemory();
    #endif
    
    /// Start read from gyro and accellerometer.
    void StartRead();

    /// Poll for ready data, return 1 on success or 0 if not ready
    uint8_t FinishRead();
    
    /// Perform live calibration and set gyro offset
    void CalibrateGyroOffset();

    ////////////////////////////////////////////////////////////////////////////
    /// MESSAGE OPERATIONS
    
    /// Read message and act (presently over USB, in future general)
    void ReadMsg(CommunicationInterface& com, uint8_t* rx_data, uint8_t rx_length);
    
    /// Fill raw measurement message from the IMU
    void FillMsgImuRaw(MsgImuRaw *msg);

    /// Send raw measurement from the IMU
    void SendRaw(CommunicationInterface& com);
    
    /// Fill metric measurement message from the IMu
    void FillMsgImuMetric(MsgImuMetric *msg);

    /// Send metric measurement from the IMU
    void SendMetric(CommunicationInterface& com);
    
    ////////////////////////////////////////////////////////////////////////////
    /// DEFAULT SETTINGS
    
    /// Set default IMU offset
    void DefaultOffset();
    
    /// Set gyro sensitivity and load default scale (no write to imu)
    void DefaultGyroSensitivity(uint8_t fs_sel);
    
    /// Set accellerometer sensitivity and load default scale (no write to imu)
    void DefaultAccelSensitivity(uint8_t fs_sel);

    ////////////////////////////////////////////////////////////////////////////
    /// Memory Accessing Operations
    #ifdef USING_PERSISTENT_MEMORY
    /// Write all settings to memory.
    void WriteAll();
    
    /// Read gyro sensitivity setting
    void ReadGyroSensitivity();
    
    /// Write gyro sensitivity setting
    void WriteGyroSensitivity();
    
    /// Read accel sensitivity setting
    void ReadAccelSensitivity();
    
    /// Write accel sensitivity setting
    void WriteAccelSensitivity();

    /// Read IMU offsets
    void ReadOffset();

    /// Write IMU offsets
    void WriteOffset();

    /// Read IMU scale from memory
    void ReadScale();

    /// Write IMU scale to memory
    void WriteScale();
    #endif

private:

    #ifdef USING_PERSISTENT_MEMORY
    PersistentMemory* mem;
    #endif
    
    uint8_t a_fs_sel;
    uint8_t w_fs_sel;
    int16_t a_offset[3];
    int16_t w_offset[3];
    float a_scale[3];
    float w_scale[3];
    
    ////////////////////////////////////////////////////////////////////////////
    // ATTACHED HARDWARE CONFIGURATION
    
    /// Initialize object and configure attached hardware from defaults
    void InitFromDefault(uint8_t gyro_fs, uint8_t accel_fs);
    
    /// Configure attached hardware: R26, R107 registers
    void ConfigMpu60XX();
    
    /// Configure attached hardware: GYRO_CONFIG register
    void ConfigGyroSensitivity();

    /// Configure attached hardware: ACCEL_CONFIG register
    void ConfigAccelSensitivity();
};

#endif // MPU_60XX_IMU_H

#endif // USING_MPU_60XX_IMU
