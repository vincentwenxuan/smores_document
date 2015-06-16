#include "pulsing_coax_controller_quat.h"
#include "math_helper.h"
#include <math.h>

// persistent memory key values (based on component type and subtypes)
const uint16_t key_advance_angle    = (uint16_t)256*kTypePulsingCoaxControllerQuat + kSubtypeAdvanceAngle;
const uint16_t key_enable_bottom    = (uint16_t)256*kTypePulsingCoaxControllerQuat + kSubtypeEnableBottom;
const uint16_t key_pulse_sat        = (uint16_t)256*kTypePulsingCoaxControllerQuat + kSubtypePulseSat;
const uint16_t key_pulse_thresh     = (uint16_t)256*kTypePulsingCoaxControllerQuat + kSubtypePulseThresh;
const uint16_t key_pulse_min        = (uint16_t)256*kTypePulsingCoaxControllerQuat + kSubtypePulseMin;
const uint16_t key_yaw_sat          = (uint16_t)256*kTypePulsingCoaxControllerQuat + kSubtypeYawSat;
const uint16_t key_thrust_sat       = (uint16_t)256*kTypePulsingCoaxControllerQuat + kSubtypeThrustSat;

// constants
const float kMaxRollDes = 45.0f * PI / 180.0f;


// Memory constructor, follow with InitFromMemory() after Freeze()
PulsingCoaxControllerQuat::PulsingCoaxControllerQuat( PersistentMemory &memory,
                                                      QuatPD &controller,
                                                      Mpu60XXImu &inertial_measurement_unit,
                                                      GyroAccelDriftUkfIo &estimator,
                                                      MotorHal &motor_hal_ref):
// outputs
top_mean(0),
top_pulse_amp(0),
top_pulse_phase(0),
bottom_mean(0),
u_phase(0),
// flash parameters
advance_angle(-0.8f),
pulse_thresh(0.056f),
pulse_min(1.5f),
pulse_sat(4.0f),
yaw_sat(1.5f),
thrust_sat(5.6f),
enable_bottom(1),
// desired state
thrust_des(0)
{
  // pointers to used components
  mem = &memory;
  pd = &controller;
  imu = &inertial_measurement_unit;
  ukf = &estimator;
  motor_hal = &motor_hal_ref;

  // request persistent memory service
  mem->AddKey(key_advance_angle);
  mem->AddKey(key_enable_bottom);
  mem->AddKey(key_pulse_sat);
  mem->AddKey(key_pulse_thresh);
  mem->AddKey(key_pulse_min);
  mem->AddKey(key_yaw_sat);
  mem->AddKey(key_thrust_sat);

  // initialize from memory
  mem->Read(key_advance_angle,  (uint32_t*)&advance_angle);
  mem->Read(key_enable_bottom,  (uint32_t*)&enable_bottom);
  mem->Read(key_pulse_sat,      (uint32_t*)&pulse_sat);
  mem->Read(key_pulse_thresh,   (uint32_t*)&pulse_thresh);
  mem->Read(key_pulse_min,      (uint32_t*)&pulse_min);
  mem->Read(key_yaw_sat,        (uint32_t*)&yaw_sat);
  mem->Read(key_thrust_sat,     (uint32_t*)&thrust_sat);

  BiquadLpfDynamicCInit(&omega_filter,  20.0);

  // controller state
  t.sec = 0;
  t.nsec = 0;
}


// Update outputs based on desired and estimated state
void PulsingCoaxControllerQuat::Update() {
  struct Time new_t = GetTime();
  float dt = ToSeconds(SubtractTime(new_t, t));
  t = new_t;

  // collect estimated state
  QuatState est;
  est.q = ukf->ukf.get_q();
  est.w = ukf->ukf.get_w();

  // calculate PD control law
  float omega = motor_hal->get_top_speed() - motor_hal->get_bottom_speed(); // basis for angular momentum estimate
  float omega_filt = BiquadLpfDynamicCUpdate(&omega_filter,  omega, dt);

  pd->Calculate(dt, thrust_des, des, est, omega_filt, u);

  // saturate thrust command
  u.thrust = SatFloat(u.thrust, -thrust_sat, thrust_sat);

  // saturate yaw command 
  u.yaw = SatFloat(u.yaw, -u.thrust, u.thrust);
  u.yaw = SatFloat(u.yaw, -yaw_sat, yaw_sat);
  
  // calculate mean motor voltages
  top_mean = u.thrust - u.yaw;
  if(enable_bottom)
    bottom_mean = u.thrust + u.yaw;
  else
    bottom_mean = 0;

  // map roll and pitch to motor pulsing commands
  u_phase = atan2f(u.pitch, u.roll);
  top_pulse_amp = sqrtf(u.roll*u.roll + u.pitch*u.pitch);
  if(top_pulse_amp < pulse_thresh) {
    top_pulse_amp = 0;
  }
  else {
    top_pulse_amp = top_pulse_amp + pulse_min;
  }

  // amplitude saturation
  top_pulse_amp = fmin(top_pulse_amp, pulse_sat);                // no pulse above sat value

  // phase correction
  top_pulse_phase = WrapRad(- u_phase + advance_angle);

  // zero pulsing amplitude if top rotor spinning too slowly
  if(motor_hal->get_top_speed() < 100.0f) { // 100 rad/s about equal to 1000 rpm
    top_pulse_amp = 0;
  }
}


