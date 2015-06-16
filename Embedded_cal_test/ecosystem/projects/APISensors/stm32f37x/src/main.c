// /**
//  * Cedric Destin
//  **/ 
// /**
//  * Headers: 
//  * 	stm32f37x
//  * 	mGeneral
//  * 	common_peripherals
//  * 	stdio
//  **/
// #include "stm32f37x.h"
// #include "mGeneral.h"
// #include "common_peripherals.h"
// #include "stdio.h"
// #include "IMU.h"
// #include "IR.h"
// #include "MB1010.h"
// /** Include your custom headers here
//  * 
//  **/
// /** Initialize variables here
//  * 	SRF10: Address of the SRF10
//  * 	ret: Use to check successes/failures
//  * 	GPIO_InitStructure
//  * 	TIM_ICInitStructure
//  * 	ADC_InitStructure
//  * 	USART_InitStructure
//  **/ 
// GPIO_InitTypeDef GPIO_InitStructure;
// TIM_ICInitTypeDef TIM_ICInitStructure;
// ADC_InitTypeDef ADC_InitStructure;
// USART_InitTypeDef USART_InitStructure;
// #define SRF10 0b11100000
// #define MIMU 0b11010010
// uint16_t accelxyz[2];
// float Len_Vol[1];
// float PW_AN[1];
// uint8_t ret = 0;
// /** Function Declarations
//  * 
//  **/ 
// /** Main
//  * 		
//  **/
// int main(void){
//   InitPeripherals();  // configure GPIO, USB, I2C, ect based on peripherals.h
//   /// Initialize code here
//   ret = mBusPeriphReady(SRF10,1);
//   // Set up IMU
//   //IMU_Config(1, 1, 1); // Not too sure about the accel_scale and gyro_scale
//   // Set up Sonar1 PW (MP1010)
//   // Set up IR
//   IRConfig();		// This is right quick as well
//   MB1010Config_PW(); // This is very quick
//   MB1010Config_AN(); // This is very quick too
//   while(1){
//     //ret = mBusIMU_accel(accelxyz, 1);
//     //IR_range(Len_Vol);
//     //MB1010Range(PW_AN);
//     mYellowTOGGLE;
//     mWaitms(10);
//     //printf("\rPW_AN %f\n", PW_AN[0]);
//     //printf("\rLen_Vol %f\n", Len_Vol[0]);
//   }
//   return 1;
// }
// void checker(uint8_t x){
//   if(x == 1){
//     while(1){
//       uint8_t ret = mBusPeriphReady(SRF10,1);
//       if(!ret){
// 	mGreenOFF;
// 	mRedON;
//       }else{
// 	mGreenON;
// 	mRedOFF;
//       }
//     }
//   }else{ // Get out of loop
//   }
// }
/**
 * Cedric Destin
 * I2C config for the SRF10
 * The SRF10, is a Ultrasonic range with I2C interface
 * The SRF10 needs 5V for Vcc and SDA, SCL
 * However the M4 only suplies 3.3V,
 * So I thought, that if I used an external Vc, suplying 5V,
 * I would be able to communicate t=with the SRF10...
 * Well not really... I was able to communicate with it
 * using Dr. Fienny's template, however... the code from the ppr
 * would always get stucked at the initialization phase. 
 *Although I initialized communication, only writing to the SRF10
 * would pass... Everytime I tried to read... I would get a 0
 * So I stopped working on it at night time... First thing in the morning
 * I got back to it... And I started from ground, just try to establish com
 * using Matt's ppr code which I found out why it failed to initialize
 * Again writing to it passed, Then the SRF10 stopped working...
 * I told Matt about it and I figured out that the line would operate on
 * 5V but the M4 only has 3.3V... So for now I want to initialize the SRF10
 * Not thinking too much of the hardware and using what I would do in
 * a perfect setting...
 * Ideally, this will:
 * Connect
 * Write to the command register to read in cm
 * read in cm
 * Send the data out...
 * So maybe I will use Jimmy's new packetizer...
 * Eventually I have to figure out the UART of the MB1010
 **/
/**
 * Headers: 
 * 	stm32f37x
 * 	mGeneral
 * 	common_peripherals
 * 	stdio
 **/
#include "stm32f37x.h"
#include "mGeneral.h"
#include "common_peripherals.h"
#include "stdio.h"
/** Include your custom headers here
 * 
 **/ 
#include "project_message_types.h"
#include "usb_interface.h"
/** Initialize variables here
 * 	SRF10: Address of the SRF10
 * 	ret: Use to check successes/failures
 **/ 
#define SRF10 0b11100000
uint8_t ret = 0;
uint16_t Range = 0;
uint8_t tx_buf[136];
uint8_t tx_len;
void checker(uint8_t x);

/** Main
 * 		
 **/
int main(void){
  InitPeripherals();  // configure GPIO, USB, I2C, ect based on peripherals.h
  /// Initialize code here
  InitUsbInterface();
  checker(1);
  ret = mBusPeriphReady(SRF10,1);
  while(!ret){				// Check if the SRF10 is ready
    ret = mBusPeriphReady(SRF10,1);
    _delay_ms(100);
    mRedTOGGLE;
  }
  ret = mBusWrite(SRF10, 0, 80, 1);
  /*
   * Write to the SRF10, to initialize the output
   * Reg: 0, Command register
   * data: 80, value to initialize cm output
   */
  while(!ret){				// Check if the writing was successful
    ret = mBusWrite(SRF10, 0, 80, 1);
  }
  ret = mBusWrite(SRF10, 2, 10, 1);
  /* 
   * Write to SRF10 to set the range
   * Reg: 2, range register
   * data 10: Analog gain = 200
   */
  while(!ret){
    ret = mBusWrite(SRF10, 2, 10, 1);
  }
  while(1){ 				// SRF10 is set, read data from it
    ret = mBusRead(SRF10, 2, 1) * 256;
    while(!ret){
      ret = mBusRead(SRF10, 2, 1) * 256;
      printf("High byte being read");
      _delay_ms(10);
    }
    printf("High byte read");
    Range =  ret + mBusRead(SRF10, 3, 1);
    while(mBusRead(SRF10, 3, 1)){
      Range =  ret + mBusRead(SRF10, 3, 1);
      printf("Low byte being read");
      _delay_ms(10);
    }
    printf("\rRange = %d\t", Range);
    _delay_ms(100);
    mYellowON;
    // Set up CDC
    uint8_t msg_type = 1;   
    //Packing data into struct
    MsgMyData data_out;
    data_out.uint16_var = Range;
    SendUsbPacket(msg_type, (uint8_t*)&data_out, sizeof(MsgMyData));
    SendUsbNow();
    printf("\n");
  }
}
void checker(uint8_t x){
  if(x == 1){
    while(1){
      uint8_t ret = mBusPeriphReady(SRF10,1);
      if(!ret){
	mGreenOFF;
	mRedON;
      }else{
	mGreenON;
	mRedOFF;
      }
    }
  }else{ // Get out of loop
  }
}
