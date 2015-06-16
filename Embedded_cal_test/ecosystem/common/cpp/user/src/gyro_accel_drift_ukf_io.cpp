/* UKF Implementation Over Quaternions and Angular Rates
 * Jimmy Paulos, June 2013
 * Closely follows the algorithm of Kraft 2003 with resolved errata.
 */

#define NDEBUG // for Eigen
//#define EIGEN_UNROLLING_LIMIT 0

#include "gyro_accel_drift_ukf_io.h"
#include "usb_interface.h"
#include "common_message_types.h"
#include "system_clock.h"
#include <stdint.h>
#include <float.h>

const uint8_t kSubtypeUkf6dofInitEst = 0;

using namespace Eigen;

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
GyroAccelDriftUkfIo::GyroAccelDriftUkfIo() {
  
  InitEstimate();

  Vector6f temp;
  temp << 1e-2f, 1e-2f, 1e-2f, 1e-2f, 1e-2f, 1e-2f;
  Matrix6f P = temp.asDiagonal();
  ukf.set_P(P);               // initial estimate covariance
  temp << 1e-6f, 1e-6f, 1e-6f, 1e-2f, 1e-2f, 1e-2f;
  ukf.set_Q_diag(temp);               // model process covariance
  temp << 1e1f, 1e1f, 1e1f, 1e-2f, 1e-2f, 1e-2f;
  ukf.set_R_diag(temp);               // model measurement covariance
}

/// Update method.
void GyroAccelDriftUkfIo::Update(struct Time meas_t, float* meas_w, float* meas_a) {

  float dt;
  if(meas_t.sec == 0 && meas_t.nsec == 0)
    dt = 0.001f;
  else
    dt = ToSeconds(SubtractTime(meas_t, last_t));
  last_t = meas_t;
  
  Vector3f w_meas_eigen = Map<Vector3f>(meas_w);
  Vector3f a_meas_eigen = Map<Vector3f>(meas_a);
  ukf.Update(dt, w_meas_eigen, a_meas_eigen);
}

void GyroAccelDriftUkfIo::ObserveYaw(float yaw) {
  // expect a yaw consistent with a quat = z_quat * xy_quat decomposition
  // decompose the current estimate
  Quaternionf z_quat_original;
  Quaternionf xy_quat;
  Quaternionf q; q = ukf.get_q();
  decompose_xy_z(q, xy_quat, z_quat_original);
  // form the replacement z component
  Quaternionf z_quat_replacement(AngleAxisf(yaw, Vector3f::UnitZ()));
  // recompose the estimate
  ukf.set_q(z_quat_replacement * xy_quat);
}

void GyroAccelDriftUkfIo::ObserveQuat(Quaternionf& obs_q) {
  ukf.set_q(obs_q);
}

void GyroAccelDriftUkfIo::ReadMsg(CommunicationInterface& com, uint8_t* rx_data, uint8_t rx_length) {
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

void GyroAccelDriftUkfIo::SendEstQuat(CommunicationInterface& com) {
    MsgEstQuat tx_msg;
    tx_msg.t = ToSeconds(last_t);
    Vector3f w; w = ukf.get_w();
    Quaternionf q; q = ukf.get_q();
    tx_msg.wx = w(0);
    tx_msg.wy = w(1);
    tx_msg.wz = w(2);
    tx_msg.qw = q.w();
    tx_msg.qx = q.x();
    tx_msg.qy = q.y();
    tx_msg.qz = q.z();
    com.SendPacket(kTypeEstQuat, (uint8_t*)&tx_msg, sizeof(MsgEstQuat));
}

void GyroAccelDriftUkfIo::SendEstEuler(CommunicationInterface& com) {
    MsgEstEuler tx_msg;
    FillMsgEstEuler(&tx_msg);
    com.SendPacket(kTypeEstEuler, (uint8_t*)&tx_msg, sizeof(MsgEstEuler));
}

void GyroAccelDriftUkfIo::FillMsgEstEuler(MsgEstEuler* msg) {
  Quaternionf q; q = ukf.get_q();
  Vector3f zyx = q.toRotationMatrix().eulerAngles(2,1,0);
  Vector3f w; w = ukf.get_w();
  msg->t = ToSeconds(last_t);
  msg->wx = w(0);
  msg->wy = w(1);
  msg->wz = w(2);
  msg->roll  = zyx(2);
  msg->pitch = zyx(1);
  msg->yaw   = zyx(0);
}

void GyroAccelDriftUkfIo::InitEstimate() {
  last_t.sec  = 0;
  last_t.nsec = 0;
  ukf.set_w(Vector3f::Zero());
  ukf.set_q(Quaternionf::Identity());
}

void GyroAccelDriftUkfIo::GetEulerState(EulerState& est) {
  // rotation about Z, then Y, then X
  Quaternionf q; q = ukf.get_q();
  Vector3f rpy_zyx = q.toRotationMatrix().eulerAngles(2,1,0);
  Vector3f w; w = ukf.get_w();
  est.wx = w[0];
  est.wy = w[1];
  est.wz = w[2];
  est.roll = rpy_zyx(2);
  est.pitch = rpy_zyx(1);
  est.yaw = rpy_zyx(0);
}