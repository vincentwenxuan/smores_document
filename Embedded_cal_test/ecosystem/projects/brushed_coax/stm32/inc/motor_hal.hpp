// A hardware abstraction layer for top and bottom motors, where the top motor does phase locked pulsing.

#ifndef MOTOR_HAL_H
#define MOTOR_HAL_H

#include "common_peripherals.h"
#include "project_message_types.h"

#include "persistent_memory.h"
#include "communication_interface.h"

#include <stdint.h>

class MotorHal {

  public:

    MotorHal(PersistentMemory &memory);

    ////////////////////////////////////////////////////////////////////////////
    // Setters and Getters
    void  set_top_cmd_volts(float volts);
    float get_top_cmd_volts();

    void  set_top_cmd_volts_pulse_amp(float volts);
    float get_top_cmd_volts_pulse_amp();

    void  set_top_cmd_pulse_phase(float radians);
    float get_top_cmd_pulse_phase();

    void  set_bottom_cmd_volts(float volts);
    float get_bottom_cmd_volts();

    float get_top_speed();
    float get_bottom_speed();

    float get_top_mech_angle();

    void  set_top_mech_zero(float radians);
    float get_top_mech_zero();

    ////////////////////////////////////////////////////////////////////////////
    // I2C Commands
    void SendToBottomMotor();
    void BottomMotorReadStart();
    void BottomMotorReadEnd();

    ////////////////////////////////////////////////////////////////////////////
    // Message Operations
    void ReadMsg(CommunicationInterface& com, uint8_t* rx_data, uint8_t rx_length);

    void SendMsgMotorHalParam(CommunicationInterface& com);
    void FillMsgMotorHalParam(MsgMotorHalParam* tx_msg);

    void SendMsgMotorHalReport(CommunicationInterface& com);
    void FillMsgMotorHalReport(MsgMotorHalReport* tx_msg);

    ////////////////////////////////////////////////////////////////////////////
    // Public Members
    float top_mech_zero;        // motor angle when '+' rotor points to x+ (rad)

  private:

    PersistentMemory* mem;
};
  
#endif // MOTOR_HAL_H