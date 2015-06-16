// Flight controller for brushed pulsing coax vehicle.

// LED OPERATION
// GREEN: ON when under pilot message control
// BLUE:  ON after communication timeout
// RED:   ON after fatal error (check main loop lock or fault traps)

// TODO
// -- voltage sensing, voltage-based controls
// -- rotation estimation
// -- phased motor control

#define NDEBUG

// project components
#include "pulsing_coax_controller_quat.h"  // attitude controller
#include "uav_reporter.h"                   // manages message responses
#include "loop_monitor.h"
#include "state_machine.h"
#include "imu_raw_msg_logger.hpp"
#include "coax_open_controller.hpp"
#include "coax_open_attitude_controller.hpp"
#include "tim1_drive.h"
#include "motor_hal.hpp"

// common components
#include "common_message_types.h" // common messages
#include "persistent_memory.h"    // persistent memory
#include "usb_interface.h"        // usb interface
#include "zigbee_interface.h"     // radio
#include "mpu_60XX_imu.h"         // imu
#include "gyro_accel_drift_ukf_io.h"
#include "battery_monitor.h"

// common libraries
#include "project_message_types.h"
#include "common_peripherals.h"
#include "error_reporting.h"

#include "get_position.h"

// conditional compile
// #define STREAM_IMU_RAW

const float kMaxVolts = 3.7f; // volts
const float kInvMaxVolts = 1.0f/kMaxVolts; // 1/volts

MotorHal *motor_hal_ptr;
BatteryMonitor *battery_ptr;
LoopMonitor monitor;


////////////////////////////////////////////////////////////////////////////////
// Entry Point

