#include "mpu_60XX_imu.h"
#include "common_message_types.h"
#include "usb_interface.h"
#include "math_helper.h"
#include "mBus.h"
#include "common_peripherals.h"
#include "system_clock.h"

#ifdef USING_MPU_60XX_IMU

// Imu Address on I2C
const uint8_t kImuAddress = 0b11010000;

// MPU 6050 Registers and Default Settings
// use PLL with X-axis gyro for MPU CLKSEL
const uint8_t kMpuPwrMgmt1Register  = 0x6B;
const uint8_t kMpuPwrMgmt1Value     = 0x01;

const uint8_t kMpuConfigRegister  = 0x1A;
const uint8_t kMpuConfigValue     = 0x02;  // built-in LPF, bandwidth 94-98 Hz

const uint8_t kMpuAccelConfigRegister = 0x1C; // datasheet: ACCEL_CONFIG
const uint8_t kMpuGyroConfigRegister  = 0x1B; // datasheet: GYRO_CONFIG

// ImuCommand message subtypes
const uint8_t kImuCommandDefaultOffset        = 0;
const uint8_t kImuCommandDefaultScale         = 1;
const uint8_t kImuCommandCalibrateGyroOffset  = 2;
const uint8_t kImuCommandWriteAll             = 3;

const uint16_t kTypeIMU60XX = 0;

const uint16_t kSubtypeGyroOffsetX      = 0;
const uint16_t kSubtypeGyroOffsetY      = 1;
const uint16_t kSubtypeGyroOffsetZ      = 2;
const uint16_t kSubtypeGyroScaleX       = 6;
const uint16_t kSubtypeGyroScaleY       = 7;
const uint16_t kSubtypeGyroScaleZ       = 8;
const uint16_t kSubtypeGyroScaleSelect  = 12;

const uint16_t kSubtypeAccelOffsetX     = 3;
const uint16_t kSubtypeAccelOffsetY     = 4;
const uint16_t kSubtypeAccelOffsetZ     = 5;
const uint16_t kSubtypeAccelScaleX      = 9;
const uint16_t kSubtypeAccelScaleY      = 10;
const uint16_t kSubtypeAccelScaleZ      = 11;
const uint16_t kSubtypeAccelScaleSelect = 13;

// keys for persistent memory access
#define MPU_60XX_IMU_BASE_KEY 0x0000
const uint16_t key_wx_offset = MPU_60XX_IMU_BASE_KEY +  kSubtypeGyroOffsetX;
const uint16_t key_wy_offset = MPU_60XX_IMU_BASE_KEY +  kSubtypeGyroOffsetY;
const uint16_t key_wz_offset = MPU_60XX_IMU_BASE_KEY +  kSubtypeGyroOffsetZ;
const uint16_t key_ax_offset = MPU_60XX_IMU_BASE_KEY +  kSubtypeAccelOffsetX;
const uint16_t key_ay_offset = MPU_60XX_IMU_BASE_KEY +  kSubtypeAccelOffsetY;
const uint16_t key_az_offset = MPU_60XX_IMU_BASE_KEY +  kSubtypeAccelOffsetZ;

const uint16_t key_wx_scale  = MPU_60XX_IMU_BASE_KEY +  kSubtypeGyroScaleX;
const uint16_t key_wy_scale  = MPU_60XX_IMU_BASE_KEY +  kSubtypeGyroScaleY;
const uint16_t key_wz_scale  = MPU_60XX_IMU_BASE_KEY +  kSubtypeGyroScaleZ;
const uint16_t key_ax_scale  = MPU_60XX_IMU_BASE_KEY +  kSubtypeAccelScaleX;
const uint16_t key_ay_scale  = MPU_60XX_IMU_BASE_KEY +  kSubtypeAccelScaleY;
const uint16_t key_az_scale  = MPU_60XX_IMU_BASE_KEY +  kSubtypeAccelScaleZ;

const uint16_t key_w_fs_sel  = MPU_60XX_IMU_BASE_KEY + kSubtypeGyroScaleSelect;
const uint16_t key_a_fs_sel  = MPU_60XX_IMU_BASE_KEY + kSubtypeAccelScaleSelect;








