#ifndef GYRO_ACCEL_DRIFT_UKF_H
#define GYRO_ACCEL_DRIFT_UKF_H

#include <Eigen/Dense>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <stdint.h>

using namespace Eigen;

#define N_SIGMA_PTS 12

template <class Scalar>
class GyroAccelDriftUkf
{
  public:
    GyroAccelDriftUkf() {
      kCholskyCoef = sqrt(N_SIGMA_PTS * Scalar(0.5));
    }

    void Update(Scalar dt,
                const Matrix<Scalar,3,1> &w_meas,
                const Matrix<Scalar,3,1> &a_meas);

    inline Quaternion<Scalar> get_q() {return q;};
    inline Matrix<Scalar,3,1> get_w() {return w;};
    inline Matrix<Scalar,6,6> get_P() {return P;};
    inline Matrix<Scalar,6,6> get_Q() {return Q;};
    inline Matrix<Scalar,6,6> get_R() {return R;};

    template <typename Derived>
    inline void set_q(const QuaternionBase<Derived>& q) {this->q = q;};

    template <typename Derived>
    inline void set_w(const MatrixBase<Derived>& w) {this->w = w;};

    template <typename Derived>
    inline void set_P(const MatrixBase<Derived>& P) {this->P = P;};

    template <typename Derived>
    inline void set_Q_diag(const MatrixBase<Derived>& Q_diag) {this->Q = Q_diag.asDiagonal();};

    template <typename Derived>
    inline void set_R_diag(const MatrixBase<Derived>& R_diag) {this->R = R_diag.asDiagonal();};

  private:
    DiagonalMatrix<Scalar,6> Q; // process covariance
    DiagonalMatrix<Scalar,6> R; // measurement covariance

    Quaternion<Scalar> q; // estimate quaternion
    Matrix<Scalar,3,1> w; // estimate body rates (rad/s)
    Matrix<Scalar,6,6> P; // estimate covariance

    Scalar kCholskyCoef;  // internally used calculated constant

    // quaternion mean by fixed number of iterations
    void StateMeanCov(const Quaternion<Scalar>        (&q)[N_SIGMA_PTS],
                      const Matrix<Scalar,3,N_SIGMA_PTS>  &w,
                      const Quaternion<Scalar>        &q_est,
                      Quaternion<Scalar>      &q_mean_best,
                      Matrix<Scalar,3,1>          &w_mean,
                      Matrix<Scalar,6,6>          &P,
                      Matrix<Scalar,6,N_SIGMA_PTS> &e);
};


namespace GyroAccelDriftUkfHelper
{
  template <typename DerivedA>
  inline Matrix<typename DerivedA::Scalar,3,1>
  Quat2Vect(const QuaternionBase<DerivedA>& q);

  template <typename Derived>
  inline Quaternion<typename Derived::Scalar>
  Vect2Quat(const MatrixBase<Derived>& v);
}


////////////////////////////////////////////////////////////////////////////////


// main update function
template <class Scalar>
void GyroAccelDriftUkf<Scalar>::Update( Scalar dt,
                                        const Matrix<Scalar,3,1> &w_meas,
                                        const Matrix<Scalar,3,1> &a_meas)
{
  using namespace GyroAccelDriftUkfHelper;
  typedef Matrix<Scalar,3,1> Vector3;
  typedef Matrix<Scalar,6,1> Vector6;
  typedef Matrix<Scalar,6,6> Matrix6;
  typedef Matrix<Scalar,3,N_SIGMA_PTS> Matrix3by12;
  typedef Matrix<Scalar,6,N_SIGMA_PTS> Matrix6by12;

  // create sigma points from the estimate covariance and process noise
  P += Q;
  LLT<Matrix6> llt; llt.compute(P);  // compute Cholsky decomposition
  Matrix6 W = llt.matrixL();
  Matrix<Scalar,3,N_SIGMA_PTS> Ww;     // rate vector sigma points
  Ww.template leftCols<6>() = kCholskyCoef * W.template bottomRows<3>();
  Ww.template rightCols<6>() = -Ww.template leftCols<6>();
  Quaternion<Scalar> Wq[N_SIGMA_PTS];        // orientation quaternion sigma points
  for(uint8_t i=0; i<6; i++) {        // form sigma quaternions Wq
    Wq[i] = Vect2Quat(kCholskyCoef * W.template block<3,1>(0,i));
    Wq[i+6] = Wq[i].conjugate();
  }

  // apply sigma point distribution to initial estimate
  // the center point of the distribution is just the initial estimate
  for(uint8_t i=0; i<N_SIGMA_PTS; i++) {
    Wq[i] = q * Wq[i];
  }
  Ww = Ww.colwise() + w;
  Quaternion<Scalar> W0q(q);
  Vector3 W0w(w);

  // transform points through the process
  // the process assume velocities are constant
  for(uint8_t i=0; i<N_SIGMA_PTS; i++) {
    Wq[i] = Wq[i] * Vect2Quat(Ww.col(i)*dt);
  }
  W0q = W0q * Vect2Quat(W0w*dt);

  // recover a priori mean and covariance from transformed points
  Quaternion<Scalar> q1;
  Vector3 w1;
  Matrix6by12 W1;
  StateMeanCov( Wq, Ww, W0q,      // inputs
                q1, w1, P, W1);   // results

  // transform Y sigma points through measurement model to get sampling Z
  Vector3 g_vect; g_vect << 0, 0, Scalar(9.8);
  Matrix6by12 Z;  // matrix of measurement vectors [acc; w]
  for(uint8_t i=0; i<N_SIGMA_PTS; i++) {
    Z.template block<3,1>(0,i) = Wq[i].conjugate()._transformVector(g_vect);
  }
  Z.template bottomRows<3>() = Ww;

  // find measurement mean and covariance
  Vector6 z_mean = Z.rowwise().mean();
  Z = Z.colwise() - z_mean;
  Matrix6 Pvv = Z * Z.transpose() / N_SIGMA_PTS; // covariance of expected measurements

  // innovation
  Vector6 v;
  v.head(3) = a_meas - z_mean.head(3);
  v.tail(3) = w_meas - z_mean.tail(3);
  Pvv += R; // covariance of expected measurements including sensor noise

  // cross correlation
  Matrix6 Pxz = W1 * Z.transpose() / N_SIGMA_PTS;

  // kalman gain
  Matrix6 K = Pxz * Pvv.inverse();

  // vectorial measurement update
  Vector6 x2_vect;
  x2_vect.template head<3>() = Quat2Vect(q1);
  x2_vect.template tail<3>() = w1;
  x2_vect += K*v;
  q = Vect2Quat(x2_vect.template head<3>());
  w = x2_vect.template tail<3>();
  P = P - K * Pvv * K.transpose();
}


