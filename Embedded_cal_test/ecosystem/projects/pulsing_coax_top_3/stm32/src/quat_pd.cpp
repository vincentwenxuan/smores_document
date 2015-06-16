#include "project_message_types.h"
#include "communication_interface.h"
#include "persistent_memory.h"
#include "math_helper.h"
#include "quat_control_math.hpp"
#include "quat_pd.h"

// persistent memory key values (based on component type and subtypes)
const uint16_t key_f0_yaw                     = (uint16_t)256*kTypeQuatPD + kSubtypeF0Yaw;
const uint16_t key_f1_yaw                     = (uint16_t)256*kTypeQuatPD + kSubtypeF1Yaw;
const uint16_t key_f2_yaw                     = (uint16_t)256*kTypeQuatPD + kSubtypeF2Yaw;
const uint16_t key_p_yaw                      = (uint16_t)256*kTypeQuatPD + kSubtypePYaw;
const uint16_t key_d_yaw                      = (uint16_t)256*kTypeQuatPD + kSubtypeDYaw;
const uint16_t key_p_roll                     = (uint16_t)256*kTypeQuatPD + kSubtypePRoll;
const uint16_t key_d_roll                     = (uint16_t)256*kTypeQuatPD + kSubtypeDRoll;
const uint16_t key_wxy_f0                     = (uint16_t)256*kTypeQuatPD + kSubtypeWxyF0;
const uint16_t key_wz_f0                      = (uint16_t)256*kTypeQuatPD + kSubtypeWzF0;
const uint16_t key_g                          = (uint16_t)256*kTypeQuatPD + kSubtypeG;

// Memory Constructor: Follow with InitFromMemory() after Freeze()
QuatPD::QuatPD(PersistentMemory& memory):
  // flash parameters, default values
  f0_yaw(0.0f),
  f1_yaw(0.0f),
  f2_yaw(0.0f),
  wxy_f0(30.0f),
  wz_f0(20.0f),
  g(0.0f)
{
  // reserve memory
  mem = &memory;
  mem->AddKey(key_f0_yaw);
  mem->AddKey(key_f1_yaw);
  mem->AddKey(key_f2_yaw);
  mem->AddKey(key_p_yaw);
  mem->AddKey(key_d_yaw);
  mem->AddKey(key_p_roll);
  mem->AddKey(key_d_roll);
  mem->AddKey(key_wxy_f0);
  mem->AddKey(key_wz_f0);
  mem->AddKey(key_g);

  // default gains
  controller.kq << 20.0, 20.0, 1.4;
  controller.kw << 0.5, 0.5, 0.3;

  // init from memory
  mem->Read(key_f0_yaw, (uint32_t*)&f0_yaw);
  mem->Read(key_f1_yaw, (uint32_t*)&f1_yaw);
  mem->Read(key_f2_yaw, (uint32_t*)&f2_yaw);
  
  mem->Read(key_p_yaw, (uint32_t*)&controller.kq(2));
  mem->Read(key_d_yaw, (uint32_t*)&controller.kw(2));
  
  mem->Read(key_p_roll, (uint32_t*)&controller.kq(1));
  mem->Read(key_d_roll, (uint32_t*)&controller.kw(1));

  mem->Read(key_p_roll, (uint32_t*)&controller.kq(0));
  mem->Read(key_d_roll, (uint32_t*)&controller.kw(0));

  mem->Read(key_wxy_f0, (uint32_t*)&wxy_f0);
  BiquadLpfDynamicCInit(&wx_filt,  wxy_f0);
  BiquadLpfDynamicCInit(&wy_filt,  wxy_f0);
  mem->Read(key_wz_f0, (uint32_t*)&wz_f0);
  BiquadLpfDynamicCInit(&wz_filt,  wz_f0);

  mem->Read(key_g, (uint32_t*)&g);

  // init intermediate values
  w_filt[0] = 0; w_filt[1] = 0; w_filt[2] = 0;
}


