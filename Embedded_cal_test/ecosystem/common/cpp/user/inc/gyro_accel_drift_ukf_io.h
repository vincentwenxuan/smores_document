/* UKF Implementation Over Quaternions and Angular Rates
 * Jimmy Paulos, June 2013
 * Closely follows the algorithm of Kraft 2003 with resolved errata.
 */

#ifndef GYRO_ACCEL_DRIFT_UKF_IO_H
#define GYRO_ACCEL_DRIFT_UKF_IO_H

#include "embedded_eigen.h"
#include <stdint.h>
#include "communication_interface.h"
#include "common_message_types.h"
#include "euler_state.h"
#include "gyro_accel_drift_ukf.hpp"
#include "system_clock.h"

typedef Matrix<float,6,6> Matrix6f;
typedef Matrix<float,6,1> Vector6f;

class GyroAccelDriftUkfIo {
  public:
    GyroAccelDriftUkfIo();
    
    // Update with New Measurement
    // z_meas is [wx, wy, wz, ax, ay, az]
    void Update(struct Time t, float* w, float* a);

    void ObserveYaw(float yaw);
    void ObserveQuat(Eigen::Quaternionf& obs_q);
    
    void SendEstQuat(CommunicationInterface& com);
    
    void SendEstEuler(CommunicationInterface& com);
    void FillMsgEstEuler(MsgEstEuler* msg);
    
    void ReadMsg(CommunicationInterface& com, uint8_t* rx_data, uint8_t rx_length);

    void InitEstimate();

    void GetEulerState(EulerState& est);

    // Estimator State
    struct Time last_t;   // timestamp of last Update()
    GyroAccelDriftUkf<float> ukf;
    //Vector3f w;     // anglar rate estimate, rad/s, [ax, ay, az]
    //Quaternionf q;  // quaternion estimate, [qw, qi, qj, qk]
    //Matrix6f P;     // estimate covariance [vector displacement, vector rate]
    
    // Estimator Parameters
    //Matrix6f Q;   // process covariance, [vector displacement, vector rate]
    //Matrix6f R;   // measurement covariance
};

#endif // GYRO_ACCEL_DRIFT_UKF_IO_H
