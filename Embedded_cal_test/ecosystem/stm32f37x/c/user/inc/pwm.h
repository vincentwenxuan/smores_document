#ifndef PWM_H
#define PWM_H

#include "common_peripherals.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(USING_PWM1)

void InitPWM(void);

uint32_t GetPWMPeriod(void);

void EnablePWM(void);

void DisablePWM(void);

void SetPWM(uint8_t,float);

void SetPWM1(float);
void SetServo1(float);

#ifdef USING_PWM2
void SetPWM2(float);
void SetServo2(float);
#endif //USING_PWM2

#ifdef USING_PWM3
void SetPWM3(float);
void SetServo3(float);
#endif //USING_PWM3

#ifdef USING_PWM4
void SetPWM4(float);
void SetServo4(float);
#endif //USING_PWM4

#endif //defined(USING_PWM1)

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // PWM_H