////////////////////////////////////////////////////////////////////////////////
// BASIC API

// Default Constructor
// No memory: object initialization and attached hardware configuration.
// Default sensitivities.
Mpu60XXImu::Mpu60XXImu() {
  InitFromDefault(1, 0);
}

// Selectable Constructor
// No memory: object initialization and attached hardware configuration.
// Selectable sensitivities.
Mpu60XXImu::Mpu60XXImu(uint8_t w_fs_sel, uint8_t a_fs_sel) {
  InitFromDefault(w_fs_sel, a_fs_sel);
}

#ifdef USING_PERSISTENT_MEMORY
// Smart Constructor
// Needs memory: follow with InitFromMemory() after Freeze()
Mpu60XXImu::Mpu60XXImu(PersistentMemory& memory) {
  mem = &memory;
  
  mem->AddKey(key_wx_offset);
  mem->AddKey(key_wy_offset);
  mem->AddKey(key_wz_offset);
  mem->AddKey(key_ax_offset);
  mem->AddKey(key_ay_offset);
  mem->AddKey(key_az_offset);

  mem->AddKey(key_wx_scale);
  mem->AddKey(key_wy_scale);
  mem->AddKey(key_wz_scale);
  mem->AddKey(key_ax_scale);
  mem->AddKey(key_ay_scale);
  mem->AddKey(key_az_scale);
  
  mem->AddKey(key_w_fs_sel);
  mem->AddKey(key_a_fs_sel);

  // must initialize object after Freeze() using InitFromMemory()
}
#endif

#ifdef USING_PERSISTENT_MEMORY
/// Initialize object and configure attached hardware from memory
void Mpu60XXImu::InitFromMemory() {
  uint32_t temp;
  uint16_t retval = mem->Read(key_ax_offset, &temp); // 0 if valid data
  if(retval == 0) {
    // initialize object from flash
    ReadOffset();
    ReadScale();
    ReadGyroSensitivity();
    ReadAccelSensitivity();
    
    // configure attached hardware
    ConfigMpu60XX();
    ConfigGyroSensitivity();
    ConfigAccelSensitivity();
  }
  else {
    InitFromDefault(1, 0);  // initialize and configure from defaults
    WriteAll();
  }

  flip_z = 0;
}
#endif

/// Start read from gyro and accellerometer.
void Mpu60XXImu::StartRead() {
  mBusReadBurstStartNB(kImuAddress, 0x3B, 14, IMU_MBUS);
}

/// Poll for ready data, return 1 on success or 0 if not ready
uint8_t Mpu60XXImu::FinishRead() {
  uint8_t raw_imu_data_buf[14];
  if(mBusReadBurstDataNB(14, raw_imu_data_buf, IMU_MBUS) != ERROR) {

    time = GetTime();
      
    a_raw[0] = 256*raw_imu_data_buf[ 0] + raw_imu_data_buf[ 1]; // ax
    a_raw[1] = 256*raw_imu_data_buf[ 2] + raw_imu_data_buf[ 3]; // ay
    a_raw[2] = 256*raw_imu_data_buf[ 4] + raw_imu_data_buf[ 5]; // az
    
    w_raw[0] = 256*raw_imu_data_buf[ 8] + raw_imu_data_buf[ 9]; // wx
    w_raw[1] = 256*raw_imu_data_buf[10] + raw_imu_data_buf[11]; // wy
    w_raw[2] = 256*raw_imu_data_buf[12] + raw_imu_data_buf[13]; // wz

    if(flip_z) {
      a_raw[1] = 0xFFFF - a_raw[1]; // flip ay
      a_raw[2] = 0xFFFF - a_raw[2]; // flip az
      w_raw[1] = 0xFFFF - w_raw[1]; // flip wy
      w_raw[2] = 0xFFFF - w_raw[2]; // flip wz
    }

    for(uint8_t i=0; i<3; i++) {
      a[i] = (a_raw[i] - a_offset[i]) * a_scale[i];
      w[i] = (w_raw[i] - w_offset[i]) * w_scale[i];
    }
    return(1);
  }
  else {
    return(0);
  }
}

