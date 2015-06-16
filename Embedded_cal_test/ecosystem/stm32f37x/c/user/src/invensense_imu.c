#include "invensense_imu.h"

#ifdef USE_IMU_INTERFACE

#define USE_EEPROM


// Imu Address on I2C
const uint8_t kImuAddress = 0b11010000;

// MPU 6050 Registers and Default Settings
// use PLL with X-axis gyro for MPU CLKSEL
const uint8_t kMpuPwrMgmt1Register = 0x6B;
const uint8_t kMpuPwrMgmt1Value = 0x01;

// built-in low pass filter, bandwidth 94-98 Hz
const uint8_t kMpuConfigRegister = 0x1A;
const uint8_t kMpuConfigValue = 0x02;

// nominal value for +/-2g, but this yields +/-4g on early run parts
const uint8_t kMpuAccelConfigRegister = 0x1C;
const uint8_t kMpuAccelConfigValue = 0<<3;

// nominal value +-500 deg/s
const uint8_t kMpuGyroConfigRegister = 0x1B;
const uint8_t kMpuGyroConfigValue = 1<<3;


void ConfigureImu() {

  while(!mBusPeriphReady(kImuAddress,1)) {
    DelayMicroseconds(1000);
  }
  
  mBusWrite(kImuAddress, kMpuPwrMgmt1Register, kMpuPwrMgmt1Value, 1);
  DelayMicroseconds(1000);
  
  mBusWrite(kImuAddress, kMpuConfigRegister, kMpuConfigValue, 1);
  DelayMicroseconds(1000);
  
  mBusWrite(kImuAddress, kMpuAccelConfigRegister, kMpuAccelConfigValue, 1);
  DelayMicroseconds(1000);
  
  mBusWrite(kImuAddress, kMpuGyroConfigRegister, kMpuGyroConfigValue, 1);  
}


void InitializeImu(int16_t offset[6], float scale[6]) {

  #ifdef USE_EEPROM
  // test read to see if initialized
  uint32_t temp;
  uint16_t is_initialized = (0 == EE_ReadVariable(kEepromAccelOffsetX, &temp));
  
  if(is_initialized == 1) {
    ReadImuOffset(offset);
    ReadImuScale(scale);
  }
  else {
  #endif
    DefaultImuOffset(offset);
    DefaultImuScale(scale);
  #ifdef USE_EEPROM      
    WriteImuOffset(offset);
    WriteImuScale(scale);
  }
  #endif
}

void ReadImuStart() {
  mBusReadBurstStartNB(kImuAddress, 0x3B, 14, 1);
}


uint8_t ReadImuData(int16_t imu_raw[6], float* imu_time) {
  uint8_t raw_imu_data_buf[14];
  if(mBusReadBurstDataNB(14, raw_imu_data_buf, 1) != ERROR) {
    imu_raw[0] = 256*raw_imu_data_buf[ 0] + raw_imu_data_buf[ 1]; // ax
    imu_raw[1] = 256*raw_imu_data_buf[ 2] + raw_imu_data_buf[ 3]; // ay
    imu_raw[2] = 256*raw_imu_data_buf[ 4] + raw_imu_data_buf[ 5]; // az
    imu_raw[3] = 256*raw_imu_data_buf[ 8] + raw_imu_data_buf[ 9]; // wx
    imu_raw[4] = 256*raw_imu_data_buf[10] + raw_imu_data_buf[11]; // wy
    imu_raw[5] = 256*raw_imu_data_buf[12] + raw_imu_data_buf[13]; // wz
    *imu_time = GetSeconds();
    return(1);
  }
  else {
    return(0);
  }
}


void ScaleImu(const int16_t imu_raw[6], const int16_t imu_offset[6],
              const float imu_scale[6], float imu_metric[6]) {
  for(uint8_t i=0; i<6; i++) {
    imu_metric[i] = (imu_raw[i] - imu_offset[i]) * imu_scale[i];
  }
}
  

void DefaultImuOffset(int16_t offset[6]) {
  int16_t default_offset[6] = {0, 0, 0, 0, 0, 0};
  memcpy(offset, default_offset, 6*sizeof(int16_t));
}


void ReadImuOffset(int16_t offset[6]) {
  #ifdef USE_EEPROM
  int32_t value;
  EE_ReadVariable(kEepromAccelOffsetX, (uint32_t*)&value);
    offset[0] = value;
  EE_ReadVariable(kEepromAccelOffsetY, (uint32_t*)&value);
    offset[1] = value;
  EE_ReadVariable(kEepromAccelOffsetZ, (uint32_t*)&value);
    offset[2] = value;
  EE_ReadVariable(kEepromGyroOffsetX, (uint32_t*)&value);
    offset[3] = value;
  EE_ReadVariable(kEepromGyroOffsetY, (uint32_t*)&value);
    offset[4] = value;
  EE_ReadVariable(kEepromGyroOffsetZ, (uint32_t*)&value);
    offset[5] = value;
  #endif
}


