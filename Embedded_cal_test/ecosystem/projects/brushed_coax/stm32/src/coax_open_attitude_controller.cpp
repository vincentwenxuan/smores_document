#include "coax_open_attitude_controller.hpp"
#include "math_helper.h"

// persistent memory key values (based on component type and subtypes)
const uint16_t key_f0_yaw       = (uint16_t)256*kTypeCoaxOpenAttitudeController + kSubtypeCoaxOpenAttitudeControllerF0Yaw;
const uint16_t key_f1_yaw       = (uint16_t)256*kTypeCoaxOpenAttitudeController + kSubtypeCoaxOpenAttitudeControllerF1Yaw;
const uint16_t key_f2_yaw       = (uint16_t)256*kTypeCoaxOpenAttitudeController + kSubtypeCoaxOpenAttitudeControllerF2Yaw;
const uint16_t key_p_yaw        = (uint16_t)256*kTypeCoaxOpenAttitudeController + kSubtypeCoaxOpenAttitudeControllerPYaw;
const uint16_t key_d_yaw        = (uint16_t)256*kTypeCoaxOpenAttitudeController + kSubtypeCoaxOpenAttitudeControllerDYaw;

const uint16_t key_wz_f0        = (uint16_t)256*kTypeCoaxOpenAttitudeController + kSubtypeCoaxOpenAttitudeControllerWzF0;

const uint16_t key_pulse_sat    = (uint16_t)256*kTypeCoaxOpenAttitudeController + kSubtypeCoaxOpenAttitudeControllerPulseSat;
const uint16_t key_yaw_sat      = (uint16_t)256*kTypeCoaxOpenAttitudeController + kSubtypeCoaxOpenAttitudeControllerYawSat;
const uint16_t key_thrust_sat   = (uint16_t)256*kTypeCoaxOpenAttitudeController + kSubtypeCoaxOpenAttitudeControllerThrustSat;

const uint16_t key_advance_angle    = (uint16_t)256*kTypeCoaxOpenAttitudeController + kSubtypeCoaxOpenAttitudeControllerAdvanceAngle;

CoaxOpenAttitudeController::CoaxOpenAttitudeController( PersistentMemory& memory,
                                                        GyroAccelDriftUkfIo &estimator):
top_mean(0.0f),
bottom_mean(0.0f),
top_pulse_amp(0.0f),
top_pulse_phase(0.0f),

f0_yaw(0.0f),
f1_yaw(0.0f),
f2_yaw(0.0f),
wz_f0(20.0f),
pulse_sat(4.0f),
yaw_sat(1.5f),
thrust_sat(5.6f),
advance_angle(0.0f),

pilot_thrust(0.0f),
pilot_pulse_phase(0.0f),
pilot_yaw(0.0f)
{
  mem = &memory;
  ukf = &estimator;

  // request persistent memory service
  mem->AddKey(key_f0_yaw);
  mem->AddKey(key_f1_yaw);
  mem->AddKey(key_f2_yaw);
  mem->AddKey(key_p_yaw);
  mem->AddKey(key_d_yaw);
  mem->AddKey(key_wz_f0);
  mem->AddKey(key_pulse_sat);
  mem->AddKey(key_yaw_sat);
  mem->AddKey(key_thrust_sat);
  mem->AddKey(key_advance_angle);

  // default gains
  controller.kq << 20.0, 20.0, 1.4;
  controller.kw << 0.5, 0.5, 0.3;

  // init from memory
  mem->Read(key_f0_yaw, (uint32_t*)&f0_yaw);
  mem->Read(key_f1_yaw, (uint32_t*)&f1_yaw);
  mem->Read(key_f2_yaw, (uint32_t*)&f2_yaw);

  mem->Read(key_p_yaw,  (uint32_t*)&controller.kq(2));
  mem->Read(key_d_yaw,  (uint32_t*)&controller.kw(2));

  mem->Read(key_wz_f0, (uint32_t*)&wz_f0);
  BiquadLpfDynamicCInit(&wz_filter,  wz_f0);

  mem->Read(key_pulse_sat,  (uint32_t*)&pulse_sat);
  mem->Read(key_yaw_sat,    (uint32_t*)&yaw_sat);
  mem->Read(key_thrust_sat, (uint32_t*)&thrust_sat);

  mem->Read(key_advance_angle,  (uint32_t*)&advance_angle);

  // controller state
  t.sec = 0;
  t.nsec = 0;
}