/// Perform live calibration and set gyro offset
void Mpu60XXImu::CalibrateGyroOffset() {
  const uint16_t kBiasSamples = 1000;
  int32_t raw_imu_data_accumulator[6];
  memset(raw_imu_data_accumulator, 0, 6*sizeof(int32_t));
  
  for(uint16_t i=0; i<kBiasSamples; i++) {

    uint8_t raw_imu_data_buf[14];

    while(mBusReadBurstDataNB(14, raw_imu_data_buf, IMU_MBUS) == ERROR) {} // why doesn't this work?

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

    DelayMicroseconds(50);
    mBusReadBurstStartNB(kImuAddress, 0x3B, 14, IMU_MBUS);
    DelayMilliseconds(1);
  }
  
  w_offset[0] = (int16_t)(raw_imu_data_accumulator[3] / kBiasSamples);
  w_offset[1] = (int16_t)(raw_imu_data_accumulator[4] / kBiasSamples);
  w_offset[2] = (int16_t)(raw_imu_data_accumulator[5] / kBiasSamples);

  if(flip_z) {
    w_offset[1] = 0xFFFF - w_offset[1];
    w_offset[2] = 0xFFFF - w_offset[2];
  }
}








////////////////////////////////////////////////////////////////////////////////
/// MESSAGE OPERATIONS

