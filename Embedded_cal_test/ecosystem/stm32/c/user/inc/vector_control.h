#ifndef VECTOR_CONTROL_H
#define VECTOR_CONTROL_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
		
enum Shape{kTrapezoidal=0, kSinusoidal=1, kQuadrature=2};

float MechToElecAngle(float mech_angle, uint16_t motor_poles);
void CalcSpaceVector(float elec_ang, float amplitude, float *pwms, enum Shape sv_shape);	

#ifdef __cplusplus
}
#endif // __cplusplus
#endif //VECTOR_CONTROL_H
