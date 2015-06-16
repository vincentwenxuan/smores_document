// Bottom Module of Pulsing Coax Flyer

// LED Operation
//  GREEN:  ON when in i2c communication active ()
//  WHITE:  ON when packet communication active (dominates i2c)
//  AMBER:  TOGGLE on main loop heartbeat
//  RED:    UNUSED

#define NDEBUG
#define DISABLE_ZIGBEE

// common components
#include "common_message_types.h" // common messages
#include "persistent_memory.h"    // persistent memory
#include "usb_interface.h"        // usb interface
#ifndef DISABLE_ZIGBEE
  #include "zigbee_interface.h"     // radio
#endif
#include "brushless_controller.h" // motor controller
#include "battery_monitor.h"

// common libraries
#include "mBus.h"
#include "math_helper.h"
#include "project_message_types.h"
#include "common_peripherals.h"
#include "error_reporting.h"

// master-slave transfer parameters
#define AUX_MBUS 2
#define SLAVE_TO_MASTER_START 0xB400
#define SLAVE_TO_MASTER_BYTES 20
#define MASTER_TO_SLAVE_START 0xB500
#define MASTER_TO_SLAVE_BYTES 4
const uint8_t slave_addr = 0;



////////////////////////////////////////////////////////////////////////////////
// Entry Point

BrushlessController *bottom_motor_ptr;
BatteryMonitor *battery_ptr;

// heartbeat
struct Time heartbeat_timestamp;
const float heartbeat_timeout = 0.5;

uint8_t packet_alive = 0;
struct Time packet_timestamp;
const float packet_timeout = 0.5;

uint8_t mbus_alive = 0;
struct Time mbus_timestamp;
const float mbus_timeout = 0.1;

const uint16_t pwm_reg = 0x0000;

int main(void) {

  InitPeripherals();
  mGreenOFF; mWhiteOFF; mAmberOFF; mRedOFF;

  UsbInterface usb = UsbInterface();
  usb.Init();

  PersistentMemory mem;

  #ifndef DISABLE_ZIGBEE
    DelaySeconds(1);
    ZigbeeInterface zig = ZigbeeInterface();
    DelaySeconds(0.05);
    zig.Init(mem);
  #endif

  error_reporting_com1 = &usb;
  #ifndef DISABLE_ZIGBEE
  error_reporting_com2 = &zig;
  #endif

  BatteryMonitor battery(mem);
  battery_ptr = &battery;

  BrushlessController bottom_motor(mem);
  bottom_motor_ptr = &bottom_motor;

  mem.Freeze();

  bottom_motor.set_motor_data(7, 1380, 0, 0, 0, kSinusoidal);
  bottom_motor.set_permute_wires(1);

  PlayTimebase();

  heartbeat_timestamp = GetTime(); // start heartbeat
  mbus_timestamp      = GetTime(); // start communications timeout
  packet_timestamp    = GetTime(); // start packet timeout

  mBusSlaveInit(AUX_MBUS);
  mBusSlaveListen(AUX_MBUS);

  //////////////////////////////////////////////////////////////////////////////
  // Main Loop
  while(1) {

    // disable mbus_alive after timeout
    if(ToSeconds(SubtractTime(GetTime(), mbus_timestamp)) > mbus_timeout) {
    //if(SecondsElapsed(mbus_timestamp) > mbus_timeout) {
      mbus_timestamp = GetTime();
      mbus_alive = 0;
      mGreenOFF;
    }

    // disable packet_alive after timeout
    if(ToSeconds(SubtractTime(GetTime(), packet_timestamp)) > packet_timeout) {
    //if(SecondsElapsed(packet_timestamp) > packet_timeout) {
      packet_timestamp = GetTime();
      packet_alive = 0;
      mWhiteOFF;
    }

    // disable motor if all communications dead
    if(!(packet_alive || mbus_alive)) {
      bottom_motor.set_pwm_cmd(0);
    }

    ////////////////////////////////////////////////////////////////////////////
    // Packet Communication

    // USB Input
    uint8_t is_data = 0;  // 1 iff data received
    uint8_t *rx_data;     // temporary pointer to received type+data bytes
    uint8_t rx_length;    // number of received type+data bytes

    ////////////////////////////////////////////////////////////////////////////
    // USB Input
    is_data = 0;
    usb.GetBytes();
    while(usb.PeekPacket(&rx_data, &rx_length)) {
      #ifndef DISABLE_ZIGBEE
        zig.ReadMsg(          usb, rx_data, rx_length);
      #endif
      mem.ReadMsg(          usb, rx_data, rx_length);
      bottom_motor.ReadMsg( usb, rx_data, rx_length);
      battery.ReadMsg(      usb, rx_data, rx_length);
      usb.DropPacket();
      packet_timestamp = GetTime();
      packet_alive = 1;
      is_data = 1;
      mWhiteON;
    } // while(Peek...
    if(is_data) {
      usb.SendNow();
    }

    ////////////////////////////////////////////////////////////////////////////
    // Radio Input
    #ifndef DISABLE_ZIGBEE
    is_data = 0;
    zig.GetBytes();
    while(zig.PeekPacket(&rx_data, &rx_length)) {
      zig.ReadMsg(          zig, rx_data, rx_length);
      mem.ReadMsg(          zig, rx_data, rx_length);
      bottom_motor.ReadMsg( zig, rx_data, rx_length);
      battery.ReadMsg(      zig, rx_data, rx_length);
      zig.DropPacket();
      packet_timestamp = GetTime();
      packet_alive = 1;
      is_data = 1;
      mWhiteON;
    }
    if(is_data) {
      zig.SendNow();
    }
    #endif

    // main loop heartbeat
    if(ToSeconds(SubtractTime(GetTime(), heartbeat_timestamp)) > heartbeat_timeout) {
    //if(SecondsElapsed(heartbeat_timestamp) > heartbeat_timeout) {
      heartbeat_timestamp = GetTime();
      mAmberOFF;
    }
  } // while(1)
  
  return(0);
}

