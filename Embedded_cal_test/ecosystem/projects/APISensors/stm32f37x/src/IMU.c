/**
 * Cedric Destin
 **/ 
/** Include your custom headers here
 * 
 **/
#include "IMU.h"
/** Initialize variables here
 * 	ret: Use to check successes/failures
 **/ 
void IMU_Config(uint8_t I2Cx, uint8_t accel_scale, uint8_t gyro_scale){
  uint8_t ret = mBusPeriphReady(MIMU, I2Cx);
  mRedON;
  float sec = 0;
  while(!ret /*&& sec < 60*/){
    uint8_t ret = mBusPeriphReady(MIMU, I2Cx);
    _delay_ms(100);
    mYellowTOGGLE;
  }
//   if(sec > 60 && ret == 0){
//    //ERROR 
//   }
//   if(accel_scale < 0 || gyro_scale < 0 || accel_scale > 3 
//     || gyro_scale > 3 ){
//   //ERROR  
//   }
  ret = mBusWrite(MIMU, 0x6B, 0x01, I2Cx);
  while(!ret){
    ret = mBusWrite(MIMU, 0x6B, 0x01, I2Cx);
  }
  ret = mBusWrite(MIMU,0x1C,accel_scale<<3, I2Cx);
  while(!ret){
    ret = mBusWrite(MIMU,0x1C,accel_scale<<3, I2Cx);
  }
  ret =  mBusWrite(MIMU,0x1B,gyro_scale<<3, I2Cx);
  while(!ret){
    ret =  mBusWrite(MIMU,0x1B,gyro_scale<<3, I2Cx);
  }
  //If connected writing shoud be easy
}
uint8_t mBusIMU_accel(uint16_t* raw_data, uint8_t I2Cx){
  int i;
  uint8_t buffer[6];
  for(i = 0; i < 6; i++){
    buffer[i] = mBusRead(MIMU, (0x40 - i), I2Cx);
  }
  for(i = 1; i < 6; i = i + 2){
    int sentinel = 0;
    raw_data[sentinel] = buffer[sentinel*2]*256 + buffer[i];
    sentinel = sentinel + 1;
  }
  return 1;
}
//TODO conversion for gyro
uint8_t mBusIMU_gyro(uint16_t* raw_data, uint8_t I2Cx){
  uint16_t i;
  uint8_t buffer[6];
  for(i = 0; i < 6; i++){
    buffer[i] = mBusRead(MIMU, (0x48 - i), I2Cx);
  }
  for(i = 0; i < 6; i = i + 2){
    int sentinel = 0;
    raw_data[sentinel] = buffer[sentinel * 2] * 256 + buffer[i];
  }
  return 1;
}