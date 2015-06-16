#define NDEBUG // for Eigen

#include "stm32f37x.h"
#include "mGeneral.h"
#include "common_peripherals.h"

/// Include your custom headers here
#include "brushless_controller.h"
#include "usb_interface.h"
#include "zigbee_interface.h"
#include "persistent_memory.h"  // persistent memory
#include "system_clock.h"


/// Initialize functions here

/// Initialize variables here
typedef enum {NO_DATA=0, ZIGBEE_DATA, USB_DATA} datasource_t;
datasource_t datasource = NO_DATA;
CommunicationInterface* data_interface; 
uint32_t com_ms_timeout = 250;
volatile uint32_t com_ms_since_last = 0;
volatile uint8_t report_flag = 0;

BrushlessController motor;
PersistentMemory mem;

// main function
int main(void)
{
  InitPeripherals();  // configure GPIO, USB, I2C, ect based on peripherals.h

  mRedON;
  mWhiteON;
  mGreenON;
  mAmberON;
  
  DelayMilliseconds(1000);
  
  mRedOFF;
  mWhiteOFF;
  mGreenOFF;
  mAmberOFF;
  
  UsbInterface usb = UsbInterface();
  ZigbeeInterface zig = ZigbeeInterface();
  zig.Init();
  DelaySeconds(0.05f);
  zig.rx_channel = 102;
  zig.tx_channel = 102;
  zig.SetModeRx();
  usb.Init();
  
  motor = BrushlessController(mem);
  
  mem.Freeze();

  motor.set_motor_data(7, 1380, 0, 0, 0, kSinusoidal);
  motor.set_permute_wires(1);
  
  // begin operation
  PlayTimebase();

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
      data_interface = &zig;
    }
    else if(usb.PeekPacket(&received_data, &received_length)) 
    {
      datasource = USB_DATA;
      data_interface = &usb;
    }
    else 
      datasource = NO_DATA;
    //TODO:: this only does one packet per loop, should it be more or would that kill the main loop with big message???
    if(datasource != NO_DATA) // if there's data from somewhere
    {
      mWhiteON;
      com_ms_since_last = 0;
      //Send around the received packet
      usb.ReadMsg(*data_interface, received_data, received_length);
      zig.ReadMsg(*data_interface, received_data, received_length);
      mem.ReadMsg(*data_interface, received_data, received_length);
      motor.ReadMsg(*data_interface, received_data, received_length);
      mWhiteOFF;
      mAmberON;
      data_interface->DropPacket();
      data_interface->SendNow();
      mAmberOFF;
    } // if(datasource != NO_DATA
    if (com_ms_since_last > com_ms_timeout)
    {
      motor.set_pwm_cmd(0.0);
      mRedON;
    }
    else
    {
      mRedOFF;
      
    }
    if(report_flag == 1)
      {
        //printf("Speed: %f\n\r",motor.get_speed());
        //printf("PWM: %f\n\r",motor.get_pwm_duty());
        //printf("Pos: %f\n\r",motor.get_mechanical_angle());
        report_flag = 0;
      }
  }
  
  return(0);
}

// Vector control math
void SyncTimerCallback(void) //PWM rate
{
  // start 11.01us
  motor.BrushlessControllerCommute();
  // end 11.01us
}

// Speed calculations
void timerCallback1(void) //1000hz
{
  motor.BrushlessControllerControl();
  com_ms_since_last++;
}

// Speed reporting
void timerCallback2(void) //10hz
{
  report_flag = 1;
}