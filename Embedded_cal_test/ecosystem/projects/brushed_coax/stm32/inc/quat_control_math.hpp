#ifndef QUAT_CONTROL_MATH_HPP
#define QUAT_CONTROL_MATH_HPP

#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/Geometry>

template <typename Scalar> class QuatControlMath {

  public:

    QuatControlMath():
    kq(0,0,0),
    kw(0,0,0),
    uq(0,0,0),
    uw(0,0,0),
    u(0,0,0)
    {
    }


    void Update(const Eigen::Quaternion<Scalar> &q,     const Eigen::Matrix<Scalar,3,1> &w,
                const Eigen::Quaternion<Scalar> &q_des, const Eigen::Matrix<Scalar,3,1> &w_des)
    {
      // define the error quaternion such that q_err*q_des = q
      Eigen::Quaternion<Scalar> q_err = q_des.inverse()*q;    // makes sense

      // define the rotation error vector with direction equal to the axis of this 
      // rotation and magnitude equal to sin of the half-angle
      Eigen::Matrix<Scalar,3,1> eq = q_err.vec();
      if(q_err.w() < 0) {
        eq = -eq;
      }

      // define the rate error vector as the vector difference between desired and 
      // observed rates in body-fixed axes
      Eigen::Matrix<Scalar,3,1> ew = w - w_des;

      // control output is a vector in the body-fixed frame
      uq = (-kq).asDiagonal()*eq;
      uw = (-kw).asDiagonal()*ew;
      u = uq + uw;
    }

    // for small perturbations about the identity, there is mathematical
    // similarly to a linear PD controller with kp = 0.5*kq, kd = kw
    Eigen::Matrix<Scalar,3,1> kq; // gain on orientation (quaternion) error
    Eigen::Matrix<Scalar,3,1> kw; // gain on angular rate error

    Eigen::Matrix<Scalar,3,1> uq;  // controller output contribution from q
    Eigen::Matrix<Scalar,3,1> uw;  // controller output contribution frow w

    Eigen::Matrix<Scalar,3,1> u;  // controller output
};

#endif // QUAT_CONTROL_MATH_HPP