/// Read message and act (presently over USB, in future general)
void Mpu60XXImu::ReadMsg(CommunicationInterface& com, uint8_t* rx_data, uint8_t rx_length) {
  // rx_length intentionally unused, probably should be used

  uint8_t type = rx_data[0];

  if(type == kTypeIMU60XX && rx_length >= 3) {
    uint8_t subtype = rx_data[1];
    uint8_t direction = rx_data[2];
      
    if(direction == GET_CMD) {
      uint8_t tx_msg[6];
      tx_msg[0] = subtype;
      tx_msg[1] = RESPONSE_CMD;
      switch(subtype) {

        case kSubtypeGyroOffsetX:
          memcpy(&tx_msg[2], &w_offset[0], 2);
          com.SendPacket(kTypeIMU60XX, tx_msg, 4); break;
        case kSubtypeGyroOffsetY:
          memcpy(&tx_msg[2], &w_offset[1], 2);
          com.SendPacket(kTypeIMU60XX, tx_msg, 4); break;
        case kSubtypeGyroOffsetZ:
          memcpy(&tx_msg[2], &w_offset[2], 2);
          com.SendPacket(kTypeIMU60XX, tx_msg, 4); break;
        case kSubtypeGyroScaleX:
          memcpy(&tx_msg[2], &w_scale[0], 4);
          com.SendPacket(kTypeIMU60XX, tx_msg, 6); break;
        case kSubtypeGyroScaleY:
          memcpy(&tx_msg[2], &w_scale[1], 4);
          com.SendPacket(kTypeIMU60XX, tx_msg, 6); break;
        case kSubtypeGyroScaleZ:
          memcpy(&tx_msg[2], &w_scale[2], 4);
          com.SendPacket(kTypeIMU60XX, tx_msg, 6); break;
        case kSubtypeGyroScaleSelect:
          tx_msg[2] = w_fs_sel;
          com.SendPacket(kTypeIMU60XX, tx_msg, 3); break;

        case kSubtypeAccelOffsetX:
          memcpy(&tx_msg[2], &a_offset[0], 2);
          com.SendPacket(kTypeIMU60XX, tx_msg, 4); break;
        case kSubtypeAccelOffsetY:
          memcpy(&tx_msg[2], &a_offset[1], 2);
          com.SendPacket(kTypeIMU60XX, tx_msg, 4); break;
        case kSubtypeAccelOffsetZ:
          memcpy(&tx_msg[2], &a_offset[2], 2);
          com.SendPacket(kTypeIMU60XX, tx_msg, 4); break;
        case kSubtypeAccelScaleX:
          memcpy(&tx_msg[2], &a_scale[0], 4);
          com.SendPacket(kTypeIMU60XX, tx_msg, 6); break;
        case kSubtypeAccelScaleY:
          memcpy(&tx_msg[2], &a_scale[1], 4);
          com.SendPacket(kTypeIMU60XX, tx_msg, 6); break;
        case kSubtypeAccelScaleZ:
          memcpy(&tx_msg[2], &a_scale[2], 4);
          com.SendPacket(kTypeIMU60XX, tx_msg, 6); break;
        case kSubtypeAccelScaleSelect:
          tx_msg[2] = a_fs_sel;
          com.SendPacket(kTypeIMU60XX, tx_msg, 3); break;
      }
    } // end if get
    else if(direction == SET_CMD) {
      switch(subtype) {

        case kSubtypeGyroOffsetX:
          w_offset[0] =      PunToInt16(&rx_data[3]); break;
        case kSubtypeGyroOffsetY:
          w_offset[1] =      PunToInt16(&rx_data[3]); break;
        case kSubtypeGyroOffsetZ:
          w_offset[2] =      PunToInt16(&rx_data[3]); break;
        case kSubtypeGyroScaleX:
          w_scale[0] =      PunToFloat(&rx_data[3]);  break;
        case kSubtypeGyroScaleY:
          w_scale[1] =      PunToFloat(&rx_data[3]);  break;
        case kSubtypeGyroScaleZ:
          w_scale[2] =      PunToFloat(&rx_data[3]);  break;
        case kSubtypeGyroScaleSelect:
          w_fs_sel = rx_data[3];
          ConfigGyroSensitivity();                    break;

        case kSubtypeAccelOffsetX:
          a_offset[0] =      PunToInt16(&rx_data[3]); break;
        case kSubtypeAccelOffsetY:
          a_offset[1] =      PunToInt16(&rx_data[3]); break;
        case kSubtypeAccelOffsetZ:
          a_offset[2] =      PunToInt16(&rx_data[3]); break;
        case kSubtypeAccelScaleX:
          a_scale[0] =      PunToFloat(&rx_data[3]);  break;
        case kSubtypeAccelScaleY:
          a_scale[1] =      PunToFloat(&rx_data[3]);  break;
        case kSubtypeAccelScaleZ:
          a_scale[2] =      PunToFloat(&rx_data[3]);  break;
        case kSubtypeAccelScaleSelect:
          a_fs_sel = rx_data[3];
          ConfigAccelSensitivity();                   break;
      }
    } // end if set
    else if(direction == SAVE_CMD) {
      switch(subtype) {

        case kSubtypeGyroOffsetX:
          mem->Write(key_wx_offset, PunToUint32(w_offset[0]));  break;
        case kSubtypeGyroOffsetY:
          mem->Write(key_wy_offset, PunToUint32(w_offset[1]));  break;
        case kSubtypeGyroOffsetZ:
          mem->Write(key_wz_offset, PunToUint32(w_offset[2]));  break;
        case kSubtypeGyroScaleX:
          mem->Write(key_wx_scale,  PunToUint32(w_scale[0]));   break;
        case kSubtypeGyroScaleY:
          mem->Write(key_wy_scale,  PunToUint32(w_scale[1]));   break;
        case kSubtypeGyroScaleZ:
          mem->Write(key_wz_scale,  PunToUint32(w_scale[2]));   break;
        case kSubtypeGyroScaleSelect:
          mem->Write(key_w_fs_sel, w_fs_sel);                   break;

        case kSubtypeAccelOffsetX:
          mem->Write(key_ax_offset, PunToUint32(a_offset[0]));  break;
        case kSubtypeAccelOffsetY:
          mem->Write(key_ay_offset, PunToUint32(a_offset[1]));  break;
        case kSubtypeAccelOffsetZ:
          mem->Write(key_az_offset, PunToUint32(a_offset[2]));  break;
        case kSubtypeAccelScaleX:
          mem->Write(key_ax_scale,  PunToUint32(a_scale[0]));   break;
        case kSubtypeAccelScaleY:
          mem->Write(key_ay_scale,  PunToUint32(a_scale[1]));   break;
        case kSubtypeAccelScaleZ:
          mem->Write(key_az_scale,  PunToUint32(a_scale[2]));   break;
        case kSubtypeAccelScaleSelect:
          mem->Write(key_a_fs_sel, a_fs_sel);                   break;
      }
    } // end if save
  } // end if kTypeIMU60XX


  else if(type == kTypeRequestMessage) {
    MsgRequestMessage *data_in = (MsgRequestMessage*) &rx_data[1];
    uint8_t msg_type = data_in->msg_type;

    if     (msg_type == kTypeImuOffset) {
      MsgImuOffset tx_msg;
      tx_msg.ax = a_offset[0];
      tx_msg.ay = a_offset[1];
      tx_msg.az = a_offset[2];
      tx_msg.wx = w_offset[0];
      tx_msg.wy = w_offset[1];
      tx_msg.wz = w_offset[2];
      com.SendPacket(kTypeImuOffset, (uint8_t*)&tx_msg, sizeof(MsgImuOffset));
    }
    else if(msg_type == kTypeImuScale) {
      MsgImuScale tx_msg;
      tx_msg.ax = a_scale[0];
      tx_msg.ay = a_scale[1];
      tx_msg.az = a_scale[2];
      tx_msg.wx = w_scale[0];
      tx_msg.wy = w_scale[1];
      tx_msg.wz = w_scale[2];
      com.SendPacket(kTypeImuScale, (uint8_t*)&tx_msg, sizeof(MsgImuScale));
    }
    else if(msg_type == kTypeImuRaw) {
      SendRaw(com);
    }
    else if(msg_type == kTypeImuMetric) {
      SendMetric(com);
    }
  } // kTypeRequestMessage
  
  else if(type == kTypeImuOffset) {
    MsgImuOffset *msg = (MsgImuOffset*) &rx_data[1];
    a_offset[0] = msg->ax;
    a_offset[1] = msg->ay;
    a_offset[2] = msg->az;
    w_offset[0] = msg->wx;
    w_offset[1] = msg->wy;
    w_offset[2] = msg->wz;
    #ifdef USING_PERSISTENT_MEMORY
    WriteOffset();
    #endif
  } // kTypeImuOffset
 
  else if(type == kTypeImuScale) {
    MsgImuScale *msg = (MsgImuScale*) &rx_data[1];
    a_scale[0] = msg->ax;
    a_scale[1] = msg->ay;
    a_scale[2] = msg->az;
    w_scale[0] = msg->wx;
    w_scale[1] = msg->wy;
    w_scale[2] = msg->wz;
    #ifdef USING_PERSISTENT_MEMORY
    WriteScale();
    #endif
  } // kTypeImuScale
 
  else if(type == kTypeImuCommand) {
    MsgImuCommand *data_in = (MsgImuCommand*) &rx_data[1];
    uint8_t cmd = data_in->command;
    
    if      (cmd == kImuCommandDefaultOffset) {
      DefaultOffset();
    }
    else if (cmd == kImuCommandDefaultScale) {
      //DefaultScale();
      DefaultGyroSensitivity(1);
      DefaultAccelSensitivity(0);
    }
    else if (cmd == kImuCommandCalibrateGyroOffset) {
      CalibrateGyroOffset();
      //StartRead();                     // start normal measurements again
    }
    else if (cmd == kImuCommandWriteAll) {
      #ifdef USING_PERSISTENT_MEMORY
      WriteAll();
      #endif
    }
  } // kTypeImuCommand
}

