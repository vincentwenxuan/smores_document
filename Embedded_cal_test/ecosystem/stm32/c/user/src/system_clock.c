#include "common_peripherals.h"
#include "system_clock.h"

#include STM32FXXX_H
#include STM32FXXX_MISC_H
#include STM32FXXX_TIM_H
#include STM32FXXX_RCC_H

#ifdef USING_SYSTEM_CLOCK

struct Time global_system_time_stamp;

#define RELOAD_VALUE     0xFFFF
#define TICK_DURATION_NS 125
#define TICK_PRESCALE    8

////////////////////////////////////////////////////////////////////////////////
//  Initialization

void InitSystemClock() {

  // initialize state
  global_system_time_stamp.sec  = 0;
  global_system_time_stamp.nsec = 0;

  //////////////////////////////////////////////////////////////////////////////
  // Enable Required TIM Peripheral Clock
  #if defined(STM32F303xC)
    #if SYSTEM_CLOCK_TIMER == 2 || SYSTEM_CLOCK_TIMER == 3 || SYSTEM_CLOCK_TIMER == 4 || SYSTEM_CLOCK_TIMER == 6 || SYSTEM_CLOCK_TIMER == 7
      RCC_APB1PeriphClockCmd(MAKENAME(RCC_APB1Periph_TIM,SYSTEM_CLOCK_TIMER), ENABLE);
    #elif SYSTEM_CLOCK_TIMER == 1 || SYSTEM_CLOCK_TIMER == 8 || SYSTEM_CLOCK_TIMER == 15 || SYSTEM_CLOCK_TIMER == 16 || SYSTEM_CLOCK_TIMER == 17
      RCC_APB1PeriphClockCmd(MAKENAME(RCC_APB2Periph_TIM,SYSTEM_CLOCK_TIMER), ENABLE);
    #endif

  #elif defined(STM32F37X)
    #if SYSTEM_CLOCK_TIMER == 15 || SYSTEM_CLOCK_TIMER == 16 || SYSTEM_CLOCK_TIMER == 17 || SYSTEM_CLOCK_TIMER == 19
      RCC_APB2PeriphClockCmd(MAKENAME(RCC_APB2Periph_TIM,SYSTEM_CLOCK_TIMER), ENABLE);
    #else
      RCC_APB1PeriphClockCmd(MAKENAME(RCC_APB1Periph_TIM,SYSTEM_CLOCK_TIMER), ENABLE);
    #endif
  #else
    #error("Unrecognized target. The Makefile should define one of the following: {STM32F37X, STM32F303xC}")
  #endif

  //////////////////////////////////////////////////////////////////////////////
  // Configured timer for up-counting to RELOAD_VALUE with a prescale of 
  // TICK_PRESCALE to achieve ticks with duration TICK_DURATION_NS.
  TIM_TimeBaseInitTypeDef tb;
  TIM_TimeBaseStructInit(&tb);
  tb.TIM_Prescaler          = TICK_PRESCALE;
  tb.TIM_CounterMode        = TIM_CounterMode_Up;
  tb.TIM_Period             = RELOAD_VALUE;
  tb.TIM_ClockDivision      = TIM_CKD_DIV1;
  tb.TIM_RepetitionCounter  = 0;  // unused
  TIM_TimeBaseInit( MAKENAME(TIM,SYSTEM_CLOCK_TIMER), &tb);

  //////////////////////////////////////////////////////////////////////////////
  // Configure Interrupt on Update Event
  NVIC_InitTypeDef NVIC_InitStructure;
  #if defined(STM32F303xC)
    #if   SYSTEM_CLOCK_TIMER == 6
      NVIC_InitStructure.NVIC_IRQChannel = TIM6_DAC_IRQn;
    #elif (SYSTEM_CLOCK_TIMER == 2 || SYSTEM_CLOCK_TIMER == 7)
      NVIC_InitStructure.NVIC_IRQChannel = MAKENAME(TIM,MAKENAME(SYSTEM_CLOCK_TIMER,_IRQn));
    #else
      #error("SYSTEM_CLOCK_TIMER choice in project_peripherals.h not supported.")
    #endif
  #elif defined(STM32F37X)
    #if   SYSTEM_CLOCK_TIMER == 6
      NVIC_InitStructure.NVIC_IRQChannel = TIM6_DAC1_IRQn;
    #elif (SYSTEM_CLOCK_TIMER == 2 || SYSTEM_CLOCK_TIMER == 5 || SYSTEM_CLOCK_TIMER == 7)
      NVIC_InitStructure.NVIC_IRQChannel = MAKENAME(TIM,MAKENAME(SYSTEM_CLOCK_TIMER,_IRQn));
    #else
      #error("SYSTEM_CLOCK_TIMER choice in project_peripherals.h not supported.")
    #endif
  #else
    #error("Unrecognized target. The Makefile should define one of the following: {STM32F37X, STM32F303xC}")
  #endif
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 15; // must have priority in system
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  //////////////////////////////////////////////////////////////////////////////
  // Enable Interrupt
  TIM_ITConfig(MAKENAME(TIM,SYSTEM_CLOCK_TIMER), TIM_IT_Update, ENABLE);

  //////////////////////////////////////////////////////////////////////////////
  // Enable the Counter
  TIM_Cmd( MAKENAME(TIM,SYSTEM_CLOCK_TIMER), ENABLE);

}

