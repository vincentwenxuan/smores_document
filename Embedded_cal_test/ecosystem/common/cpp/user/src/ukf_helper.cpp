/* UKF Implementation Over Quaternions and Angular Rates
 * Jimmy Paulos, June 2013
 * Closely follows the algorithm of Kraft 2003 with resolved errata.
 */
#define NDEBUG // for Eigen

#include "ukf_helper.h"
#include <stdint.h>
#include <float.h>

//#define EMBEDDED_LED_DEBUG

// conditional includes
#if DEBUG_LEVEL > 0
#include <iostream>
#endif

#ifdef EMBEDDED_LED_DEBUG
#include "common_peripherals.h"
#endif


using namespace Eigen;
using namespace std;


template <typename DerivedA>
inline Matrix<typename DerivedA::Scalar,3,1>
Quat2Vect(const QuaternionBase<DerivedA>& q) {
  float w = q.w();    // real part of quaternion
  float coef = 1.0f;  // coefficient turns the vector part of the quaternion into a rotation vector
  if(w < 0) {
    w = -w;       // for negative real part quaternions, consider the positive version instead
    coef = -1.0f;
  }
  if(w >= 1) {
    return (Matrix<typename DerivedA::Scalar,3,1>::Zero());
  }
  else {
    // this computation is bad numerically (0/0) but the polynomial approximation
    // for the quotient is well behaved.
    // coef = coef * 2*acos(w)/sqrt(1-w*w);
    coef = coef * (3.141585826873779296875f
                    + w * (-1.999252796173095703125f
                      + w * (1.55736255645751953125f
                        + w * (-1.240174770355224609375f
                          + w * (0.851204395294189453125f
                            + w * (-0.3984979093074798583984375f
                              + w * 8.7779559195041656494140625e-2f))))));
    return (coef * q.vec());
  }
}




template <typename Derived>
inline Quaternion<typename Derived::Scalar>
Vect2Quat(const MatrixBase<Derived>& v) {
  float n = v.norm();
  if(n != 0.0f) {
    return Quaternion<typename Derived::Scalar>(AngleAxis<typename Derived::Scalar>(n, v/n));
  }
  else {
    return (Quaternion<typename Derived::Scalar>::Identity());
  }
}




void UpdateHelper( const Vector6f& z_meas,  // input, [acc; w]
             float dt,                // input
             const Matrix6f& Q,       // input
             const Matrix6f& R,       // input
             Matrix6f& P,             // input-output
             Quaternionf& q,          // input-output
             Vector3f& w) {           // input-output

  // create sigma points from the estimate covariance
  Quaternionf Wq[N_SIGMA_PTS];    // orientation quaternion sigma points
  Matrix<float,3,N_SIGMA_PTS> Ww; // rate vector sigma points
  SigmaPoints(P+Q, Wq, Ww);


  // apply sigma point distribution to initial estimate
  // the center point of the distribution is just the initial estimate
  for(uint8_t i=0; i<N_SIGMA_PTS; i++) {
    Wq[i] = q * Wq[i];
  }
  Ww = Ww.colwise() + w;
  Quaternionf W0q(q);
  Vector3f W0w(w);


  // transform points through the process
  // the process assume velocities are constant
  for(uint8_t i=0; i<N_SIGMA_PTS; i++) {
    Wq[i] = Wq[i] * Vect2Quat(Ww.col(i)*dt);
  }
  W0q = W0q * Vect2Quat(W0w*dt);


  // recover a priori mean and covariance from transformed points
  Quaternionf q1;
  Vector3f w1;
  Matrix<float,6,N_SIGMA_PTS> W1;
  StateMeanCov( Wq, Ww, W0q,     // inputs
                q1, w1, P, W1); // results


  // transform Y sigma points through measurement model to get sampling Z
  Vector3f g_vect; g_vect << 0, 0, 9.8f;
  Matrix<float,6,N_SIGMA_PTS> Z;  // matrix of measurement vectors [acc; w]
  for(uint8_t i=0; i<N_SIGMA_PTS; i++) {
    Z.block<3,1>(0,i) = Wq[i].conjugate()._transformVector(g_vect);
  }
  Z.bottomRows<3>() = Ww;

  // Center Z Distribution and find Measurement Mean and Covariance
  Vector6f z_mean = Z.rowwise().mean();
  Z = Z.colwise() - z_mean;
  Matrix6f Pvv = Z * Z.transpose() / N_SIGMA_PTS; // covariance of expected measurements


  // Innovation
  Vector6f v = z_meas - z_mean;
  Pvv += R; // covariance of expected measurements including sensor noise


  // cross correlation
  Matrix6f Pxz = W1 * Z.transpose() / N_SIGMA_PTS;

  // kalman gain
  Matrix6f K = Pxz * Pvv.inverse();


  // vectorial measurement update
  Vector6f x2_vect;
  x2_vect.head<3>() = Quat2Vect(q1);
  x2_vect.tail<3>() = w1;
  x2_vect += K*v;
  q = Vect2Quat(x2_vect.head<3>());
  w = x2_vect.tail<3>();
  P = P - K * Pvv * K.transpose();
}


