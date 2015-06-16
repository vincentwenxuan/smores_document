#include "pulsing_controller.h"
#include "system_clock.h"
#include "common_message_types.h"
#include "pulsing_controller_msgs.h"
#include <stdint.h>
#include "math_helper.h"
#include "arm_math.h"

#if defined(SYNC_TIMER_LOW) && defined(SYNC_TIMER_HIGH) && defined(ENCODER_QEP_TIMER) 

////////////////////////////////////////////////////////////////////////////////
// Static Commutation Callback
// one pointer is held to the most recently constructed PulsingController
// it is used to resolve the commutation callback of BrushlessController
static PulsingController* motor; 
float PulsingCallback(float mech_angle) {
  // signal phase = mech_angle + (-mech_zero - u_phase + advance_angle)
  return motor->get_voltage_supply_inverse()*motor->get_cmd_volts_pulse_amp()*arm_cos_f32(mech_angle + motor->get_cmd_pulse_phase());
}

////////////////////////////////////////////////////////////////////////////////
// Member Functions

PulsingController::PulsingController(PersistentMemory& memory):
BrushlessController::BrushlessController(memory),
mem(&memory),
cmd_pulse_phase(0),
cmd_volts_pulse_amp(0)
{
  // set object and function callback for commutation
  motor = this;
  CommuteCallback = PulsingCallback;
}

void PulsingController::ReadMsg(CommunicationInterface& com, uint8_t* rx_data, uint8_t rx_length) {
  BrushlessController::ReadMsg(com, rx_data, rx_length);

  uint8_t type = rx_data[0];

  if(type == kTypeRequestMessage) {
    MsgRequestMessage *data_in = (MsgRequestMessage*) &rx_data[1];
    uint8_t msg_type = data_in->msg_type;

    if     (msg_type == kTypePulsingControllerState) {
      SendState(com);
    }
  }
  else if (type == kTypePulsingController && rx_length >= 3) {

    uint8_t subtype = rx_data[1];
    uint8_t direction = rx_data[2];
    float ftemp = 0;

    if(direction == GET_CMD) {
      uint8_t tx_msg[6];
      tx_msg[0] = subtype;
      tx_msg[1] = 3;
      switch(subtype) {
        case kSubtypeCmdVoltsPulseAmp:
          ftemp = get_cmd_volts_pulse_amp();
          memcpy(&tx_msg[2], &ftemp, 4);
          com.SendPacket(kTypePulsingController, tx_msg, 6); break;
        case kSubtypeCmdPulsePhase:
          ftemp = get_cmd_pulse_phase();
          memcpy(&tx_msg[2], &ftemp, 4);
          com.SendPacket(kTypePulsingController, tx_msg, 6); break;
      }
    }
    else if(direction == SET_CMD) {
      switch(subtype) {
        case kSubtypeCmdVoltsPulseAmp:
          set_cmd_volts_pulse_amp(PunToFloat(&rx_data[3])); break;
        case kSubtypeCmdPulsePhase:
          set_cmd_pulse_phase(PunToFloat(&rx_data[3])); break;
      }
    }
    else if(direction == SAVE_CMD) {

    }
  }
}

/////////////////////////////////////////////////////////////////////////////

void PulsingController::set_cmd_pulse_phase(float phase) {
  cmd_pulse_phase = phase;
}

float PulsingController::get_cmd_pulse_phase() {
  return cmd_pulse_phase;
}

/////////////////////////////////////////////////////////////////////////////

void PulsingController::set_cmd_volts_pulse_amp(float amp) {
  cmd_volts_pulse_amp = amp;
}

float PulsingController::get_cmd_volts_pulse_amp() {
  return cmd_volts_pulse_amp;
}

/////////////////////////////////////////////////////////////////////////////

void PulsingController::SendState(CommunicationInterface& com) {
  MsgPulsingControllerState tx_msg;
  tx_msg.t = GetSeconds();
  tx_msg.cmd_volts = get_voltage_cmd();
  tx_msg.cmd_volts_pulse_amp = get_cmd_volts_pulse_amp();
  tx_msg.cmd_pulse_phase = get_cmd_pulse_phase();
  tx_msg.pwm = get_pwm();
  tx_msg.speed = get_speed();
  tx_msg.position = get_mechanical_angle();
  com.SendPacket(kTypePulsingControllerState, (uint8_t*)&tx_msg, sizeof(tx_msg));
}

#endif // #if defined(SYNC_TIMER_LOW) && defined(SYNC_TIMER_HIGH) && defined(ENCODER_QEP_TIMER)