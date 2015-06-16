#include "common_message_types.h" // common messages
#include "mBus.h"
#include "zigbee.h"
#include "project_message_types.h"


// generic required includes (from template project)
#include "mGeneral.h"
#include "stm32f37x.h"
#include "common_peripherals.h"
#include "battery_monitor_it.h"
#include "usb_interface.h"

#include "MPU6050.h"
#include "Quadrotor.h"

/// Initialize variables here
#define QUAD_ID 2

#define MPU6050_ADDRESS (0b11010000)

int main(void)
{
  InitPeripherals();
//  if (InitBatteryMonitorHardware()) {
//    mBlueON;
//  }
  EnablePWM();
  
  mRedON;
  mGreenOFF;
  mBlueOFF;
  mYellowOFF;
  
  //  hardware devices
  UsbInterface usb;
  ZigbeeInterface zig;
  MPU6050 imu(MPU6050_ADDRESS);
  
  //  initialize IMU
  imu.setCommDelay(50);   //  50usec delay when trying reads
  imu.setCommRetries(3);  //  try at most 3 times before bailing
    
  //  instantiate the main object
  Quadrotor quad(QUAD_ID, &imu, &usb, &zig);
  quad.init();
  mRedOFF;

  //  enter main run loop, blocks forever
  quad.run();
  return 0;
}
