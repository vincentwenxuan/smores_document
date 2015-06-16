#include "stm32f37x.h"
#include "mGeneral.h"
#include "common_peripherals.h"

/// Include your custom headers here
#include "brushless_controller.h"
#include "usb_interface.h"
#include "zigbee_interface.h"
#include "mpu_60XX_imu.h"       // imu
#include "ukf_6dof.h"           // attitude estimator
#include "persistent_memory.h"  // persistent memory
#include "system_clock.h"


/// Initialize functions here

/// Initialize variables here
typedef enum {NO_DATA=0, ZIGBEE_DATA, USB_DATA} datasource_t;
datasource_t datasource = NO_DATA;
uint32_t com_ms_timeout = 250;
volatile uint32_t com_ms_since_last = 0;

const uint8_t kImuCommandDefaultOffset = 0;
const uint8_t kImuCommandDefaultScale = 1;
const uint8_t kImuCommandCalibrateGyroOffset = 2;

BrushlessController motor;

// main function
int main(void)
{
  InitPeripherals();  // configure GPIO, USB, I2C, ect based on peripherals.h
  
  PersistentMemory mem;
  
  // initialize Zigbee interface object
  ZigbeeInterface zig;
  zig.Init();
  
  UsbInterface usb;
  usb.Init();
  
  Mpu60XXImu imu(mem);
  motor = BrushlessController(mem);
  
  Vector3f w; w << 0.0f, 0.0f, 0.0f;            // initial angular rate
  Quaternionf q = Quaternionf::Identity();      // initial quaternion
  Vector6f temp;
  temp << 1e-2f, 1e-2f, 1e-2f, 1e-2f, 1e-2f, 1e-2f;
  Matrix6f P = temp.asDiagonal();               // initial estimate covariance
  temp << 1e-6f, 1e-6f, 1e-6f, 1e-2f, 1e-2f, 1e-2f;
  Matrix6f Q = temp.asDiagonal();               // model process covariance
  temp << 1e1f, 1e1f, 1e1f, 1e-2f, 1e-2f, 1e-2f;
  Matrix6f R = temp.asDiagonal();               // model measurement covariance
  Ukf6DOF est(w, q, P, Q, R);
  
  mem.Freeze();
  
  // load settings from memory and 
  imu.InitFromMemory();
  imu.DefaultAccelSensitivity(1); // HACK FOR BAD IMUs (overwrite scale for off-datasheet fix)
  imu.StartRead();
  
  mRedON;
  mWhiteON;
  //mGreenON;
  mAmberON;
  
  DelaySeconds(1.0f);
  
  mRedOFF;
  mWhiteOFF;
  //mGreenOFF;
  mAmberOFF;
  
  // begin operation
  PlayTimebase();
  imu.StartRead();
  
  // main loop
  while(1)
  {
    /// CHECK FOR DATA (Zigbee and USB)
    zig.GetBytes();
    usb.GetBytes(); // poll for new USB bytes
    // if packets are available
    uint8_t *received_data;  // temporary pointer to received type+data bytes
    uint8_t received_length; // number of of received type+data bytes
    if(zig.PeekPacket(&received_data, &received_length))
    {
      datasource = ZIGBEE_DATA;
    }
    else if(usb.PeekPacket(&received_data, &received_length)) 
    {
      datasource = USB_DATA;
    }
    else 
      datasource = NO_DATA;
    
    //TODO:: this only does one packet per loop, should it be more or would that kill the main loop with big message???
    if(datasource != NO_DATA) // if there's data from somewhere
    {
      mWhiteON;
      com_ms_since_last = 0;
      //Send around the received packet
      imu.ReadMsg(usb, received_data, received_length);
      est.ReadMsg(usb, received_data, received_length);
      mem.ReadMsg(usb, received_data, received_length);
      motor.ReadMsg(usb, received_data, received_length);
      mWhiteOFF;
      mAmberON;
      if(datasource == ZIGBEE_DATA)
      {
        zig.DropPacket(); // now that the data is safe, drop the packet from the input buffer
        // send any replies
        zig.SendNow(); 
      }
      else if(datasource == USB_DATA)
      {
        usb.DropPacket(); // now that the data is safe, drop the packet from the input buffer
        // send any replies
        usb.SendNow();
      }
      mAmberOFF;

    } // if(datasource != NO_DATA
    if (com_ms_since_last > com_ms_timeout)
    {
      motor.set_pwm_cmd(0.0);
      mRedON;
    }
    else
      mRedOFF;
    /// CHECK FOR IMU DATA
    if(imu.FinishRead()) { // if IMU data is ready
      //mGreenTOGGLE;
      DelayMicroseconds(50); // this seems to be critical (?!?)
      imu.StartRead();
      est.Update(imu.time, imu.a, imu.w);
    }
  }
  
  return(0);
}

// Vector control math
void SyncTimerCallback(void) // PWM rate
{
  mGreenON;
  motor.BrushlessControllerCommute();
  mGreenOFF;
}

// Speed calculations
void timerCallback1(void) //1000hz
{
  motor.BrushlessControllerControl();
  com_ms_since_last++;
}