void CoaxOpenAttitudeController::Update()
{
  struct Time new_t = GetTime();
  float dt = ToSeconds(SubtractTime(new_t, t));
  t = new_t;

  // collect estimated state
  QuatState est;
  est.q = ukf->ukf.get_q();
  est.w = ukf->ukf.get_w();

  float wz_filt = BiquadLpfDynamicCUpdate(&wz_filter,  est.w(2), dt);
  Eigen::Vector3f w; w << 0, 0, wz_filt;
  Eigen::Quaternionf des_q = Quaternionf::Identity();                           // this should use 'pilot_yaw'
  controller.Update(est.q, w, des_q, Eigen::Vector3f::Zero());
  float u_yaw = controller.u(2);

  // saturate thrust command
  pilot_thrust = SatFloat(pilot_thrust, -thrust_sat, thrust_sat);

  uz_f = f0_yaw + f1_yaw*pilot_thrust + f2_yaw*pilot_thrust*pilot_thrust;
  u_yaw += uz_f;

  // saturate yaw command 
  u_yaw = SatFloat(u_yaw, -pilot_thrust, pilot_thrust);
  u_yaw = SatFloat(u_yaw, -yaw_sat, yaw_sat);

  // calculate mean motor voltages
  top_mean    = pilot_thrust - u_yaw;
  bottom_mean = pilot_thrust + u_yaw;

  // pulsing phase
  top_pulse_phase = WrapRad(- pilot_pulse_phase + advance_angle);
}