// internally used quaternion mean by fixed number of iterations
template <class Scalar>
void GyroAccelDriftUkf<Scalar>::StateMeanCov(
                      const Quaternion<Scalar>            (&q)[N_SIGMA_PTS],
                      const Matrix<Scalar,3,N_SIGMA_PTS>  &w,
                      const Quaternion<Scalar>            &q_est,
                      Quaternion<Scalar>          &q_mean_best,
                      Matrix<Scalar,3,1>          &w_mean,
                      Matrix<Scalar,6,6>          &P,
                      Matrix<Scalar,6,N_SIGMA_PTS> &e)
{
  using namespace GyroAccelDriftUkfHelper;
  typedef Matrix<Scalar,3,1> Vector3;
  typedef Matrix<Scalar,3,N_SIGMA_PTS> Matrix3by12;

  const uint8_t kUpdateIter = 5;

  // iteratively improve quaternion mean and associated error vectors
  Quaternion<Scalar> q_mean = q_est;           // curr. estimate of quaternion mean
  Matrix3by12 e_vect;   // curr. difference vectors

  // maintain best quaternion mean and associated error vectors for result
  Scalar best_squared_norm = 1.0e9f;

  for(uint8_t k=0; k<kUpdateIter; k++) {

    Quaternion<Scalar> q_mean_inverse = q_mean.conjugate();
    for(uint8_t j_pt=0; j_pt<N_SIGMA_PTS; j_pt++) {
      Quaternion<Scalar> e_quat = q[j_pt] * q_mean_inverse;
      e_vect.col(j_pt) = Quat2Vect(e_quat);
    }

    Vector3 e_vect_mean = e_vect.rowwise().mean();
    Quaternion<Scalar> e_quat_mean;
    e_quat_mean = Vect2Quat(e_vect_mean);

    // improve quaternion mean estimate
    q_mean = e_quat_mean * q_mean;
    q_mean.normalize();

    Scalar temp_squared_norm = e_vect_mean.squaredNorm();
    if( temp_squared_norm < best_squared_norm) { // will always occur in first iteration
      q_mean_best = q_mean;
      e.template topRows<3>() = e_vect;
      best_squared_norm = temp_squared_norm;
    }
  }

  // build matrix of residuals and calculate covariance
  w_mean = w.rowwise().mean();
  e.template bottomRows<3>() = w.colwise() - w_mean;
  P = e*e.transpose() / N_SIGMA_PTS;
}


////////////////////////////////////////////////////////////////////////////////


template <typename DerivedA>
inline Matrix<typename DerivedA::Scalar,3,1>
GyroAccelDriftUkfHelper::Quat2Vect(const QuaternionBase<DerivedA>& q) {
  typename DerivedA::Scalar w = q.w();    // real part of quaternion
  typename DerivedA::Scalar coef = 1.0f;  // coefficient turns the vector part of the quaternion into a rotation vector
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
GyroAccelDriftUkfHelper::Vect2Quat(const MatrixBase<Derived>& v) {
  typename Derived::Scalar n = v.norm();
  if(n != 0.0f) {
    return Quaternion<typename Derived::Scalar>(AngleAxis<typename Derived::Scalar>(n, v/n));
  }
  else {
    return (Quaternion<typename Derived::Scalar>::Identity());
  }
}


#endif // GYRO_ACCEL_DRIFT_UKF_H