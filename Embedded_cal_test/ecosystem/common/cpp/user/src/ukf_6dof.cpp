/* UKF Implementation Over Quaternions and Angular Rates
 * Jimmy Paulos, June 2013
 * Closely follows the algorithm of Kraft 2003 with resolved errata.
 */

#define NDEBUG // for Eigen
//#define EIGEN_UNROLLING_LIMIT 0

#include "ukf_6dof.h"
#include "ukf_helper.h"
#include "usb_interface.h"
#include "common_message_types.h"
#include <stdint.h>
#include <float.h>

const uint8_t kSubtypeUkf6dofInitEst = 0;




// Decompose a general rotation into a rotation perpendicular to the z-axis followed by a 
// rotation about the z-axis. Thus, 
// quat = z_quat * xy_quat. The first rotation, xy_quat, has zero k
// component and the second rotation, z_quat, has zero i,j components.
template<typename Scalar>
void decompose_xy_z(Quaternion<Scalar> &quat, Quaternion<Scalar> &xy_quat, Quaternion<Scalar> &z_quat) {
  Scalar denom = sqrt(pow(quat.w(),2) + pow(quat.z(),2));
  z_quat =  Quaternion<Scalar>( quat.w()/denom,
                                0,
                                0,
                                quat.z()/denom);
  xy_quat = Quaternion<Scalar>( denom,
                                (quat.w()*quat.x() + quat.y()*quat.z())/denom,
                                (quat.w()*quat.y() - quat.x()*quat.z())/denom,
                                0);
}




/// Default constructor provides "reasonable" model parameters.
Ukf6DOF::Ukf6DOF() {
  
  // initial estimate values
  InitEstimate();

  /*
  // old settings
  Vector6f temp;
  temp << 1e-3f, 1e-3f, 1e-3f, 1e-3f, 1e-3f, 1e-3f;
  P = temp.asDiagonal();                            // initial estimate covariance
  temp << 1e-6f, 1e-6f, 1e-6f, 1e-2f, 1e-2f, 1e-2f;
  Q = temp.asDiagonal();                            // process covariance
  temp << 1e-1f, 1e-1f, 1e-1f, 1e-2f, 1e-2f, 1e-2f;
  R = temp.asDiagonal();                            // measurement covariance
  */

  // settings that work ok on the brushless_module_v2 board
  Vector6f temp;
  temp << 1e-2f, 1e-2f, 1e-2f, 1e-2f, 1e-2f, 1e-2f;
  P = temp.asDiagonal();               // initial estimate covariance
  temp << 1e-6f, 1e-6f, 1e-6f, 1e-2f, 1e-2f, 1e-2f;
  Q = temp.asDiagonal();               // model process covariance
  temp << 1e1f, 1e1f, 1e1f, 1e-2f, 1e-2f, 1e-2f;
  R = temp.asDiagonal();               // model measurement covariance
}

/// Full constructor assigns complete estimator state and model values.
Ukf6DOF::Ukf6DOF( Vector3f w,
                  Quaternionf q,
                  Matrix6f P,
                  Matrix6f Q,
                  Matrix6f R) {
  this->w = w;
  this->q = q;
  this->P = P;
  this->Q = Q;
  this->R = R;
  last_t = 0.0f;
}

/// Update method.
void Ukf6DOF::Update(float meas_t, float* meas_w, float* meas_a) {

  // calculate time delta
  float dt;
  if(last_t == 0.0f) // let be zero if not prior updates
    dt = 0.001f;
  else            // else find difference in seconds
    dt = meas_t - last_t;
  last_t = meas_t;
  
  float temp[6];
  for(uint8_t i=0; i<3; i++) {
    temp[i] = meas_a[i];
    temp[i+3] = meas_w[i];
  }
  Vector6f z_meas = Map<Vector6f>(temp);

  // make update
  UpdateHelper(z_meas,  // input
         dt,      // input
         Q,       // input
         R,       // input
         P,       // input-output
         q,       // input-output
         w);      // input-output
}

void Ukf6DOF::ObserveYaw(float yaw) {
  // expect a yaw consistent with a quat = z_quat * xy_quat decomposition
  // decompose the current estimate
  Quaternionf z_quat_original;
  Quaternionf xy_quat;
  decompose_xy_z(q, xy_quat, z_quat_original);
  // form the replacement z component
  Quaternionf z_quat_replacement(AngleAxisf(yaw, Vector3f::UnitZ()));
  // recompose the estimate
  q = z_quat_replacement * xy_quat;
}

void Ukf6DOF::ReadMsg(CommunicationInterface& com, uint8_t* rx_data, uint8_t rx_length) {
  // rx_length intentionally unused, probably should be used

  uint8_t type = rx_data[0];
  if(type == kTypeUkf6dof) {
    uint8_t subtype = rx_data[1];
    if(subtype == kSubtypeUkf6dofInitEst) {
      InitEstimate();
    }
  }
  else if(type == kTypeRequestMessage) {
    MsgRequestMessage *data_in = (MsgRequestMessage*) &rx_data[1];
    uint8_t msg_type = data_in->msg_type;

    if     (msg_type == kTypeEstQuat) {
      SendEstQuat(com);
    } else if (msg_type == kTypeEstEuler) {
      SendEstEuler(com);
    }
  }
}

void Ukf6DOF::SendEstQuat(CommunicationInterface& com) {
    MsgEstQuat tx_msg;
    tx_msg.t = last_t;
    tx_msg.wx = w(0);
    tx_msg.wy = w(1);
    tx_msg.wz = w(2);
    tx_msg.qw = q.w();
    tx_msg.qx = q.x();
    tx_msg.qy = q.y();
    tx_msg.qz = q.z();
    com.SendPacket(kTypeEstQuat, (uint8_t*)&tx_msg, sizeof(MsgEstQuat));
}

void Ukf6DOF::SendEstEuler(CommunicationInterface& com) {
    MsgEstEuler tx_msg;
    FillMsgEstEuler(&tx_msg);
    com.SendPacket(kTypeEstEuler, (uint8_t*)&tx_msg, sizeof(MsgEstEuler));
}

void Ukf6DOF::FillMsgEstEuler(MsgEstEuler* msg) {
  Vector3f zyx = q.toRotationMatrix().eulerAngles(2,1,0);
  msg->t = last_t;
  msg->wx = w(0);
  msg->wy = w(1);
  msg->wz = w(2);
  msg->roll  = zyx(2);
  msg->pitch = zyx(1);
  msg->yaw   = zyx(0);
}

void Ukf6DOF::InitEstimate() {
  last_t = 0.0f;
  w = Vector3f::Zero();
  q = Quaternionf::Identity();
}

void Ukf6DOF::GetEulerState(EulerState& est) {
  // rotation about Z, then Y, then X
  Vector3f rpy_zyx = q.toRotationMatrix().eulerAngles(2,1,0);
  est.wx = w[0];
  est.wy = w[1];
  est.wz = w[2];
  est.roll = rpy_zyx(2);
  est.pitch = rpy_zyx(1);
  est.yaw = rpy_zyx(0);
}