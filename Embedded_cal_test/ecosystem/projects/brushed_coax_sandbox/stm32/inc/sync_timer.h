////////////////////////////////////////////////////////////////////
// This program sets Timer 3 and Timer 4 to act synchronously.
// Timer 4 is started by channel 4 of Timer 3 and channel 1, 2,
// and 3 of both timers generate PWM signals. The three outputs
// of Timer 3 are active-high, while that of Timer 4 are active-
// low.
//
// Function description
// --------------------
//
// SetSyncTimers:
//   Configure both timers and setup the timebase.
//   Output of PWMs are:
//   TIM3OC1 -> PB4
//   TIM3OC2 -> PB5
//   TIM3OC3 -> PB0
//   TIM4OC1 -> PD12
//   TIM4OC2 -> PD13
//   TIM4OC3 -> PD14
//   These could be reconfigured.
//
//   Parameter:
//   psc(0 ~ 65535)        The prescaler of timerbase
//
// SetPWMH:
//   Configure the three channels of Timer 3. Duty cycle is the
//   timer period of output-high.
//
//   Parameter:
//   arr(0 ~ 2^16 - 1)     The period of PWM
//   nccr1(0 ~ 2^16 - 1)    The duty cycle of PWM 1
//   nccr2(0 ~ 2^16 - 1)    The duty cycle of PWM 2
//   nccr3(0 ~ 2^16 - 1)    The duty cycle of PWM 3
//
// SetPWML:
//   Configure the three channels of Timer 4. Outputs are set
//   to be active-low. Duty cycle is the timer period of output-low.
//
//   Parameter:
//   arr(0 ~ 2^16 - 1)     The period of PWM
//   nccr1(0 ~ 2^16 - 1)    The duty cycle of PWM 1
//   nccr2(0 ~ 2^16 - 1)    The duty cycle of PWM 2
//   nccr3(0 ~ 2^16 - 1)    The duty cycle of PWM 3
//
// TimerStart:
//   Configure the timers to auto-reload, center-aligned and
//   enable the timers.
////////////////////////////////////////////////////////////////////

#ifndef __SYNC_TIMER_H
#define __SYNC_TIMER_H

#include "common_peripherals.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(SYNC_TIMER_LOW) && defined(SYNC_TIMER_HIGH)
  void SetSyncTimers();
  void SetPWM(uint16_t, uint16_t, uint16_t, uint16_t);
  void SetPWMH(uint16_t, uint16_t, uint16_t, uint16_t);
  void SetPWML(uint16_t, uint16_t, uint16_t, uint16_t);
  void SyncTimerCallback(void);
  void MAKENAME(TIM,MAKENAME(SYNC_TIMER_HIGH,_IRQHandler))(void);
#endif //defined(SYNC_TIMER_LOW) && defined(SYNC_TIMER_HIGH)

#ifdef __cplusplus
}
#endif // __cplusplus

#endif
