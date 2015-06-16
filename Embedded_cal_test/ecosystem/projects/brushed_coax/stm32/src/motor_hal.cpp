#include "motor_hal.hpp"
#include "common_peripherals.h"
#include "tim1_drive.h"
#include "get_position.h"
#include "math_helper.h"

// persistent memory key values (based on component type and subtypes)
const uint16_t key_top_mech_zero  = (uint16_t)256*kTypeMotorHal + kSubtypeMotorHalTopMechZero;

////////////////////////////////////////////////////////////////////////////////
// Constructor
MotorHal::MotorHal(PersistentMemory &memory):
top_mech_zero(0.0f)
{
  // reserve memory
  mem = &memory;
  mem->AddKey(key_top_mech_zero);

  // init from memory
  mem->Read(key_top_mech_zero, (uint32_t*)&top_mech_zero);
}


////////////////////////////////////////////////////////////////////////////////
// Setters and Getters
void  MotorHal::set_top_cmd_volts(float volts) {
  cmd_volts_mean_2 = volts;
}
float MotorHal::get_top_cmd_volts() {
  return cmd_volts_mean_2;
}

void  MotorHal::set_top_cmd_volts_pulse_amp(float volts) {
  cmd_volts_amplitude_2 = volts;
}
float MotorHal::get_top_cmd_volts_pulse_amp() {
  return cmd_volts_amplitude_2;
}

void  MotorHal::set_top_cmd_pulse_phase(float radians) {
  cmd_phase_2 = (radians - top_mech_zero);
}
float MotorHal::get_top_cmd_pulse_phase() {
  return (cmd_phase_2 + top_mech_zero);
}

void  MotorHal::set_bottom_cmd_volts(float volts) {
  cmd_volts_mean_1 = volts;
}
float MotorHal::get_bottom_cmd_volts() {
  return cmd_volts_mean_1;
}

float MotorHal::get_top_speed() {
  return get_velocity();
}
float MotorHal::get_bottom_speed() {
  return 0;
}

float MotorHal::get_top_mech_angle() {
  return 0;
}

void  MotorHal::set_top_mech_zero(float radians) {
  top_mech_zero = radians;
}
float MotorHal::get_top_mech_zero() {
  return top_mech_zero;
}


////////////////////////////////////////////////////////////////////////////////
// Message Operations
void MotorHal::ReadMsg(CommunicationInterface& com, uint8_t* rx_data, uint8_t rx_length)
{
  uint8_t type = rx_data[0];

  if(type == kTypeMotorHal && rx_length >= 3) {
    uint8_t subtype = rx_data[1];
    uint8_t direction = rx_data[2];
      
    if(direction == GET_CMD) {
      uint8_t tx_msg[6];
      tx_msg[0] = subtype;
      tx_msg[1] = RESPONSE_CMD;
      switch(subtype) {
        case kSubtypeMotorHalTopMechZero:
          memcpy(&tx_msg[2], &top_mech_zero, 4);
          com.SendPacket(kTypeMotorHal, tx_msg, 6); break;
      }
    } // end if get

    else if(direction == SET_CMD) {
      switch(subtype) {
        case kSubtypeMotorHalTopMechZero:
          top_mech_zero = PunToFloat(&rx_data[3]);  break;
        case kSubtypeMotorHalCalibrateTopMechZero:
          top_mech_zero = get_top_mech_angle(); break;
      }
    } // end if set

    else if(direction == SAVE_CMD) {
      switch(subtype) {
        case kSubtypeMotorHalTopMechZero:
          mem->Write(key_top_mech_zero,  PunToUint32(top_mech_zero)); break;
      }
    } // end if save
  } // end if kTypeQuatPD

  else if(type == kTypeRequestMessage) {
    MsgRequestMessage *rx_req = (MsgRequestMessage*) &rx_data[1];
    if(rx_req->msg_type == kTypeMotorHalParam) {
      SendMsgMotorHalParam(com);
    }
  }
}

void MotorHal::SendMsgMotorHalParam(CommunicationInterface& com) {
  MsgMotorHalParam tx_msg;
  FillMsgMotorHalParam(&tx_msg);
  com.SendPacket(kTypeMotorHalParam, (uint8_t*)&tx_msg, sizeof(MsgMotorHalParam));
}

void MotorHal::FillMsgMotorHalParam(MsgMotorHalParam* tx_msg) {
  tx_msg->top_mech_zero  = top_mech_zero;
}

void MotorHal::SendMsgMotorHalReport(CommunicationInterface& com) {
  MsgMotorHalReport tx_msg;
  FillMsgMotorHalReport(&tx_msg);
  com.SendPacket(kTypeMotorHalReport, (uint8_t*)&tx_msg, sizeof(MsgMotorHalReport));
}

void MotorHal::FillMsgMotorHalReport(MsgMotorHalReport* tx_msg) {
  tx_msg->t             = GetSeconds();
  tx_msg->top_mean      = get_top_cmd_volts();
  tx_msg->bottom_mean   = get_bottom_cmd_volts();
  tx_msg->u_amp         = get_top_cmd_volts_pulse_amp();
  tx_msg->u_phase       = get_top_cmd_pulse_phase();
  tx_msg->top_speed     = get_top_speed();
  tx_msg->bottom_speed  = get_bottom_speed();
}