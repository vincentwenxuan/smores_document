#ifndef INVENSENSE_IMU_H
#define INVENSENSE_IMU_H

#include "common_peripherals.h"

#ifdef USE_IMU_INTERFACE

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

////////////////////////////////////
/// IMU Configuration/Initialization

/// Configure IMU Registers
void ConfigureImu();

/// Load EEPROM or Default Values into provided offset and scale arrays
void InitializeImu(int16_t imu_offset[6], float imu_scale[6]);

////////////////////////////////////
/// Data Collection

/// Start a burst read
void ReadImuStart();

/// Read data, return 1 on success or 0 if not ready
uint8_t ReadImuData(int16_t imu_raw[6], float* imu_time);

/// Convert raw measurements to m/s2 and rad/s
void ScaleImu(const int16_t imu_raw[6], const int16_t imu_offset[6],
              const float imu_scale[6], float imu_metric[6]);

////////////////////////////////////
/// Default Values

// Get default IMU offset
void DefaultImuOffset(int16_t offset[6]);

// Get default IMU scale
void DefaultImuScale(float scale[6]);

////////////////////////////////////
/// EEPROM Accessing Operations

// Read IMU offset from EEPROM
void ReadImuOffset(int16_t offset[6]);

// Write IMU offset to EEPROM
void WriteImuOffset(int16_t offset[6]);

/// Read IMU scale from EEPROM
void ReadImuScale(float scale[6]);

/// Write IMU scale to EEPROM
void WriteImuScale(float scale[6]);

/// Calculate new IMU offset and write to EEPROM
void CalibrateImuGyroOffset(int16_t offset[3]);

#ifdef __cplusplus
}
#endif

#endif // USE_IMU_INTERFACE

#endif // INVENSENSE_IMU_H
