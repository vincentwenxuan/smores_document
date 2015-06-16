#ifndef ANTICOGGING_H
#define ANTICOGGING_H

#include <stdint.h>
//#include "stm32f37x.h"
#include "common_peripherals.h"

#ifdef USING_ANTICOGGING

#ifdef __cplusplus
extern "C" {
#endif

float GetAnticogging(uint16_t index);
float GetAnticoggingFloat(float mech_angle);
int8_t SetAnticogging(uint16_t index, float value);
void EraseAnticogging();

#ifdef __cplusplus
}
#endif

#endif // USING_ANTICOGGING

#endif // ANTICOGGING_H