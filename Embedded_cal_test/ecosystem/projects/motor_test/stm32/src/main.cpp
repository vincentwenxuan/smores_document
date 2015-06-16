// Motor Controller for Thrust Stand Testing

// LED OPERATION
// GREEN: ON when communications are active
// WHITE: TOGGLE on main loop
// AMBER: TOGGLE on message received
// RED:   ON after fatal error (check main loop lock or fault traps)

//project components
#include "polling_reporter.h"                   // manages message responses
#include "battery_monitor.h"

// common components
#include "common_message_types.h" // common messages
#include "persistent_memory.h"    // persistent memory
#include "usb_interface.h"        // usb interface
#include "zigbee_interface.h"     // radio
#include "pulsing_controller.h" // motor controller
#include "system_clock.h"

// common libraries
#include "arm_math.h"
#include "project_message_types.h"
#include "common_peripherals.h"


struct Time com_timestamp;
const float com_timeout = 5.0; // seconds

////////////////////////////////////////////////////////////////////////////////
// Entry Point

PulsingController *motor_ptr;

int main(void)
{
  InitPeripherals();
  mGreenOFF; mWhiteOFF; mAmberOFF; mRedOFF;
  mGreenON;

  UsbInterface usb = UsbInterface();
  usb.Init();

  PersistentMemory mem;

  ZigbeeInterface zig = ZigbeeInterface();
  zig.Init(mem);

  BatteryMonitor battery(mem);
  PulsingController motor(mem);
  motor_ptr = &motor;

  PollingReporter reporter(mem, motor, battery);

  mem.Freeze();  // freeze memory to make writes possible

  motor.set_motor_data(7, 1380, 0, 0, 0, kSinusoidal); // also motor lead 0.00195
  motor.set_permute_wires(1);

  PlayTimebase();

  com_timestamp.sec  = 0;
  com_timestamp.nsec = 0;

  //////////////////////////////////////////////////////////////////////////////
  // Main Loop
  while(1) {

    float temp_volts = battery.GetVoltsFilt();
    if(temp_volts < 20.0f && temp_volts > 1.0f)
      motor.set_voltage_supply(temp_volts);
    else
      motor.set_voltage_supply(11.1f);

    ////////////////////////////////////////////////////////////////////////////
    // Packet Communication
    uint8_t is_data;    // 1 iff data received
    uint8_t *rx_data;   // temporary pointer to received type+data bytes
    uint8_t rx_length;  // number of received type+data bytes

    ////////////////////////////////////////////////////////////////////////////
    // USB Input
    is_data = 0;
    usb.GetBytes();
    while(usb.PeekPacket(&rx_data, &rx_length)) {
      zig.ReadMsg(      usb, rx_data, rx_length);
      mem.ReadMsg(      usb, rx_data, rx_length);
      motor.ReadMsg(    usb, rx_data, rx_length);
      battery.ReadMsg(  usb, rx_data, rx_length);
      reporter.ReadMsg( usb, rx_data, rx_length);
      usb.DropPacket();
      is_data = 1;
    } // while peek...
    if(is_data) {
      mGreenON;
      mAmberTOGGLE;
      com_timestamp = GetTime();
    }
    reporter.Stream(usb);
    usb.SendNow();

    ////////////////////////////////////////////////////////////////////////////
    // Radio Input
    is_data = 0;
    zig.GetBytes();
    while(zig.PeekPacket(&rx_data, &rx_length)) {
      zig.ReadMsg(      zig, rx_data, rx_length);
      mem.ReadMsg(      zig, rx_data, rx_length);
      motor.ReadMsg(    zig, rx_data, rx_length);
      battery.ReadMsg(  zig, rx_data, rx_length);
      reporter.ReadMsg( zig, rx_data, rx_length);
      zig.DropPacket();
      is_data = 1;
    } // while peek...
    if(is_data) {
      mGreenON;
      mAmberTOGGLE;
      com_timestamp = GetTime();
      zig.SendNow();
    }

    ////////////////////////////////////////////////////////////////////////////
    // Control                                                                             

    //if(MicrosecondsElapsed(com_timestamp) > com_timeout) {
    if(ToSeconds(SubtractTime(GetTime(), com_timestamp)) > com_timeout) {
      motor.set_pwm_cmd(0);
      motor.set_cmd_pwm_pulse_amp(0);
      motor.set_cmd_pulse_phase(0);
      mGreenOFF;
    }

    mWhiteTOGGLE;
  } // while(1)
  return(0);
}


////////////////////////////////////////////////////////////////////////////////
// Callback Functions

// Vector control math
void SyncTimerCallback(void) // PWM rate
{
  if(motor_ptr)
    motor_ptr->BrushlessControllerCommute();
}

// Speed calculations
void timerCallback1(void) // 1000hz
{
  if(motor_ptr)
    motor_ptr->BrushlessControllerControl();
}