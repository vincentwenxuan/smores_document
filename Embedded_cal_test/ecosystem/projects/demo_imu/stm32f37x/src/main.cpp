// common functional components
#include "persistent_memory.h"  // persistent memory
#include "usb_interface.h"      // usb interface
#include "mpu_60XX_imu.h"       // imu

// general ecosystem libraries
#include "common_message_types.h" // ecosystem-wide message definitions
#include "common_peripherals.h"   // hardware configuration

int main(void)
{
  InitPeripherals();

  // instantiate components
  PersistentMemory mem;
  
  UsbInterface usb = UsbInterface();
  usb.Init();
  
  Mpu60XXImu imu(mem);

  // freeze persistent memory table -- no more additions allowed
  mem.Freeze();
  
  // load component settings from memory and begin operation
  imu.InitFromMemory();
  imu.DefaultAccelSensitivity(1); // HACK FOR BAD IMUs (overwrite scale for off-datasheet fix)
  imu.StartRead();
  
  //////////////////////////////////////////////////////////////////////////////
  // Main Loop
  while(1) {

    mWhiteTOGGLE;
    
    // Get IMU Data and Start New Measurement
    while(!imu.FinishRead()) {};
    DelayMicroseconds(50); // this seems to be critical (?!?)
    imu.StartRead();
    
    // Streaming USB Output (don't overload it)
    //imu.SendRaw();
    imu.SendMetric(usb);
    
    // USB Input
    usb.GetBytes();
    uint8_t *rx_data;  // temporary pointer to received type+data bytes
    uint8_t rx_length; // number of received type+data bytes
    while(usb.PeekPacket(&rx_data, &rx_length)) {
      imu.ReadMsg(usb, rx_data, rx_length);
      mem.ReadMsg(usb, rx_data, rx_length);
      usb.DropPacket();
    } // while(Peek...

    usb.SendNow();
  } // while(1)
  
  return(0);
}