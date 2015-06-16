#ifndef WHEEL_CONTROL_NAIVE_H_
#define WHEEL_CONTROL_NAIVE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "DC_Motor.h"

void init_Dof(void);
void left_control(uint8_t direction, uint8_t speed);
void right_control(uint8_t direction, uint8_t speed);
void tilt_control(uint8_t direction, uint8_t speed);
void pan_control(uint8_t direction, uint8_t speed);

#ifdef __cplusplus
}
#endif

#endif
