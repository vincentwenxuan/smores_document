/* UKF Implementation Over Quaternions and Angular Rates
 * Jimmy Paulos, June 2013
 * Closely follows the algorithm of Kraft 2003 with resolved errata.
 */

#include "embedded_eigen.h"

#define N_SIGMA_PTS 12

using namespace Eigen;

typedef Matrix<float,6,6> Matrix6f;
typedef Matrix<float,6,1> Vector6f;

void UpdateHelper( const Vector6f& z_meas,  // input
             float dt,                // input
             const Matrix6f& Q,       // input
             const Matrix6f& R,       // input
             Matrix6f& P,             // input-output
             Quaternionf& q,          // input-output
             Vector3f& w);            // input-output

void StateMeanCov(const Quaternionf q[],
                  const Matrix<float,3,N_SIGMA_PTS> w,
                  const Quaternionf& q_est,
                  Quaternionf& q_res,
                  Vector3f& w_res,
                  Matrix6f& P,
                  Matrix<float,6,N_SIGMA_PTS>& e);
                              
void SigmaPoints( const Matrix6f& P,
                  Quaternion<float> Wq[N_SIGMA_PTS],
                  Matrix<float,3,N_SIGMA_PTS>& Ww);
                              
//void Vect2Quat(const Ref<const MatrixXf> v, Quaternionf& q);
void Vect2Quat(const Vector3f& v, Quaternionf& q);

//void Quat2Vect(const Quaternionf& q, Ref<MatrixXf> v);
void Quat2Vect(const Quaternionf& q, Vector3f& v);

void ExpectedAccelleration(const Quaternionf& q, Vector3f& z_meas_exp);