void CoaxOpenAttitudeController::ReadMsg(CommunicationInterface& com, uint8_t* rx_data, uint8_t rx_length)
{
  uint8_t type = rx_data[0];

  if(type == kTypeCoaxOpenAttitudeController && rx_length >= 3) {
    uint8_t subtype = rx_data[1];
    uint8_t direction = rx_data[2];
      
    if(direction == GET_CMD) {
      uint8_t tx_msg[6];
      tx_msg[0] = subtype;
      tx_msg[1] = RESPONSE_CMD;
      switch(subtype) {
        case kSubtypeCoaxOpenAttitudeControllerF0Yaw:
          memcpy(&tx_msg[2], &f0_yaw, 4);
          com.SendPacket(kTypeCoaxOpenController, tx_msg, 6); break;
        case kSubtypeCoaxOpenAttitudeControllerF1Yaw:
          memcpy(&tx_msg[2], &f1_yaw, 4);
          com.SendPacket(kTypeCoaxOpenController, tx_msg, 6); break;
        case kSubtypeCoaxOpenAttitudeControllerF2Yaw:
          memcpy(&tx_msg[2], &f2_yaw, 4);
          com.SendPacket(kTypeCoaxOpenController, tx_msg, 6); break;
        case kSubtypeCoaxOpenAttitudeControllerPYaw:
          memcpy(&tx_msg[2], &controller.kq(2), 4);
          com.SendPacket(kTypeCoaxOpenController, tx_msg, 6); break;
        case kSubtypeCoaxOpenAttitudeControllerDYaw:
          memcpy(&tx_msg[2], &controller.kw(2), 4);
          com.SendPacket(kTypeCoaxOpenController, tx_msg, 6); break;
        case kSubtypeCoaxOpenAttitudeControllerWzF0:
          memcpy(&tx_msg[2], &wz_f0, 4);
          com.SendPacket(kTypeCoaxOpenController, tx_msg, 6); break;
        case kSubtypeCoaxOpenAttitudeControllerPulseSat:
          memcpy(&tx_msg[2], &pulse_sat, 4);
          com.SendPacket(kTypeCoaxOpenController, tx_msg, 6); break;
        case kSubtypeCoaxOpenAttitudeControllerYawSat:
          memcpy(&tx_msg[2], &yaw_sat, 4);
          com.SendPacket(kTypeCoaxOpenController, tx_msg, 6); break;
        case kSubtypeCoaxOpenAttitudeControllerThrustSat:
          memcpy(&tx_msg[2], &thrust_sat, 4);
          com.SendPacket(kTypeCoaxOpenController, tx_msg, 6); break;
        case kSubtypeCoaxOpenAttitudeControllerAdvanceAngle:
          memcpy(&tx_msg[2], &advance_angle, 4);
          com.SendPacket(kTypeCoaxOpenController, tx_msg, 6); break;
      }
    } // end if get

    else if(direction == SET_CMD) {
      switch(subtype) {
        case kSubtypeCoaxOpenAttitudeControllerF0Yaw:
          f0_yaw =      PunToFloat(&rx_data[3]);  break;
        case kSubtypeCoaxOpenAttitudeControllerF1Yaw:
          f1_yaw =      PunToFloat(&rx_data[3]);  break;
        case kSubtypeCoaxOpenAttitudeControllerF2Yaw:
          f2_yaw =      PunToFloat(&rx_data[3]);  break;
        case kSubtypeCoaxOpenAttitudeControllerPYaw:
          controller.kq(2) = PunToFloat(&rx_data[3]); break;
        case kSubtypeCoaxOpenAttitudeControllerDYaw:
          controller.kw(2) = PunToFloat(&rx_data[3]); break;
        case kSubtypeCoaxOpenAttitudeControllerWzF0:
          wz_f0 =       PunToFloat(&rx_data[3]);  break;
        case kSubtypeCoaxOpenAttitudeControllerPulseSat:
          pulse_sat =   PunToFloat(&rx_data[3]); break;
        case kSubtypeCoaxOpenAttitudeControllerYawSat:
          yaw_sat =   PunToFloat(&rx_data[3]); break;
        case kSubtypeCoaxOpenAttitudeControllerThrustSat:
          thrust_sat =   PunToFloat(&rx_data[3]); break;
        case kSubtypeCoaxOpenAttitudeControllerAdvanceAngle:
          advance_angle = PunToFloat(&rx_data[3]); break;
      }
    } // end if set

    else if(direction == SAVE_CMD) {
      switch(subtype) {
        case kSubtypeCoaxOpenAttitudeControllerF0Yaw:
          mem->Write(key_f0_yaw,  PunToUint32(f0_yaw)); break;
        case kSubtypeCoaxOpenAttitudeControllerF1Yaw:
          mem->Write(key_f1_yaw,  PunToUint32(f1_yaw)); break;
        case kSubtypeCoaxOpenAttitudeControllerF2Yaw:
          mem->Write(key_f2_yaw,  PunToUint32(f2_yaw)); break;
        case kSubtypeCoaxOpenAttitudeControllerPYaw:
          mem->Write(key_p_yaw,   PunToUint32(controller.kq(2)));  break;
        case kSubtypeCoaxOpenAttitudeControllerDYaw:
          mem->Write(key_d_yaw,   PunToUint32(controller.kw(2)));  break;
        case kSubtypeCoaxOpenAttitudeControllerWzF0:
          mem->Write(key_wz_f0,   PunToUint32(wz_f0));  break;
        case kSubtypeCoaxOpenAttitudeControllerPulseSat:
          mem->Write(key_pulse_sat, PunToUint32(pulse_sat)); break;
        case kSubtypeCoaxOpenAttitudeControllerYawSat:
          mem->Write(key_yaw_sat, PunToUint32(yaw_sat)); break;
        case kSubtypeCoaxOpenAttitudeControllerThrustSat:
          mem->Write(key_thrust_sat, PunToUint32(thrust_sat)); break;
        case kSubtypeCoaxOpenAttitudeControllerAdvanceAngle:
          mem->Write(key_advance_angle, PunToUint32(advance_angle)); break;
      }
    } // end if save
  } // end if kTypeQuatPD

  else if(type == kTypeOpenAttitudePilot && rx_length == sizeof(MsgOpenAttitudePilot)+1) {
    MsgOpenAttitudePilot *msg = (MsgOpenAttitudePilot*) &rx_data[1];
    pilot_thrust      = msg->thrust;
    top_pulse_amp     = msg->amplitude;
    pilot_pulse_phase = msg->phase;
    pilot_yaw         = msg->yaw;
    if(msg->obs_yaw >= -M_PI && msg->obs_yaw <= M_PI) { // if not nan
      ukf->ObserveYaw(msg->obs_yaw);
    }
  }

  else if(type == kTypeRequestMessage) {
    MsgRequestMessage *rx_req = (MsgRequestMessage*) &rx_data[1];
    if(rx_req->msg_type == kTypeCoaxOpenAttitudeControllerParam) {
      SendMsgCoaxOpenAttitudeControllerParam(com);
    }
  }
}

