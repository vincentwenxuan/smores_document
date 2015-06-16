#define NDEBUG // for Eigen

// common library components
#include "common_message_types.h"
#include "persistent_memory.h"  // persistent memory
#include "usb_interface.h"      // usb interface
#include "mpu_60XX_imu.h"       // imu
#include "ukf_6dof.h"           // attitude estimator

// generic required includes (from template project)
#include "common_peripherals.h"
#include "mGeneral.h"
#include "stm32f37x.h"

int main(void)
{
  InitPeripherals();

  // instantiate components

  PersistentMemory mem;
  
  UsbInterface usb = UsbInterface();
  usb.Init();
  
  Mpu60XXImu imu(mem);

  Ukf6DOF est;

  mem.Freeze();
  
  // load component settings from memory and begin operation
  imu.InitFromMemory();
  imu.DefaultAccelSensitivity(1); // HACK FOR BAD IMUs (overwrite scale for off-datasheet fix)
  imu.StartRead();
  
  //////////////////////////////////////////////////////////////////////////////
  // Main Loop
  while(1) {
    
    // Get IMU Data and Start New Measurement
    while(!imu.FinishRead()) {};
    DelayMicroseconds(50); // this seems to be critical (?!?)
    imu.StartRead();
    
    // Update Estimator with Measurement
    mWhiteON;
    est.Update(imu.time, imu.w, imu.a);
    mWhiteOFF;
    
    // Streaming USB Output (don't overload it)
    //imu.SendRaw();
    //imu.SendMetric();
    //est.SendEstQuat();
    est.SendEstEuler(usb);
    
    // USB Input
    usb.GetBytes();
    uint8_t *rx_data;  // temporary pointer to received type+data bytes
    uint8_t rx_length; // number of received type+data bytes
    while(usb.PeekPacket(&rx_data, &rx_length)) {
      imu.ReadMsg(usb, rx_data, rx_length);
      est.ReadMsg(usb, rx_data, rx_length);
      mem.ReadMsg(usb, rx_data, rx_length);
      usb.DropPacket();
    } // while(Peek...

    usb.SendNow();
  } // while(1)
  
  return(0);
}