void SigmaPoints( const Matrix6f& P,
                  Quaternionf Wq[N_SIGMA_PTS],
                  Matrix<float,3,N_SIGMA_PTS>& Ww) {
  LLT<Matrix6f> llt;
  llt.compute(P);
  Matrix<float,6,N_SIGMA_PTS> W;          // vectorial sigma points W
  W.leftCols<6>() = llt.matrixL();        // Cholsky decomposition
  W.leftCols<6>() *= sqrtf(N_SIGMA_PTS/2);
  W.rightCols<6>() = -W.leftCols<6>();

  Ww = W.bottomRows<3>();

  for(uint8_t i=0; i<N_SIGMA_PTS; i++) {  // form sigma quaternions Wq
    Wq[i] = Vect2Quat(W.block<3,1>(0,i));
  }
}


// quaternion mean by fixed number of iterations
void StateMeanCov(const Quaternionf q[],
                  const Matrix<float,3,N_SIGMA_PTS> w,
                  const Quaternionf& q_est,
                  Quaternionf& q_mean_best,
                  Vector3f& w_mean,
                  Matrix6f& P,
                  Matrix<float,6,N_SIGMA_PTS>& e) {

  const uint8_t kUpdateIter = 5;

  // iteratively improve quaternion mean and associated error vectors
  Quaternionf q_mean = q_est;           // curr. estimate of quaternion mean
  Matrix<float,3,N_SIGMA_PTS> e_vect;   // curr. difference vectors

  // maintain best quaternion mean and associated error vectors for result
  float best_squared_norm = 1.0e9f;

  for(uint8_t k=0; k<kUpdateIter; k++) {

    Quaternionf q_mean_inverse = q_mean.conjugate();
    for(uint8_t j_pt=0; j_pt<N_SIGMA_PTS; j_pt++) {
      Quaternionf e_quat = q[j_pt] * q_mean_inverse;
      e_quat.normalize();
      e_vect.col(j_pt) = Quat2Vect(e_quat);
    }

    Vector3f e_vect_mean = e_vect.rowwise().mean();
    Quaternionf e_quat_mean;
    e_quat_mean = Vect2Quat(e_vect_mean);

    // improve quaternion mean estimate
    q_mean = e_quat_mean * q_mean;

    float temp_squared_norm = e_vect_mean.squaredNorm();
    if( temp_squared_norm < best_squared_norm) { // will always occur in first iteration
      q_mean_best = q_mean;
      e.topRows<3>() = e_vect;
      best_squared_norm = temp_squared_norm;
    }
  }

  // build matrix of residuals and calculate covariance
  w_mean = w.rowwise().mean();
  e.bottomRows<3>() = w.colwise() - w_mean;
  P = e*e.transpose() / N_SIGMA_PTS;
}


/*
// BOTTLENECK
// Only valid for unit quaternion inputs.
// Recall unit quaternion construction
//   q = cos(theta/2)*[1] + sin(theta/2)*[ux*i + uy*j + uz*k] for ijk = -1
//   for a rotation size theta about unit vector [ux, uy, uz]
void Quat2Vect(const Quaternionf& q, Ref<MatrixXf> v) {
  if(q.w() >= 1 || q.w() <= -1) {
    v = Vector3f::Zero();
  }
  else {
    v = q.vec();

    //float theta = 2*fast_acosf(q.w());
    float theta = 2*acosf(q.w());

    if(theta > kPi) {
      theta = k2Pi - theta;
      v = -v;
    }

    float n = v.norm();
    if(n > 1e-12f)
      v = v / n * theta;
    else
      v = Vector3f::Zero();
  }
      #if DEBUG_LEVEL > 0
      if(v(0) != v(0)) {
        cout << "\n\nQUAT2VECT NAN\n";
        cout << "in: [ " << q.w() << " " << q.x() << " " << q.y() << " " << q.z() << " ]\n";
        cout << "out: [ " << v(0) << " " << v(1) << " " << v(2) << " ]\n\n";
      }
      #endif
}
*/


void ExpectedAccelleration(const Quaternionf& q, Vector3f& z_meas_exp) {
  //z_meas_exp = (q.inverse() * Quaternionf(0,0,0,9.8f) * q).vec();
  z_meas_exp = (q.conjugate() * Quaternionf(0.0f, 0.0f, 0.0f, 9.8f) * q).vec();
}