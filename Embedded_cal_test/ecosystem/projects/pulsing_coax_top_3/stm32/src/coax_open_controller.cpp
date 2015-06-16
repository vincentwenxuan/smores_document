#include "coax_open_controller.hpp"
#include "math_helper.h"

// persistent memory key values (based on component type and subtypes)
const uint16_t key_f0_yaw                     = (uint16_t)256*kTypeCoaxOpenController + kSubtypeCoaxOpenControllerF0Yaw;
const uint16_t key_f1_yaw                     = (uint16_t)256*kTypeCoaxOpenController + kSubtypeCoaxOpenControllerF1Yaw;
const uint16_t key_f2_yaw                     = (uint16_t)256*kTypeCoaxOpenController + kSubtypeCoaxOpenControllerF2Yaw;

CoaxOpenController::CoaxOpenController(PersistentMemory& memory):
top_mean(0),
bottom_mean(0),
top_pulse_amp(0),
top_pulse_phase(0),
f0_yaw(0.0f),
f1_yaw(0.0f),
f2_yaw(0.0f)
{
  // reserve memory
  mem = &memory;
  mem->AddKey(key_f0_yaw);
  mem->AddKey(key_f1_yaw);
  mem->AddKey(key_f2_yaw);

  // init from memory
  mem->Read(key_f0_yaw, (uint32_t*)&f0_yaw);
  mem->Read(key_f1_yaw, (uint32_t*)&f1_yaw);
  mem->Read(key_f2_yaw, (uint32_t*)&f2_yaw);
}

void CoaxOpenController::Update()
{

}

void CoaxOpenController::ReadMsg(CommunicationInterface& com, uint8_t* rx_data, uint8_t rx_length)
{
  uint8_t type = rx_data[0];

  if(type == kTypeCoaxOpenController && rx_length >= 3) {
    uint8_t subtype = rx_data[1];
    uint8_t direction = rx_data[2];
      
    if(direction == GET_CMD) {
      uint8_t tx_msg[6];
      tx_msg[0] = subtype;
      tx_msg[1] = RESPONSE_CMD;
      switch(subtype) {
        case kSubtypeCoaxOpenControllerF0Yaw:
          memcpy(&tx_msg[2], &f0_yaw, 4);
          com.SendPacket(kTypeCoaxOpenController, tx_msg, 6); break;
        case kSubtypeCoaxOpenControllerF1Yaw:
          memcpy(&tx_msg[2], &f1_yaw, 4);
          com.SendPacket(kTypeCoaxOpenController, tx_msg, 6); break;
        case kSubtypeCoaxOpenControllerF2Yaw:
          memcpy(&tx_msg[2], &f2_yaw, 4);
          com.SendPacket(kTypeCoaxOpenController, tx_msg, 6); break;
      }
    } // end if get

    else if(direction == SET_CMD) {
      switch(subtype) {
        case kSubtypeCoaxOpenControllerF0Yaw:
          f0_yaw =      PunToFloat(&rx_data[3]);  break;
        case kSubtypeCoaxOpenControllerF1Yaw:
          f1_yaw =      PunToFloat(&rx_data[3]);  break;
        case kSubtypeCoaxOpenControllerF2Yaw:
          f2_yaw =      PunToFloat(&rx_data[3]);  break;
      }
    } // end if set

    else if(direction == SAVE_CMD) {
      switch(subtype) {
        case kSubtypeCoaxOpenControllerF0Yaw:
          mem->Write(key_f0_yaw,  PunToUint32(f0_yaw)); break;
        case kSubtypeCoaxOpenControllerF1Yaw:
          mem->Write(key_f1_yaw,  PunToUint32(f1_yaw)); break;
        case kSubtypeCoaxOpenControllerF2Yaw:
          mem->Write(key_f2_yaw,  PunToUint32(f2_yaw)); break;
      }
    } // end if save
  } // end if kTypeQuatPD

  else if(type == kTypeOpenPilot && rx_length == sizeof(MsgOpenPilot)+1) {
    MsgOpenPilot *msg = (MsgOpenPilot*) &rx_data[1];
    float uz_f = f0_yaw + f1_yaw*msg->thrust + f2_yaw*msg->thrust*msg->thrust;
    top_mean        = msg->thrust - uz_f - msg->yaw_delta;
    top_pulse_amp   = msg->amplitude;
    top_pulse_phase = msg->phase;
    bottom_mean     = msg->thrust + uz_f + msg->yaw_delta;
  }

  else if(type == kTypeRequestMessage) {
    MsgRequestMessage *rx_req = (MsgRequestMessage*) &rx_data[1];
    if(rx_req->msg_type == kTypeCoaxOpenControllerParam) {
      SendMsgCoaxOpenControllerParam(com);
    }
  }
}

void CoaxOpenController::SendMsgCoaxOpenControllerParam(CommunicationInterface& com) {
  MsgCoaxOpenControllerParam tx_msg;
  FillMsgCoaxOpenControllerParam(&tx_msg);
  com.SendPacket(kTypeCoaxOpenControllerParam, (uint8_t*)&tx_msg, sizeof(MsgCoaxOpenControllerParam));
}

void CoaxOpenController::FillMsgCoaxOpenControllerParam(MsgCoaxOpenControllerParam* tx_msg) {
  tx_msg->f0_yaw  = f0_yaw;         // model feedforward polynomial (volts)
  tx_msg->f1_yaw  = f1_yaw;         // model feedforward polynomial (volts/volt)
  tx_msg->f2_yaw  = f2_yaw;         // model feedforward polynomial (volts/volts^2)
}