/// Fill raw measurement message from the IMU
void Mpu60XXImu::FillMsgImuRaw(MsgImuRaw *msg) {
  msg->t = ToMicroseconds(time);
  msg->ax = a_raw[0];
  msg->ay = a_raw[1];
  msg->az = a_raw[2];
  msg->wx = w_raw[0];
  msg->wy = w_raw[1];
  msg->wz = w_raw[2];
}

/// Send raw measurements from the IMU
void Mpu60XXImu::SendRaw(CommunicationInterface& com) {
  MsgImuRaw tx_msg;
  FillMsgImuRaw(&tx_msg);
  com.SendPacket(kTypeImuRaw, (uint8_t*)&tx_msg, sizeof(MsgImuRaw));
}

/// Fill metric measurements from the IMU
void Mpu60XXImu::FillMsgImuMetric(MsgImuMetric *msg) {
  msg->t = ToMicroseconds(time);
  msg->ax = a[0];
  msg->ay = a[1];
  msg->az = a[2];
  msg->wx = w[0];
  msg->wy = w[1];
  msg->wz = w[2];
}

/// Send metric measurements from the IMU
void Mpu60XXImu::SendMetric(CommunicationInterface& com) {
  MsgImuMetric tx_msg;
  FillMsgImuMetric(&tx_msg);
  com.SendPacket(kTypeImuMetric, (uint8_t*)&tx_msg, sizeof(MsgImuMetric));
}








