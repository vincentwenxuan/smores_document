/* UKF Implementation Over Quaternions and Angular Rates
 * Jimmy Paulos, June 2013
 * Closely follows the algorithm of Kraft 2003 with resolved errata.
 */

#ifndef UKF_6DOF_H
#define UKF_6DOF_H

#include "embedded_eigen.h"
#include <stdint.h>
#include "communication_interface.h"
#include "common_message_types.h"
#include "euler_state.h"

using namespace Eigen;

typedef Matrix<float,6,6> Matrix6f;
typedef Matrix<float,6,1> Vector6f;

class Ukf6DOF {
  public:
    Ukf6DOF();
    
    Ukf6DOF(Vector3f w0,     // initial wx, wy, wz
            Quaternionf q0,  // initial qw, qi, qj, qk
            Matrix6f P0,     // initial estimate covariance
            Matrix6f Q,      // process covariance, [wx, wy, wz, x, y, z]
            Matrix6f R);     // sensor covariance, [ax, ay, az, wx, wy, wz]

    
    // Update with New Measurement
    // z_meas is [wx, wy, wz, ax, ay, az]
    void Update(float t, float* w, float* a);

    void ObserveYaw(float yaw);
    
    void SendEstQuat(CommunicationInterface& com);
    
    void SendEstEuler(CommunicationInterface& com);
    void FillMsgEstEuler(MsgEstEuler* msg);
    
    void ReadMsg(CommunicationInterface& com, uint8_t* rx_data, uint8_t rx_length);

    void InitEstimate();

    void GetEulerState(EulerState& est);

    // Estimator State
    float last_t;   // timestamp of last Update(), seconds
    Vector3f w;     // anglar rate estimate, rad/s, [ax, ay, az]
    Quaternionf q;  // quaternion estimate, [qw, qi, qj, qk]
    Matrix6f P;     // estimate covariance [vector displacement, vector rate]
    
    // Estimator Parameters
    Matrix6f Q;   // process covariance, [vector displacement, vector rate]
    Matrix6f R;   // measurement covariance
};

#endif // UKF_6DOF_H