////////////////////////////////////////////////////////////////////////////////
//  Interrupt Handler

#if defined(STM32F303xC)
  #if SYSTEM_CLOCK_TIMER == 6
    #define SYSTEM_CLOCK_IRQHandler TIM6_DAC_IRQHandler
  #elif (SYSTEM_CLOCK_TIMER == 2 || SYSTEM_CLOCK_TIMER == 7)
    #define SYSTEM_CLOCK_IRQHandler MAKENAME(TIM,MAKENAME(SYSTEM_CLOCK_TIMER,_IRQHandler))
  #else
    #error("SYSTEM_CLOCK_TIMER choice in project_peripherals.h not supported.")
  #endif
#elif defined(STM32F37X)
  #if SYSTEM_CLOCK_TIMER == 6
    #define SYSTEM_CLOCK_IRQHandler TIM6_DAC1_IRQHandler
  #elif (SYSTEM_CLOCK_TIMER == 2 || SYSTEM_CLOCK_TIMER == 5 || SYSTEM_CLOCK_TIMER == 7)
    #define SYSTEM_CLOCK_IRQHandler MAKENAME(TIM,MAKENAME(SYSTEM_CLOCK_TIMER,_IRQHandler))
  #else
    #error("SYSTEM_CLOCK_TIMER choice in project_peripherals.h not supported.")
  #endif
#else
  #error("Unrecognized target. The Makefile should define one of the following: {STM32F37X, STM32F303xC}")
#endif

void SYSTEM_CLOCK_IRQHandler(void) {
  if (TIM_GetITStatus( MAKENAME(TIM,SYSTEM_CLOCK_TIMER), TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit( MAKENAME(TIM,SYSTEM_CLOCK_TIMER), TIM_IT_Update);
    global_system_time_stamp.nsec += TICK_DURATION_NS * RELOAD_VALUE;
    if(global_system_time_stamp.nsec >= 1e9) {
      global_system_time_stamp.nsec -= 1e9;
      global_system_time_stamp.sec += 1;
    }

  }
}

////////////////////////////////////////////////////////////////////////////////
//  Get Time

struct Time GetTime() {
  struct Time retval; // return value in seconds, nanoseconds
  int32_t raw_ticks;  // temporary copy of raw timer counter

  // compute time from raw timer counter and previous time stamp
  raw_ticks = TIM_GetCounter( MAKENAME(TIM,SYSTEM_CLOCK_TIMER));
  retval.nsec = global_system_time_stamp.nsec + TICK_DURATION_NS * raw_ticks;
  retval.sec = global_system_time_stamp.sec;
  if(retval.nsec >= 1e9) {
    retval.nsec -= 1e9;
    retval.sec += 1;
  }

  // recheck the raw tick count from the timer
  // if it is smaller than before, the timer overflowed during our calculation
  // and the calculation needs to be repeated
  if(TIM_GetCounter( MAKENAME(TIM,SYSTEM_CLOCK_TIMER)) < raw_ticks) {
    raw_ticks = TIM_GetCounter( MAKENAME(TIM,SYSTEM_CLOCK_TIMER));
    retval.nsec = global_system_time_stamp.nsec + TICK_DURATION_NS * raw_ticks;
    retval.sec = global_system_time_stamp.sec;
    if(retval.nsec >= 1e9) {
      retval.nsec -= 1e9;
      retval.sec += 1;
    }
  }

  return retval;
}




#endif // USING_SYSTEM_CLOCK