void PulsingCoaxControllerQuat::ReadMsg(CommunicationInterface& com, uint8_t* rx_data, uint8_t rx_length) {
  uint8_t type = rx_data[0];

  if(type == kTypeQuatPilot && rx_length == sizeof(MsgQuatPilot)+1) {
    MsgQuatPilot *msg = (MsgQuatPilot*) &rx_data[1];
    des.q = Eigen::Quaternionf(msg->qw, msg->qx, msg->qy, msg->qz);
    des.w = Eigen::Vector3f::Zero();
    if(msg->obs_yaw >= -M_PI && msg->obs_yaw <= M_PI) { // if not nan
      ukf->ObserveYaw(msg->obs_yaw);
    }
    thrust_des      = msg->thrust;
  }

  else if(type == kTypeQuatFullObsPilot && rx_length == sizeof(MsgQuatFullObsPilot)+1) {
    MsgQuatFullObsPilot *msg = (MsgQuatFullObsPilot*) &rx_data[1];
    des.q = Eigen::Quaternionf(msg->qw, msg->qx, msg->qy, msg->qz);
    des.w << msg->wx, msg->wy, msg->wz;
    if(msg->obs_qw >= -1.0 && msg->obs_qw <= 1.0) { // if not nan
      Quaternionf obs_q = Eigen::Quaternionf(msg->obs_qw, msg->obs_qx, msg->obs_qy, msg->obs_qz);
      ukf->ObserveQuat(obs_q);
    }
    thrust_des      = msg->thrust;
  }

  else if(type == kTypeRequestMessage) {
    MsgRequestMessage *rx_req = (MsgRequestMessage*) &rx_data[1];

    if(rx_req->msg_type == kTypePulsingCoaxParam) {
      SendMsgPulsingCoaxParam(com);
    }
  }
  else if(type == kTypePulsingCoaxControllerQuat && rx_length >= 3) {
    uint8_t subtype = rx_data[1];
    uint8_t direction = rx_data[2];
    
    if(direction == GET_CMD) {
      uint8_t tx_msg[6];
      tx_msg[0] = subtype;
      tx_msg[1] = RESPONSE_CMD;
      switch(subtype) {
        case kSubtypeAdvanceAngle:
          memcpy(&tx_msg[2], &advance_angle, 4);
          com.SendPacket(kTypePulsingCoaxControllerQuat, tx_msg, 6); break;
        case kSubtypeEnableBottom:
          tx_msg[2] = enable_bottom;
          com.SendPacket(kTypePulsingCoaxControllerQuat, tx_msg, 3); break;
        case kSubtypePulseSat:
          memcpy(&tx_msg[2], &pulse_sat, 4);
          com.SendPacket(kTypePulsingCoaxControllerQuat, tx_msg, 6); break;
        case kSubtypePulseThresh:
          memcpy(&tx_msg[2], &pulse_thresh, 4);
          com.SendPacket(kTypePulsingCoaxControllerQuat, tx_msg, 6); break;
        case kSubtypePulseMin:
          memcpy(&tx_msg[2], &pulse_min, 4);
          com.SendPacket(kTypePulsingCoaxControllerQuat, tx_msg, 6); break;
        case kSubtypeYawSat:
          memcpy(&tx_msg[2], &yaw_sat, 4);
          com.SendPacket(kTypePulsingCoaxControllerQuat, tx_msg, 6); break;
        case kSubtypeThrustSat:
          memcpy(&tx_msg[2], &thrust_sat, 4);
          com.SendPacket(kTypePulsingCoaxControllerQuat, tx_msg, 6); break;
      }
    } // end if get

    else if(direction == SET_CMD) {
      switch(subtype) {
        case kSubtypeAdvanceAngle:
          advance_angle = PunToFloat(&rx_data[3]); break;
        case kSubtypeEnableBottom:
          enable_bottom = rx_data[3]; break;
        case kSubtypePulseSat:
          pulse_sat =   PunToFloat(&rx_data[3]); break;
        case kSubtypePulseThresh:
          pulse_thresh =   PunToFloat(&rx_data[3]); break;
        case kSubtypePulseMin:
          pulse_min =   PunToFloat(&rx_data[3]); break;
        case kSubtypeYawSat:
          yaw_sat =   PunToFloat(&rx_data[3]); break;
        case kSubtypeThrustSat:
          thrust_sat =   PunToFloat(&rx_data[3]); break;
      }
    } // end if set

    else if(direction == SAVE_CMD) {
      switch(subtype) {
        case kSubtypeAdvanceAngle:
          mem->Write(key_advance_angle, PunToUint32(advance_angle)); break;
        case kSubtypeEnableBottom:
          mem->Write(key_enable_bottom, enable_bottom); break;
        case kSubtypePulseSat:
          mem->Write(key_pulse_sat, PunToUint32(pulse_sat)); break;
        case kSubtypePulseThresh:
          mem->Write(key_pulse_thresh, PunToUint32(pulse_thresh)); break;
        case kSubtypePulseMin:
          mem->Write(key_pulse_min, PunToUint32(pulse_min)); break;
        case kSubtypeYawSat:
          mem->Write(key_yaw_sat, PunToUint32(yaw_sat)); break;
        case kSubtypeThrustSat:
          mem->Write(key_thrust_sat, PunToUint32(thrust_sat)); break;
      }
    } // end if save
  }
}


