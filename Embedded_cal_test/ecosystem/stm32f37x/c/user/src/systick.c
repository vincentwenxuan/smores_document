#include "systick.h"
#include "common_peripherals.h"
#include STM32FXXX_H
#include STM32FXXX_MISC_H
#include "core_cm4.h"

void  systick_enable();
void  systick_disable();
int   systick_int_enable();
int   systick_int_disable();
void  systick_reset();
int   systick_get_val();

void systick_init() {
  // choose full speed clock source
  SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);  

  // set reload value
  #if (SYSTICK_RELOAD_VALUE > SysTick_LOAD_RELOAD_Msk)
    #error "SYSTICK_RELOAD_VALUE max value is 2^24"
  #endif
  SysTick->LOAD  = (SYSTICK_RELOAD_VALUE & SysTick_LOAD_RELOAD_Msk) - 1;

  // clear counter and then enable
  systick_reset();
  systick_enable();

  NVIC_SetPriority (SysTick_IRQn, (1<<__NVIC_PRIO_BITS) - 1);
}

void systick_enable() {
  SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}

void systick_disable() {
  SysTick->CTRL &= ~(SysTick_CTRL_ENABLE_Msk);
}

int systick_int_enable() {
  SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
  return 0;
}

int systick_int_disable() {
  SysTick->CTRL &= ~(SysTick_CTRL_TICKINT_Msk);
  return 0;
}

void systick_reset() {
  SysTick->VAL   = 0;
}

int systick_get_val() {
  return SysTick->VAL;
}

void systick_block(uint32_t nticks) {
  int32_t tentry  = SysTick->VAL;
  int32_t tstop = tentry - nticks;  // down-counting

  int32_t tnow;
  int32_t diff;

  if (tstop >= 0)
  {
    tnow = SysTick->VAL;
    diff = tnow - tstop;
    while( (diff > 0) && (tentry >= tnow) )
    {
      tnow = SysTick->VAL;
      diff = tnow - tstop;
    }
  }
  else
  {
    tstop += SYSTICK_RELOAD_VALUE;
    tnow  = SysTick->VAL;

    while( tentry >= tnow )  // wait for overflow
    {
      tnow  = SysTick->VAL;
    }

    tnow = SysTick->VAL;
    diff = tnow - tstop;
    while( (diff > 0) && (tentry < tnow) )
    {
      tnow = SysTick->VAL;
      diff = tnow - tstop;
    }
  }
}