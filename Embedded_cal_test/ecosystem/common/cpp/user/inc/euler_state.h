#ifndef EULER_STATE_H
#define EULER_STATE_H

typedef struct {
  float wx;
  float wy;
  float wz;
  float roll;
  float pitch;
  float yaw;
} EulerState;

#endif // EULER_STATE_H