void CoaxOpenAttitudeController::SendMsgCoaxOpenAttitudeControllerParam(CommunicationInterface& com) {
  MsgCoaxOpenAttitudeControllerParam tx_msg;
  FillMsgCoaxOpenAttitudeControllerParam(&tx_msg);
  com.SendPacket(kTypeCoaxOpenAttitudeControllerParam, (uint8_t*)&tx_msg, sizeof(MsgCoaxOpenAttitudeControllerParam));
}

void CoaxOpenAttitudeController::FillMsgCoaxOpenAttitudeControllerParam(MsgCoaxOpenAttitudeControllerParam* tx_msg) {
  tx_msg->f0_yaw  = f0_yaw;         // model feedforward polynomial (volts)
  tx_msg->f1_yaw  = f1_yaw;         // model feedforward polynomial (volts/volt)
  tx_msg->f2_yaw  = f2_yaw;         // model feedforward polynomial (volts/volts^2)
  tx_msg->p_yaw   = controller.kq(2);   // proportional gain  (volts/rad)
  tx_msg->d_yaw   = controller.kw(2);   // deriative gain     (volts/(rad/s))
  tx_msg->wz_f0   = wz_f0;
  tx_msg->pulse_sat     = pulse_sat;
  tx_msg->yaw_sat       = yaw_sat;
  tx_msg->thrust_sat    = thrust_sat;
  tx_msg->advance_angle = advance_angle;
}

void CoaxOpenAttitudeController::SendMsgDebug(CommunicationInterface& com) {
  MsgCoaxOpenAttitudeControllerDebug tx_msg;
  FillMsgDebug(&tx_msg);
  com.SendPacket(kTypeCoaxOpenAttitudeControllerDebug, (uint8_t*)&tx_msg, sizeof(MsgCoaxOpenAttitudeControllerDebug));
}

void CoaxOpenAttitudeController::FillMsgDebug(MsgCoaxOpenAttitudeControllerDebug* tx_msg) {
  tx_msg->t                   = GetSeconds();
  tx_msg->uz_p                = controller.uq(2);
  tx_msg->uz_d                = controller.uw(2);;
  tx_msg->uz_f                = uz_f;
  tx_msg->pilot_thrust        = pilot_thrust;
  tx_msg->pilot_pulse_phase   = pilot_pulse_phase;
  tx_msg->top_mean            = top_mean;
  tx_msg->bottom_mean         = bottom_mean;
  tx_msg->top_pulse_amp       = top_pulse_amp;
  tx_msg->top_pulse_phase     = top_pulse_phase;
}