// Update control law with estimates
void QuatPD::Calculate(float dt, float thrust, const QuatState& des, const QuatState& est, float omega, OrthoControl& u) {

  w_filt[0] = BiquadLpfDynamicCUpdate(&wx_filt,  est.w(0), dt);
  w_filt[1] = BiquadLpfDynamicCUpdate(&wy_filt,  est.w(1), dt);
  w_filt[2] = BiquadLpfDynamicCUpdate(&wz_filt,  est.w(2), dt);

  u.thrust = thrust;

  // evaluate quaternion control law
  Eigen::Vector3f w; w << w_filt[0], w_filt[1], w_filt[2];
  controller.Update(est.q, w, des.q, des.w);
  u.roll  = controller.u(0);
  u.pitch = controller.u(1);
  u.yaw   = controller.u(2);

  // evaluate inversion dynamics
  ux_g = -g * omega * w(1);
  uy_g =  g * omega * w(0);

  // augment attitude with inversion command
  u.roll  += ux_g;
  u.pitch += uy_g;

  // augment yaw with feedforward on thrust command
  uz_f = f0_yaw + f1_yaw*thrust + f2_yaw*thrust*thrust;
  u.yaw += uz_f;
}


// Read Incomming Messages
void QuatPD::ReadMsg(CommunicationInterface& com, uint8_t* rx_data, uint8_t rx_length) {

  uint8_t type = rx_data[0];

  if(type == kTypeQuatPD && rx_length >= 3) {
    uint8_t subtype = rx_data[1];
    uint8_t direction = rx_data[2];
      
    if(direction == GET_CMD) {
      uint8_t tx_msg[6];
      tx_msg[0] = subtype;
      tx_msg[1] = RESPONSE_CMD;
      switch(subtype) {
        case kSubtypeF0Yaw:
          memcpy(&tx_msg[2], &f0_yaw, 4);
          com.SendPacket(kTypeQuatPD, tx_msg, 6); break;
        case kSubtypeF1Yaw:
          memcpy(&tx_msg[2], &f1_yaw, 4);
          com.SendPacket(kTypeQuatPD, tx_msg, 6); break;
        case kSubtypeF2Yaw:
          memcpy(&tx_msg[2], &f2_yaw, 4);
          com.SendPacket(kTypeQuatPD, tx_msg, 6); break;
        case kSubtypePYaw:
          memcpy(&tx_msg[2], &controller.kq(2), 4);
          com.SendPacket(kTypeQuatPD, tx_msg, 6); break;
        case kSubtypeDYaw:
          memcpy(&tx_msg[2], &controller.kw(2), 4);
          com.SendPacket(kTypeQuatPD, tx_msg, 6); break;
        case kSubtypePRoll:
          memcpy(&tx_msg[2], &controller.kq(0), 4);
          com.SendPacket(kTypeQuatPD, tx_msg, 6); break;
        case kSubtypeDRoll:
          memcpy(&tx_msg[2], &controller.kw(0), 4);
          com.SendPacket(kTypeQuatPD, tx_msg, 6); break;
        case kSubtypeWxyF0:
          memcpy(&tx_msg[2], &wxy_f0, 4);
          com.SendPacket(kTypeQuatPD, tx_msg, 6); break;
        case kSubtypeWzF0:
          memcpy(&tx_msg[2], &wz_f0, 4);
          com.SendPacket(kTypeQuatPD, tx_msg, 6); break;
        case kSubtypeG:
          memcpy(&tx_msg[2], &g, 4);
          com.SendPacket(kTypeQuatPD, tx_msg, 6); break;
      }
    } // end if get

    else if(direction == SET_CMD) {
      switch(subtype) {
        case kSubtypeF0Yaw:
          f0_yaw =      PunToFloat(&rx_data[3]);  break;
        case kSubtypeF1Yaw:
          f1_yaw =      PunToFloat(&rx_data[3]);  break;
        case kSubtypeF2Yaw:
          f2_yaw =      PunToFloat(&rx_data[3]);  break;
        case kSubtypePYaw:
          controller.kq(2) = PunToFloat(&rx_data[3]); break;
        case kSubtypeDYaw:
          controller.kw(2) = PunToFloat(&rx_data[3]); break;
        case kSubtypePRoll:
          controller.kq(0) = PunToFloat(&rx_data[3]);
          controller.kq(1) = controller.kq(0);        break;
        case kSubtypeDRoll:
          controller.kw(0) = PunToFloat(&rx_data[3]);  
          controller.kw(1) = controller.kw(0);        break;
        case kSubtypeWxyF0:
          wxy_f0 =      PunToFloat(&rx_data[3]);  break;
        case kSubtypeWzF0:
          wz_f0 =       PunToFloat(&rx_data[3]);  break;
        case kSubtypeG:
          g =           PunToFloat(&rx_data[3]);  break;
      }
    } // end if set

    else if(direction == SAVE_CMD) {
      switch(subtype) {
        case kSubtypeF0Yaw:
          mem->Write(key_f0_yaw,  PunToUint32(f0_yaw)); break;
        case kSubtypeF1Yaw:
          mem->Write(key_f1_yaw,  PunToUint32(f1_yaw)); break;
        case kSubtypeF2Yaw:
          mem->Write(key_f2_yaw,  PunToUint32(f2_yaw)); break;
        case kSubtypePYaw:
          mem->Write(key_p_yaw,   PunToUint32(controller.kq(2)));  break;
        case kSubtypeDYaw:
          mem->Write(key_d_yaw,   PunToUint32(controller.kw(2)));  break;
        case kSubtypePRoll:
          mem->Write(key_p_roll,  PunToUint32(controller.kq(0))); break;
        case kSubtypeDRoll:
          mem->Write(key_d_roll,  PunToUint32(controller.kw(0))); break;
        case kSubtypeWxyF0:
          mem->Write(key_wxy_f0,  PunToUint32(wxy_f0)); break;
        case kSubtypeWzF0:
          mem->Write(key_wz_f0,   PunToUint32(wz_f0));  break;
        case kSubtypeG:
          mem->Write(key_g,       PunToUint32(g));  break;
      }
    } // end if save
  } // end if kTypeQuatPD

  else if(type == kTypeRequestMessage) {
    MsgRequestMessage *rx_req = (MsgRequestMessage*) &rx_data[1];
    if(rx_req->msg_type == kTypeQuatPDParam) {
      SendMsgQuatPDParam(com);
    }
  }
}


void QuatPD::SendMsgQuatPDParam(CommunicationInterface& com) {
  MsgQuatPDParam tx_msg;
  FillMsgQuatPDParam(&tx_msg);
  com.SendPacket(kTypeQuatPDParam, (uint8_t*)&tx_msg, sizeof(MsgQuatPDParam));
}


void QuatPD::FillMsgQuatPDParam(MsgQuatPDParam* tx_msg) {
  tx_msg->f0_yaw  = f0_yaw;         // model feedforward polynomial (volts)
  tx_msg->f1_yaw  = f1_yaw;         // model feedforward polynomial (volts/volt)
  tx_msg->f2_yaw  = f2_yaw;         // model feedforward polynomial (volts/volts^2)
  tx_msg->p_yaw   = controller.kq(2);   // proportional gain  (volts/rad)
  tx_msg->d_yaw   = controller.kw(2);   // deriative gain     (volts/(rad/s))
  tx_msg->p_roll  = controller.kq(0);  // proportional gain  (volts/rad)
  tx_msg->d_roll  = controller.kw(0);  // derivative gain    (volts/(rad/s))
  tx_msg->wxy_f0  = wxy_f0;
  tx_msg->wz_f0   = wz_f0;
  tx_msg->g       = g;
}