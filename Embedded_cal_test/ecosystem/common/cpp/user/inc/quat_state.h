#ifndef QUAT_STATE_HPP
#define QUAT_STATE_HPP

#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/Geometry>

typedef struct {
  Eigen::Vector3f     w;
  Eigen::Quaternionf  q;
} QuatState;

#endif // QUAT_STATE_HPP