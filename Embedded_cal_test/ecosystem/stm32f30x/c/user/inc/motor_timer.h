#ifndef __MOTOR_TIMER_H
#define __MOTOR_TIMER_H

#include "common_peripherals.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(MOTOR_TIMER)
  void InitMotorTimer();
#if MOTOR_TIMER_CHANNELS==1
  void SetMotorPWM(uint16_t, uint16_t);
#elif MOTOR_TIMER_CHANNELS==2
  void SetMotorPWM(uint16_t, uint16_t, uint16_t);
#elif MOTOR_TIMER_CHANNELS==3
  void SetMotorPWM(uint16_t, uint16_t, uint16_t, uint16_t);
#else
#error "MOTOR_TIMER defined, but MOTOR_TIMER_CHANNELS not 1-3"
#endif
  void MotorTimerCallback(void);
  void MAKENAME(TIM,MAKENAME(MOTOR_TIMER,_IRQHandler))(void);

#endif //defined(MOTOR_TIMER)

#ifdef __cplusplus
}
#endif // __cplusplus

#endif //__MOTOR_TIMER_H