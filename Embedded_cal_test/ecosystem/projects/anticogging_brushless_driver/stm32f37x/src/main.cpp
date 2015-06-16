/** 
** This template will guide you in making a ST M4 project.  
** All locations with double comments require your attention (** or ///)
** Please perform the following tasks:
**
** Edit peripherals.h to enable the desired peripherals
** Edit peripherals.c to enable the desired peripherals
** Include custom headers
** Write your code in the specified areas
** Comment your code
** Delete this comment and put a description of your project and the used hardware
**/

#define NDEBUG // for Eigen

#include "stm32f37x.h"
#include "mGeneral.h"
#include "common_peripherals.h"

/// Include your custom headers here
#include "brushless_controller.h"
#include "usb_interface.h"
#include "zigbee_interface.h"
#include "persistent_memory.h"  // persistent memory
#include "anticogging_interface.h"
#include "system_clock.h"
#include "battery_monitor.h"

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
AnticoggingInterface cog;

// main function
int main(void)
{
  InitPeripherals();  // configure GPIO, USB, I2C, ect based on peripherals.h

  mRedON;
  mWhiteON;
  mGreenON;
  mAmberON;
  
  DelaySeconds(1.0);
  
  mRedOFF;
  
  UsbInterface usb = UsbInterface();
  mWhiteOFF;
  ZigbeeInterface zig = ZigbeeInterface();
  mGreenOFF;
  zig.Init();
  usb.Init();
  
  
  //zig.rx_channel = 86;
  //zig.tx_channel = 86;
  //zig.SetModeRx();
  
  motor = BrushlessController(mem);
  
  cog = AnticoggingInterface(motor);
  
  BatteryMonitor battery(mem);
  
  mem.Freeze();
  
  cog.DemoEnable();

  mAmberOFF;
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

    if(datasource != NO_DATA) // if there's data from somewhere
    {
      mWhiteON;
      com_ms_since_last = 0;
      //Send around the received packet
      usb.ReadMsg(*data_interface, received_data, received_length);
      zig.ReadMsg(*data_interface, received_data, received_length);
      mem.ReadMsg(*data_interface, received_data, received_length);
      motor.ReadMsg(*data_interface, received_data, received_length);
      cog.ReadMsg(*data_interface, received_data, received_length);
      battery.ReadMsg(*data_interface, received_data, received_length);
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
    motor.set_voltage_supply(BattItGetVolts());
    cog.set_voltage_supply(BattItGetVolts());
  }
  
  return(0);
}

// Vector control math
void SyncTimerCallback(void) //10khz
{
  motor.BrushlessControllerCommute();
}

// Speed calculations
void timerCallback1(void) //1000hz
{
  motor.BrushlessControllerControl();
  com_ms_since_last++;
}

void timerCallback2(void) //1/4hz
{
  cog.DemoToggle();
}