////////////////////////////////////////////////////////////////////////////////
// DEFAULT SETTINGS

// Set default IMU offset
void Mpu60XXImu::DefaultOffset() {
  int16_t default_a_offset[3] = {0, 0, 0};
  int16_t default_w_offset[3] = {0, 0, 0};
  memcpy(a_offset, default_a_offset, 3*sizeof(int16_t));
  memcpy(w_offset, default_w_offset, 3*sizeof(int16_t));
}

/// Set gyro sensitivity and load default scale (no write to imu)
/// FS_SEL value     {0,   1,    2,    3   }
/// full scale range {250, 500,  1000, 2000} deg/s
void Mpu60XXImu::DefaultGyroSensitivity(uint8_t fs_sel) {
  w_fs_sel = fs_sel;
  for(uint8_t i=0; i<3; i++) {
    w_scale[i] = (250.0f * PI / 180 / 32768) * (1<<w_fs_sel);
  }
}

/// Set accellerometer sensitivity and load default scale (no write to imu)
/// FS_SEL value     {0,  1,  2,  3 }
/// full scale range {2,  4,  8,  16} g
void Mpu60XXImu::DefaultAccelSensitivity(uint8_t fs_sel) {
  a_fs_sel = fs_sel;
  for(uint8_t i=0; i<3; i++) {
    a_scale[i] = (2.0f * 9.8f / 32768) * (1<<a_fs_sel);
  }
}








////////////////////////////////////////////////////////////////////////////////
/// Memory Accessing Operations

#ifdef USING_PERSISTENT_MEMORY
/// Write all settings to memory.
void Mpu60XXImu::WriteAll() {
  WriteOffset();
  WriteScale();
  WriteGyroSensitivity();
  WriteAccelSensitivity();
}
#endif

#ifdef USING_PERSISTENT_MEMORY
/// Read gyro sensitivity setting
void Mpu60XXImu::ReadGyroSensitivity() {
  int32_t value;
  mem->Read(key_w_fs_sel, (uint32_t*)&value);
    w_fs_sel = value;
}
#endif

#ifdef USING_PERSISTENT_MEMORY
/// Write gyro sensitivity setting
void Mpu60XXImu::WriteGyroSensitivity() {
  mem->Write(key_w_fs_sel, w_fs_sel);
}
#endif

#ifdef USING_PERSISTENT_MEMORY
/// Read accel sensitivity setting
void Mpu60XXImu::ReadAccelSensitivity() {
  int32_t value;
  mem->Read(key_a_fs_sel, (uint32_t*)&value);
    a_fs_sel = value;
}
#endif

#ifdef USING_PERSISTENT_MEMORY
/// Write accel sensitivity setting
void Mpu60XXImu::WriteAccelSensitivity() {
  mem->Write(key_a_fs_sel, a_fs_sel);
}
#endif