////////////////////////////////////////////////////////////////////////////////
// Callback Functions

// Vector control math
void SyncTimerCallback(void) {                // PWM rate
  if(bottom_motor_ptr)
    bottom_motor_ptr->BrushlessControllerCommute();
}

// Speed calculations
void timerCallback1(void) {                   // 1000 Hz
  if(bottom_motor_ptr) {
    if(battery_ptr) {
      float temp_volts = battery_ptr->GetVoltsFilt();
      if(temp_volts < 15.0f && temp_volts > 6.0f)
        bottom_motor_ptr->set_voltage_supply(temp_volts);
      else
        bottom_motor_ptr->set_voltage_supply(11.1f);
    }

    bottom_motor_ptr->BrushlessControllerControl();

    // emergency shutdown if main loop heartbeat stops
    if(ToSeconds(SubtractTime(GetTime(), heartbeat_timestamp)) > 2*heartbeat_timeout) {
    //if(SecondsElapsed(heartbeat_timestamp) > 2*heartbeat_timeout) {
      bottom_motor_ptr->set_pwm_cmd(0);
    }
  }
}

void SlaveRx_UserCallback(uint16_t reg_addr, uint32_t value) {
  switch(reg_addr) {
    case MASTER_TO_SLAVE_START + 0:
      mbus_alive = 1;
      mGreenON;
      if(!packet_alive) {
        bottom_motor_ptr->set_voltage_cmd(PunToFloat(value));
      }
      mbus_timestamp = GetTime();
      break;
  }
}

uint32_t SlaveTx_UserCallback(uint16_t reg_addr) {
  switch(reg_addr) {
    case SLAVE_TO_MASTER_START + 0:
      mAmberTOGGLE;
      return PunToUint32(bottom_motor_ptr->get_speed());
    case SLAVE_TO_MASTER_START + 1:
      return PunToUint32(battery_ptr->GetVoltsFilt());
    case SLAVE_TO_MASTER_START + 2:
      return PunToUint32(battery_ptr->GetAmpsFilt());
    case SLAVE_TO_MASTER_START + 3:
      return PunToUint32(battery_ptr->GetWattsFilt());
    case SLAVE_TO_MASTER_START + 4:
      return PunToUint32(battery_ptr->GetJoules());
  }
  return 0;
}