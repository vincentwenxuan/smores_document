/**
 * Cedric Destin
 **/ 

/** Include your custom headers here
 * 
 **/
 #include "SRF10.h"
void SRF10Config(uint8_t I2Cx){
  uint8_t ret = mBusPeriphReady(SRF10Adrs, 1);
   while(!ret){
     ret = mBusPeriphReady(SRF10Adrs, I2Cx);
   }
}
void SRF10Command(uint8_t I2Cx, uint8_t Unit, uint8_t Gain){
  /* Gain is the interval gain; input ranges
   * from 0 - 16 for ranges for values of 40 - 700
   */ 
  uint8_t ret = mBusWrite(SRF10Adrs, 0, Unit, I2Cx);
  while(!ret){
    ret = mBusWrite(SRF10Adrs, 0, Unit, I2Cx);
  }
  ret = mBusWrite(SRF10Adrs, 2, Gain, I2Cx);
  while(!ret){
    ret = mBusWrite(SRF10Adrs, 2, Gain, I2Cx);
  }
}
uint16_t SRF10(uint8_t I2Cx){
  uint16_t SRF10Range;
  SRF10Range = mBusRead(SRF10Adrs, 2, I2Cx) * 256;
  SRF10Range = SRF10Range + mBusRead(SRF10Adrs, 3, I2Cx);
  return SRF10Range;
}