int main(void) {

  InitPeripherals();
  mRedOFF; mGreenOFF; mBlueOFF;

  // startup blink
  const float kShortDelay = 0.1;
  const float kLongDelay = 0.3;
  mRedON; mGreenON; mBlueON;
  DelaySeconds(kShortDelay);
  mRedOFF; mGreenOFF; mBlueOFF;
  DelaySeconds(kLongDelay);
  mRedON; mGreenON; mBlueON;
  DelaySeconds(kShortDelay);
  mRedOFF; mGreenOFF; mBlueOFF;
  DelaySeconds(kLongDelay);
  mRedON; mGreenON; mBlueON;
  DelaySeconds(kShortDelay);
  mRedOFF; mGreenOFF; mBlueOFF;
  DelaySeconds(kLongDelay);

  UsbInterface usb = UsbInterface();
  usb.Init();

  PersistentMemory mem;

  DelaySeconds(1.0f);
  ZigbeeInterface zig = ZigbeeInterface();
  DelaySeconds(0.05f);
  zig.Init(mem);

  error_reporting_com1 = &usb;
  error_reporting_com2 = &zig;

  Mpu60XXImu imu(mem);
  GyroAccelDriftUkfIo est;

  BatteryMonitor battery(mem);
  battery_ptr = &battery;

  MotorHal motor_hal(mem);
  motor_hal_ptr = &motor_hal;

  // Get_pos addition:
  InitPositionSensor();
  tim1_set_position_callback_2(get_position);
  QuatPD pd(mem);
  PulsingCoaxControllerQuat uav(mem, pd, imu, est, motor_hal);

  CoaxOpenController open_controller(mem);
  CoaxOpenAttitudeController open_attitude_controller(mem, est);

  monitor = LoopMonitor(mem);
  UavReporter reporter(mem, imu, est, motor_hal, battery, uav, open_attitude_controller, pd, monitor);
  StateMachine state_machine;

  mem.Freeze();  // freeze memory to make writes possible

  imu.InitFromMemory();
  //imu.DefaultAccelSensitivity(1); // overwrite scale for off-datasheet fix
  imu.flip_z = 1;                   // flip z-axis direction for upside-down imu

  #ifdef STREAM_IMU_RAW
    imu_raw_msg_logger_init(imu);
  #endif

  monitor.InitFromMemory();

  PlayTimebase();
  imu.StartRead();
  DelayMilliseconds(10);

  tim1_init();
  tim1_set_supply_volts(3.7f);

  //////////////////////////////////////////////////////////////////////////////
  // Main Loop
  while(1) {
                                                                                monitor.Profile(0);
    ////////////////////////////////////////////////////////////////////////////
    // Get IMU Data and Start New Measurement
    while(!imu.FinishRead()) {};
    #ifdef STREAM_IMU_RAW
      imu_raw_msg_logger_push();
    #endif
    DelayMicroseconds(50); // this seems to be critical (?!?)
    imu.StartRead();
                                                                                monitor.Profile(1);
    ////////////////////////////////////////////////////////////////////////////
    // Update Estimator with Measurement
    est.Update(imu.time, imu.w, imu.a);

    ////////////////////////////////////////////////////////////////////////////
    // Control                                                                             

    // update control laws
    uav.Update();
    open_controller.Update();
    open_attitude_controller.Update();

    // map controllers to outputs based on state
    enum control_state state = state_machine.get_state();

    // in STOP state, send active kill messages to motors
    if(state == kStop) {
      mGreenOFF; mAmberON;
      motor_hal.set_top_cmd_volts(0);
      motor_hal.set_top_cmd_volts_pulse_amp(0);
      motor_hal.set_top_cmd_pulse_phase(0);
      motor_hal.set_bottom_cmd_volts(0);
    }

    // in STANDBY state, send no motor commands except on entry
    else if(state == kStandby) {
      mGreenOFF; mAmberOFF;
      if(state_machine.get_standby_needs_init()) {
        motor_hal.set_top_cmd_volts(0);
        motor_hal.set_top_cmd_volts_pulse_amp(0);
        motor_hal.set_top_cmd_pulse_phase(0);
        motor_hal.set_bottom_cmd_volts(0);
        state_machine.clear_standby_needs_init();
      }
    }

    // in QUAT state, send motor commands according to quat control law
    else if(state == kQuat) {
      mGreenON; mAmberOFF;
      motor_hal.set_top_cmd_volts(uav.top_mean);
      motor_hal.set_top_cmd_volts_pulse_amp(uav.top_pulse_amp);
      motor_hal.set_top_cmd_pulse_phase(uav.top_pulse_phase);
      motor_hal.set_bottom_cmd_volts(uav.bottom_mean);
    }

    // in OPEN state, send motor command according to open motor control commands
    else if(state == kOpen) {
      mGreenON; mAmberOFF;
      motor_hal.set_top_cmd_volts(open_controller.top_mean);
      motor_hal.set_top_cmd_volts_pulse_amp(open_controller.top_pulse_amp);
      motor_hal.set_top_cmd_pulse_phase(open_controller.top_pulse_phase);
      motor_hal.set_bottom_cmd_volts(open_controller.bottom_mean);
    }

    // in OPEN ATTITUDE state, send motor command according to open motor control commands
    else if(state == kOpenAttitude) {
      mGreenON; mAmberOFF;
      motor_hal.set_top_cmd_volts(open_attitude_controller.top_mean);
      motor_hal.set_top_cmd_volts_pulse_amp(open_attitude_controller.top_pulse_amp);
      motor_hal.set_top_cmd_pulse_phase(open_attitude_controller.top_pulse_phase);
      motor_hal.set_bottom_cmd_volts(open_attitude_controller.bottom_mean);
    }
                                                                                monitor.Profile(2);
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
      zig.ReadMsg(usb, rx_data, rx_length);
      imu.ReadMsg(usb, rx_data, rx_length);
      est.ReadMsg(usb, rx_data, rx_length);
      mem.ReadMsg(usb, rx_data, rx_length);
      uav.ReadMsg(usb, rx_data, rx_length);
      open_controller.ReadMsg(          usb, rx_data, rx_length);
      open_attitude_controller.ReadMsg( usb, rx_data, rx_length);
      pd.ReadMsg(       usb, rx_data, rx_length);
      monitor.ReadMsg(  usb, rx_data, rx_length);
      motor_hal.ReadMsg(usb, rx_data, rx_length);
      battery.ReadMsg(  usb, rx_data, rx_length);
      reporter.ReadMsg( usb, rx_data, rx_length);
      state_machine.ReadMsg(usb, rx_data, rx_length);
      usb.DropPacket();
      is_data = 1;
    } // while peek...
    if(is_data) {
      usb.SendNow();
    }

    ////////////////////////////////////////////////////////////////////////////
    // Radio Input
    is_data = 0;
    zig.GetBytes();
    while(zig.PeekPacket(&rx_data, &rx_length)) {
      #ifdef STREAM_IMU_RAW
        if(rx_data[0] == kTypeQuatPilot || rx_data[0] == kTypeQuatFullObsPilot || rx_data[0] == kTypeOpenPilot || rx_data[0] == kTypeOpenAttitudePilot) {
          imu_raw_msg_logger_send(zig);
        }
      #endif

      zig.ReadMsg(zig, rx_data, rx_length);
      imu.ReadMsg(zig, rx_data, rx_length);
      est.ReadMsg(zig, rx_data, rx_length);
      mem.ReadMsg(zig, rx_data, rx_length);
      uav.ReadMsg(zig, rx_data, rx_length);
      open_controller.ReadMsg(          zig, rx_data, rx_length);
      open_attitude_controller.ReadMsg( zig, rx_data, rx_length);
      pd.ReadMsg(       zig, rx_data, rx_length);
      monitor.ReadMsg(  zig, rx_data, rx_length);
      motor_hal.ReadMsg(zig, rx_data, rx_length);
      battery.ReadMsg(  zig, rx_data, rx_length);
      reporter.ReadMsg( zig, rx_data, rx_length);
      state_machine.ReadMsg(zig, rx_data, rx_length);
      zig.DropPacket();
      is_data = 1;
    } // while peek...
                                                                                monitor.Profile(3);
    if(is_data) {
      zig.SendNow();
    }
                                                                                monitor.Profile(4);
                                                                                monitor.Profile(5);
    ////////////////////////////////////////////////////////////////////////////    
    // Throttle main loop to main_freq
    monitor.EndMainLoop();
                                                                                monitor.Profile(6);
                                                                                //monitor.SendProfile(usb);
                                                                                //usb.SendNow();
  } // while(1)
  return(0);
}


////////////////////////////////////////////////////////////////////////////////
// Callback Functions

// Speed calculations
void timerCallback1(void) {               // 1000 Hz
  if(motor_hal_ptr) {
    if(!monitor.LoopIsAlive()) {
      motor_hal_ptr->set_top_cmd_volts(0);
      motor_hal_ptr->set_top_cmd_volts_pulse_amp(0);
      motor_hal_ptr->set_top_cmd_pulse_phase(0);
      motor_hal_ptr->set_bottom_cmd_volts(0);
    }
  }
}