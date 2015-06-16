#ifndef TIMEBASE_H
#define TIMEBASE_H

#include "common_peripherals.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef TIMEBASE_TIMER

void initTimeBase(void);

void TIMER_INTERRUPT_FCN(void);

void PauseTimebase(void);

void PlayTimebase(void);

#if TIMEBASE_TIMER == 6 || TIMEBASE_TIMER == 7 || TIMEBASE_TIMER == 18
  void timerCallback1(void);
#endif

#if TIMEBASE_TIMER != 6 && TIMEBASE_TIMER != 7 && TIMEBASE_TIMER != 18
  void timerCallback1(void);
#if defined(TIMEBASE_INT2_FREQ) && TIMEBASE_TIMER != 13 && TIMEBASE_TIMER != 14 && TIMEBASE_TIMER != 16 && TIMEBASE_TIMER != 17
  void timerCallback2(void);
#if defined(TIMEBASE_INT3_FREQ) && TIMEBASE_TIMER != 12 && TIMEBASE_TIMER != 15
  void timerCallback3(void);
#ifdef TIMEBASE_INT4_FREQ
  void timerCallback4(void);
#endif //TIMEBASE_INT4_FREQ
#endif //defined(TIMEBASE_INT3_FREQ) && TIMEBASE_TIMER != 12 && TIMEBASE_TIMER != 15
#endif //defined(TIMEBASE_INT2_FREQ) && TIMEBASE_TIMER != 13 && TIMEBASE_TIMER != 14 && TIMEBASE_TIMER != 16 && TIMEBASE_TIMER != 17
#endif //TIMEBASE_TIMER != 6 || TIMEBASE_TIMER != 7 || TIMEBASE_TIMER != 18

#endif //TIMEBASE_TIMER

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // #ifndef TIMEBASE_H