void WriteImuOffset(int16_t offset[6]) {
  #ifdef USE_EEPROM
  EE_WriteVariable(kEepromAccelOffsetX, offset[0]);
  EE_WriteVariable(kEepromAccelOffsetY, offset[1]);
  EE_WriteVariable(kEepromAccelOffsetZ, offset[2]);
  EE_WriteVariable(kEepromGyroOffsetX,  offset[3]);
  EE_WriteVariable(kEepromGyroOffsetY,  offset[4]);
  EE_WriteVariable(kEepromGyroOffsetZ,  offset[5]);
  #endif
}


void DefaultImuScale(float scale[6]) {
  //float default_scale[6] = {5.98e-4, 5.98e-4, 5.98e-4, 2.66e-4, 2.66e-4, 2.66e-4};
  float default_scale[6] = {1.20e-3, 1.20e-3, 1.20e-3, 2.66e-4, 2.66e-4, 2.66e-4};
  memcpy(scale, default_scale, 6*sizeof(float));
}


void ReadImuScale(float scale[6]) {
  #ifdef USE_EEPROM
  EE_ReadVariable(kEepromAccelScaleX, (uint32_t*)&scale[0]);
  EE_ReadVariable(kEepromAccelScaleY, (uint32_t*)&scale[1]);
  EE_ReadVariable(kEepromAccelScaleZ, (uint32_t*)&scale[2]);
  EE_ReadVariable(kEepromGyroScaleX, (uint32_t*)&scale[3]);
  EE_ReadVariable(kEepromGyroScaleY, (uint32_t*)&scale[4]);
  EE_ReadVariable(kEepromGyroScaleZ, (uint32_t*)&scale[5]);
  #endif
}


void WriteImuScale(float scale[6]) {
  #ifdef USE_EEPROM
  EE_WriteVariable(kEepromAccelScaleX, *(uint32_t*)&scale[0]);
  EE_WriteVariable(kEepromAccelScaleY, *(uint32_t*)&scale[1]);
  EE_WriteVariable(kEepromAccelScaleZ, *(uint32_t*)&scale[2]);
  EE_WriteVariable(kEepromGyroScaleX, *(uint32_t*)&scale[3]);
  EE_WriteVariable(kEepromGyroScaleY, *(uint32_t*)&scale[4]);
  EE_WriteVariable(kEepromGyroScaleZ, *(uint32_t*)&scale[5]);
  #endif
}


void CalibrateImuGyroOffset(int16_t offset[3]) {
  const uint16_t kBiasSamples = 1000;
  int32_t raw_imu_data_accumulator[6];
  memset(raw_imu_data_accumulator, 0, 6*sizeof(int32_t));
  
  for(uint16_t i=0; i<kBiasSamples; i++) {
    uint8_t raw_imu_data_buf[14];
    mBusReadBurstStartNB(kImuAddress, 0x3B, 14,1);
    while(mBusReadBurstDataNB(14, raw_imu_data_buf, 1) == ERROR) {}
    raw_imu_data_accumulator[0] 
      += (int16_t)(256*raw_imu_data_buf[ 0] + raw_imu_data_buf[ 1]);   // ax
    raw_imu_data_accumulator[1] 
      += (int16_t)(256*raw_imu_data_buf[ 2] + raw_imu_data_buf[ 3]);   // ay
    raw_imu_data_accumulator[2] 
      += (int16_t)(256*raw_imu_data_buf[ 4] + raw_imu_data_buf[ 5]);   // az
    raw_imu_data_accumulator[3] 
      += (int16_t)(256*raw_imu_data_buf[ 8] + raw_imu_data_buf[ 9]);   // wx
    raw_imu_data_accumulator[4] 
      += (int16_t)(256*raw_imu_data_buf[10] + raw_imu_data_buf[11]); // wy
    raw_imu_data_accumulator[5] 
      += (int16_t)(256*raw_imu_data_buf[12] + raw_imu_data_buf[13]); // wz
    DelayMicroseconds(1000);
  }
  
  // calculate gyro biases
  offset[0] = (int16_t)(raw_imu_data_accumulator[3] / kBiasSamples);
  offset[1] = (int16_t)(raw_imu_data_accumulator[4] / kBiasSamples);
  offset[2] = (int16_t)(raw_imu_data_accumulator[5] / kBiasSamples);
  
  #ifdef USE_EEPROM
  EE_WriteVariable(kEepromGyroOffsetX,  offset[0]);
  EE_WriteVariable(kEepromGyroOffsetY,  offset[1]);
  EE_WriteVariable(kEepromGyroOffsetZ,  offset[2]);
  #endif
}

#endif // USE_IMU_INTERFACE