#ifdef USING_PERSISTENT_MEMORY
/// Read IMU offsets
void Mpu60XXImu::ReadOffset() {
  int32_t value;
  mem->Read(key_ax_offset, (uint32_t*)&value);
    a_offset[0] = value;
  mem->Read(key_ay_offset, (uint32_t*)&value);
    a_offset[1] = value;
  mem->Read(key_az_offset, (uint32_t*)&value);
    a_offset[2] = value;
  mem->Read(key_wx_offset, (uint32_t*)&value);
    w_offset[0] = value;
  mem->Read(key_wy_offset, (uint32_t*)&value);
    w_offset[1] = value;
  mem->Read(key_wz_offset, (uint32_t*)&value);
    w_offset[2] = value;
}
#endif

#ifdef USING_PERSISTENT_MEMORY
/// Write IMU offsets
void Mpu60XXImu::WriteOffset() {
  mem->Write(key_ax_offset, a_offset[0]);
  mem->Write(key_ay_offset, a_offset[1]);
  mem->Write(key_az_offset, a_offset[2]);
  mem->Write(key_wx_offset, w_offset[0]);
  mem->Write(key_wy_offset, w_offset[1]);
  mem->Write(key_wz_offset, w_offset[2]);
}
#endif

#ifdef USING_PERSISTENT_MEMORY
/// Read IMU scale from memory
void Mpu60XXImu::ReadScale() {
  mem->Read(key_ax_scale, (uint32_t*)&a_scale[0]);
  mem->Read(key_ay_scale, (uint32_t*)&a_scale[1]);
  mem->Read(key_az_scale, (uint32_t*)&a_scale[2]);
  mem->Read(key_wx_scale, (uint32_t*)&w_scale[0]);
  mem->Read(key_wy_scale, (uint32_t*)&w_scale[1]);
  mem->Read(key_wz_scale, (uint32_t*)&w_scale[2]);
}
#endif

#ifdef USING_PERSISTENT_MEMORY
/// Write IMU scale to memory
void Mpu60XXImu::WriteScale() {
  mem->Write(key_ax_scale, PunToUint32(a_scale[0]));
  mem->Write(key_ay_scale, PunToUint32(a_scale[1]));
  mem->Write(key_az_scale, PunToUint32(a_scale[2]));
  mem->Write(key_wx_scale, PunToUint32(w_scale[0]));
  mem->Write(key_wy_scale, PunToUint32(w_scale[1]));
  mem->Write(key_wz_scale, PunToUint32(w_scale[2]));
}
#endif








////////////////////////////////////////////////////////////////////////////////
// ATTACHED HARDWARE CONFIGURATION

/// Initialize object and configure attached hardware from defaults
void Mpu60XXImu::InitFromDefault(uint8_t gyro_fs, uint8_t accel_fs) {
  // initialize object from defaults
  DefaultOffset();
  DefaultGyroSensitivity(gyro_fs);
  DefaultAccelSensitivity(accel_fs);
  flip_z = 0;
  
  // configure attached hardware
  ConfigMpu60XX();
  ConfigGyroSensitivity();
  ConfigAccelSensitivity();
}

/// Configure Attached Hardware: R26, R107 registers
void Mpu60XXImu::ConfigMpu60XX() {

  while(!mBusPeriphReady(kImuAddress, IMU_MBUS)) {
    DelayMicroseconds(1000);
  }
  
  mBusWrite(kImuAddress, kMpuPwrMgmt1Register, kMpuPwrMgmt1Value, IMU_MBUS);
  DelayMicroseconds(1000);
  
  mBusWrite(kImuAddress, kMpuConfigRegister, kMpuConfigValue, IMU_MBUS);
  DelayMicroseconds(1000);
}

/// Configure attached hardware: GYRO_CONFIG register
void Mpu60XXImu::ConfigGyroSensitivity() {
  mBusWrite(kImuAddress, kMpuGyroConfigRegister, w_fs_sel<<3, IMU_MBUS);
  DelayMicroseconds(1000);
}

/// Configure attached hardware: ACCEL_CONFIG register
void Mpu60XXImu::ConfigAccelSensitivity() {
  mBusWrite(kImuAddress, kMpuAccelConfigRegister, a_fs_sel<<3, IMU_MBUS);
  DelayMicroseconds(1000);
}

#endif // USING_MPU_60XX_IMU