////////////////////////////////////////////////////////////////////////////////

void PulsingCoaxControllerQuat::SendMsgPulsingCoaxParam(CommunicationInterface& com) {
  MsgPulsingCoaxParam tx_msg;
  FillMsgPulsingCoaxParam(&tx_msg);
  com.SendPacket(kTypePulsingCoaxParam, (uint8_t*)&tx_msg, sizeof(MsgPulsingCoaxParam));
}

void PulsingCoaxControllerQuat::FillMsgPulsingCoaxParam(MsgPulsingCoaxParam* tx_msg) {
  tx_msg->advance_angle = advance_angle;
  tx_msg->enable_bottom = enable_bottom;
  tx_msg->pulse_sat     = pulse_sat;
  tx_msg->pulse_thresh  = pulse_thresh;
  tx_msg->pulse_min     = pulse_min;
  tx_msg->yaw_sat       = yaw_sat;
  tx_msg->thrust_sat    = thrust_sat;
}


////////////////////////////////////////////////////////////////////////////////

void PulsingCoaxControllerQuat::SendMsgRollReport(CommunicationInterface& com) {
  MsgRollReport tx_msg;
  FillMsgRollReport(&tx_msg);
  com.SendPacket(kTypeRollReport, (uint8_t*)&tx_msg, sizeof(MsgRollReport));
}

void PulsingCoaxControllerQuat::FillMsgRollReport(MsgRollReport* tx_msg) {
  Vector3f w; w = ukf->ukf.get_w();

  tx_msg->t = GetSeconds();
  tx_msg->wx_meas = imu->w[0];
  tx_msg->wx_est  = w(0);
  tx_msg->wx_filt = pd->w_filt[0];
  tx_msg->ux_p    = pd->controller.uq(0);
  tx_msg->ux_d    = pd->controller.uw(0);
  tx_msg->top_mean    = top_mean;
  tx_msg->bottom_mean = bottom_mean;
  tx_msg->u_amp       = top_pulse_amp;
  tx_msg->u_phase     = u_phase;
}


////////////////////////////////////////////////////////////////////////////////

void PulsingCoaxControllerQuat::SendMsgControlCalc(CommunicationInterface& com) {
  MsgControlCalc tx_msg;
  FillMsgControlCalc(&tx_msg);
  com.SendPacket(kTypeControlCalc, (uint8_t*)&tx_msg, sizeof(MsgControlCalc));
}

void PulsingCoaxControllerQuat::FillMsgControlCalc(MsgControlCalc* tx_msg) {
  tx_msg->t = GetSeconds();
  tx_msg->ux_p = pd->controller.uq(0);
  tx_msg->uy_p = pd->controller.uq(1);
  tx_msg->uz_p = pd->controller.uq(2);
  tx_msg->ux_d = pd->controller.uw(0);;
  tx_msg->uy_d = pd->controller.uw(1);;
  tx_msg->uz_d = pd->controller.uw(2);;
  tx_msg->uz_f = pd->uz_f;
  tx_msg->ux_g = pd->ux_g;
  tx_msg->uy_g = pd->uy_g;
  tx_msg->thrust_des  = thrust_des;
  tx_msg->u_amp       = top_pulse_amp;
  tx_msg->u_phase     = u_phase;
}


////////////////////////////////////////////////////////////////////////////////

void PulsingCoaxControllerQuat::SendMsgYawReport(CommunicationInterface& com) {
  MsgYawReport tx_msg;
  FillMsgYawReport(&tx_msg);
  com.SendPacket(kTypeYawReport, (uint8_t*)&tx_msg, sizeof(MsgYawReport));
}

void PulsingCoaxControllerQuat::FillMsgYawReport(MsgYawReport* tx_msg) {
  Vector3f w; w = ukf->ukf.get_w();

  tx_msg->t = GetSeconds();
  tx_msg->wz_meas = imu->w[2];
  tx_msg->wz_est  = w(2);
  tx_msg->wz_filt = pd->w_filt[2];
  tx_msg->uz_p    = pd->controller.uq(2);
  tx_msg->uz_d    = pd->controller.uw(2);
  tx_msg->uz_f    = pd->uz_f;
  tx_msg->uz      = u.yaw;
  tx_msg->top_mean    = top_mean;
  tx_msg->bottom_mean = bottom_mean;
}