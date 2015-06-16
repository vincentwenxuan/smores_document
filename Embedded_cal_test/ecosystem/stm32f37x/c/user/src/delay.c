#include <stdint.h>
#include "stm32f37x.h"
#include "core_cm4.h"
#include "systick.h"

void _delay_us(int us)
{
  if (us < 0)
    us = 0;
  if (us > 999999)
    us = 999999;

  int32_t tentry  = SysTick->VAL;
  int32_t ntics = us*(SYSTICK_NUM_TICS_PER_SECOND/1000000);

  int32_t tstop = tentry - ntics;  //down counting

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

    while( tentry >= tnow )  //wait for overflow
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

void _delay_ms(int ms)
{
  if(ms < 0)
    return;
  if(ms > 999)
    ms = 999;

  _delay_us(ms*1000);
}
