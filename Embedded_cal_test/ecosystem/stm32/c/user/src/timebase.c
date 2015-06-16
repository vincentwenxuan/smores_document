#include "common_peripherals.h"
#include "timebase.h"
#include STM32FXXX_H
#include STM32FXXX_MISC_H
#include STM32FXXX_TIM_H
#include STM32FXXX_RCC_H
#include "error_reporting.h"
#include <stdio.h>

#ifdef TIMEBASE_TIMER

void initTimeBase(void)
{
  uint16_t PrescalerValue = 0;

  #if defined(TIMEBASE_INT1_FREQ)
    if ((SystemCoreClock/TIMEBASE_CLOCK_PSC  / TIMEBASE_INT1_FREQ - 1) < 1)
    {
      printf("TIMEBASE period 1 is too large.  Increase frequency or decrease clock.");
      ErrorTrap(ERROR_TIMEBASE_CONFIG);
    }
  #endif
  #if defined(TIMEBASE_INT2_FREQ)
    if ((SystemCoreClock/TIMEBASE_CLOCK_PSC  / TIMEBASE_INT2_FREQ - 1) < 1)
    {
      printf("TIMEBASE period 2 is too large.  Increase frequency or decrease clock.");
      ErrorTrap(ERROR_TIMEBASE_CONFIG);
    }
  #endif
  #if defined(TIMEBASE_INT3_FREQ)
    if ((SystemCoreClock/TIMEBASE_CLOCK_PSC  / TIMEBASE_INT3_FREQ - 1) < 1)
    {
      printf("TIMEBASE period 3 is too large.  Increase frequency or decrease clock.");
      ErrorTrap(ERROR_TIMEBASE_CONFIG);
    }
  #endif
  #if defined(TIMEBASE_INT4_FREQ)
    if ((SystemCoreClock/TIMEBASE_CLOCK_PSC  / TIMEBASE_INT4_FREQ - 1) < 1)
    {
      printf("TIMEBASE period 4 is too large.  Increase frequency or decrease clock.");
      ErrorTrap(ERROR_TIMEBASE_CONFIG);
    }
  #endif
  NVIC_InitTypeDef NVIC_InitStructure;
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;

  /* TIM clock enable */
  #if defined(STM32F303xC)
    #if TIMEBASE_TIMER == 2 || TIMEBASE_TIMER == 3 || TIMEBASE_TIMER == 4 || TIMEBASE_TIMER == 6 || TIMEBASE_TIMER == 7
      RCC_APB1PeriphClockCmd(MAKENAME(RCC_APB1Periph_TIM,TIMEBASE_TIMER), ENABLE);
    #elif TIMEBASE_TIMER == 1 || TIMEBASE_TIMER == 8 || TIMEBASE_TIMER == 15 || TIMEBASE_TIMER == 16 || TIMEBASE_TIMER == 17
      RCC_APB2PeriphClockCmd(MAKENAME(RCC_APB2Periph_TIM,TIMEBASE_TIMER), ENABLE);
    #endif
  #elif defined(STM32F37X)
    #if TIMEBASE_TIMER == 15 || TIMEBASE_TIMER == 16 || TIMEBASE_TIMER == 17 || TIMEBASE_TIMER == 19
      RCC_APB2PeriphClockCmd(MAKENAME(RCC_APB2Periph_TIM,TIMEBASE_TIMER), ENABLE);
    #else
      RCC_APB1PeriphClockCmd(MAKENAME(RCC_APB1Periph_TIM,TIMEBASE_TIMER), ENABLE);
    #endif
  #else
    #error("Unrecognized target. The Makefile should define one of the following: {STM32F37X, STM32F303xC}")
  #endif

  /* Enable the TIM gloabal Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = MAKENAME(TIMER_INTERRUPT,n);
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  /* Compute the prescaler value */
  PrescalerValue = (uint16_t) (TIMEBASE_CLOCK_PSC - 1);

  /* Time base configuration */
  #if TIMEBASE_TIMER == 6 || TIMEBASE_TIMER == 7 || TIMEBASE_TIMER == 18
  TIM_TimeBaseStructure.TIM_Period = (TIMEBASE_SIZE) (SystemCoreClock/TIMEBASE_CLOCK_PSC  / TIMEBASE_INT1_FREQ - 1);
  /* TIM Interrupts enable */
  TIM_ITConfig(MAKENAME(TIM,TIMEBASE_TIMER), TIM_IT_Update, ENABLE);
  #else
  #if TIMEBASE_TIMER == 2 || TIMEBASE_TIMER == 5
  TIM_TimeBaseStructure.TIM_Period = 4294967295;
  #else
  TIM_TimeBaseStructure.TIM_Period = 65535;
  #endif
  #endif
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(MAKENAME(TIM,TIMEBASE_TIMER), &TIM_TimeBaseStructure);

  /* Prescaler configuration */
  TIM_PrescalerConfig(MAKENAME(TIM,TIMEBASE_TIMER), PrescalerValue, TIM_PSCReloadMode_Immediate);
  
  #if TIMEBASE_TIMER != 6 && TIMEBASE_TIMER != 7 && TIMEBASE_TIMER != 18
  /* Output Compare Timing Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = (TIMEBASE_SIZE) (SystemCoreClock/TIMEBASE_CLOCK_PSC  / TIMEBASE_INT1_FREQ - 1);
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

  TIM_OC1Init(MAKENAME(TIM,TIMEBASE_TIMER), &TIM_OCInitStructure);

  TIM_OC1PreloadConfig(MAKENAME(TIM,TIMEBASE_TIMER), TIM_OCPreload_Disable);
  
  /* TIM Interrupts enable */
  TIM_ITConfig(MAKENAME(TIM,TIMEBASE_TIMER), TIM_IT_CC1, ENABLE);
  
  #if defined(TIMEBASE_INT2_FREQ) && TIMEBASE_TIMER != 13 && TIMEBASE_TIMER != 14 && TIMEBASE_TIMER != 16 && TIMEBASE_TIMER != 17
  /* Output Compare Timing Mode configuration: Channel2 */
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = (TIMEBASE_SIZE) (SystemCoreClock/TIMEBASE_CLOCK_PSC  / TIMEBASE_INT2_FREQ - 1);

  TIM_OC2Init(MAKENAME(TIM,TIMEBASE_TIMER), &TIM_OCInitStructure);

  TIM_OC2PreloadConfig(MAKENAME(TIM,TIMEBASE_TIMER), TIM_OCPreload_Disable);
  
  /* TIM Interrupts enable */
  TIM_ITConfig(MAKENAME(TIM,TIMEBASE_TIMER), TIM_IT_CC2, ENABLE);

  #if defined(TIMEBASE_INT3_FREQ) && TIMEBASE_TIMER != 12 && TIMEBASE_TIMER != 15
  /* Output Compare Timing Mode configuration: Channel3 */
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = (TIMEBASE_SIZE) (SystemCoreClock/TIMEBASE_CLOCK_PSC  / TIMEBASE_INT3_FREQ - 1);

  TIM_OC3Init(MAKENAME(TIM,TIMEBASE_TIMER), &TIM_OCInitStructure);

  TIM_OC3PreloadConfig(MAKENAME(TIM,TIMEBASE_TIMER), TIM_OCPreload_Disable);
  
  /* TIM Interrupts enable */
  TIM_ITConfig(MAKENAME(TIM,TIMEBASE_TIMER),TIM_IT_CC3, ENABLE);
  
  #ifdef TIMEBASE_INT4_FREQ
  /* Output Compare Timing Mode configuration: Channel4 */
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = (TIMEBASE_SIZE) (SystemCoreClock/TIMEBASE_CLOCK_PSC  / TIMEBASE_INT4_FREQ - 1);

  TIM_OC4Init(MAKENAME(TIM,TIMEBASE_TIMER), &TIM_OCInitStructure);

  TIM_OC4PreloadConfig(MAKENAME(TIM,TIMEBASE_TIMER), TIM_OCPreload_Disable);
   
  /* TIM Interrupts enable */
  TIM_ITConfig(MAKENAME(TIM,TIMEBASE_TIMER),TIM_IT_CC4, ENABLE);
  #endif //TIMEBASE_INT4_FREQ
  #endif //defined(TIMEBASE_INT3_FREQ) && TIMEBASE_TIMER != 12 && TIMEBASE_TIMER != 15
  #endif //defined(TIMEBASE_INT2_FREQ) && TIMEBASE_TIMER != 13 && TIMEBASE_TIMER != 14 && TIMEBASE_TIMER != 16 && TIMEBASE_TIMER != 17
  #endif //TIMEBASE_TIMER != 6 || TIMEBASE_TIMER != 7 || TIMEBASE_TIMER != 18
  /* TIM enable counter */
  //TIM_Cmd(MAKENAME(TIM,TIMEBASE_TIMER), ENABLE); //Don't do this here, require a PlayTimebase call! Everyone loves PlayTime...base.  All your base belong to PlayTime?
}

void PauseTimebase(void)
{
  TIM_Cmd(MAKENAME(TIM,TIMEBASE_TIMER), DISABLE);
}

void PlayTimebase(void)
{
  TIM_Cmd(MAKENAME(TIM,TIMEBASE_TIMER), ENABLE);
}
  

void TIMER_INTERRUPT_FCN(void)
{
  #if TIMEBASE_TIMER == 6 || TIMEBASE_TIMER == 7 || TIMEBASE_TIMER == 18
  if (TIM_GetITStatus(MAKENAME(TIM,TIMEBASE_TIMER), TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(MAKENAME(TIM,TIMEBASE_TIMER), TIM_IT_Update);
    timerCallback1();
  }
  #endif //TIMEBASE_TIMER == 6 || TIMEBASE_TIMER == 7 || TIMEBASE_TIMER == 18
  
  #if TIMEBASE_TIMER != 6 && TIMEBASE_TIMER != 7 && TIMEBASE_TIMER != 18
  #ifdef TIMEBASE_SIZE
  TIMEBASE_SIZE capture_val = 0;
  TIMEBASE_SIZE next_capture_val = 0;
  TIMEBASE_SIZE clock_val = 0;
  TIMEBASE_SIZE gap_val = 0;
  #endif
  
  if (TIM_GetITStatus(MAKENAME(TIM,TIMEBASE_TIMER), TIM_IT_CC1) != RESET)
  {
    TIM_ClearITPendingBit(MAKENAME(TIM,TIMEBASE_TIMER), TIM_IT_CC1);
    timerCallback1();
	  capture_val = TIM_GetCapture1(MAKENAME(TIM,TIMEBASE_TIMER));
    clock_val = TIM_GetCounter(MAKENAME(TIM,TIMEBASE_TIMER));
    gap_val = (TIMEBASE_SIZE) (SystemCoreClock/TIMEBASE_CLOCK_PSC  / TIMEBASE_INT1_FREQ - 1);
    next_capture_val = capture_val + gap_val;
    while( (((clock_val > next_capture_val) || (clock_val < capture_val)) && (capture_val < next_capture_val)) || ((clock_val > next_capture_val) && (clock_val < capture_val)) )
    {
      //increment capture and next_capture by gap_val
      capture_val += gap_val;
      next_capture_val = capture_val + gap_val;
      clock_val = TIM_GetCounter(MAKENAME(TIM,TIMEBASE_TIMER));
    }
    TIM_SetCompare1(MAKENAME(TIM,TIMEBASE_TIMER), next_capture_val);
  }
  #if defined(TIMEBASE_INT2_FREQ) && TIMEBASE_TIMER != 13 && TIMEBASE_TIMER != 14 && TIMEBASE_TIMER != 16 && TIMEBASE_TIMER != 17
  if (TIM_GetITStatus(MAKENAME(TIM,TIMEBASE_TIMER), TIM_IT_CC2) != RESET)
  {
    TIM_ClearITPendingBit(MAKENAME(TIM,TIMEBASE_TIMER), TIM_IT_CC2);
    timerCallback2();
    capture_val = TIM_GetCapture2(MAKENAME(TIM,TIMEBASE_TIMER));
    clock_val = TIM_GetCounter(MAKENAME(TIM,TIMEBASE_TIMER));
    gap_val = (TIMEBASE_SIZE) (SystemCoreClock/TIMEBASE_CLOCK_PSC  / TIMEBASE_INT2_FREQ - 1);
    next_capture_val = capture_val + gap_val;
    while( (((clock_val > next_capture_val) || (clock_val < capture_val)) && (capture_val < next_capture_val)) || ((clock_val > next_capture_val) && (clock_val < capture_val)) )
    {
      //increment capture and next_capture by gap_val
      capture_val += gap_val;
      next_capture_val = capture_val + gap_val;
      clock_val = TIM_GetCounter(MAKENAME(TIM,TIMEBASE_TIMER));
    }
    TIM_SetCompare2(MAKENAME(TIM,TIMEBASE_TIMER), next_capture_val);
  }
  #if defined(TIMEBASE_INT3_FREQ) && TIMEBASE_TIMER != 12 && TIMEBASE_TIMER != 15
  if (TIM_GetITStatus(MAKENAME(TIM,TIMEBASE_TIMER), TIM_IT_CC3) != RESET)
  {
    TIM_ClearITPendingBit(MAKENAME(TIM,TIMEBASE_TIMER), TIM_IT_CC3);
    timerCallback3();
    capture_val = TIM_GetCapture3(MAKENAME(TIM,TIMEBASE_TIMER));
    clock_val = TIM_GetCounter(MAKENAME(TIM,TIMEBASE_TIMER));
    gap_val = (TIMEBASE_SIZE) (SystemCoreClock/TIMEBASE_CLOCK_PSC  / TIMEBASE_INT3_FREQ - 1);
    next_capture_val = capture_val + gap_val;
    while( (((clock_val > next_capture_val) || (clock_val < capture_val)) && (capture_val < next_capture_val)) || ((clock_val > next_capture_val) && (clock_val < capture_val)) )
    {
      //increment capture and next_capture by gap_val
      capture_val += gap_val;
      next_capture_val = capture_val + gap_val;
      clock_val = TIM_GetCounter(MAKENAME(TIM,TIMEBASE_TIMER));
    }
    TIM_SetCompare3(MAKENAME(TIM,TIMEBASE_TIMER), next_capture_val);
  }
  #ifdef TIMEBASE_INT4_FREQ
  if (TIM_GetITStatus(MAKENAME(TIM,TIMEBASE_TIMER), TIM_IT_CC4) != RESET)
  {
    TIM_ClearITPendingBit(MAKENAME(TIM,TIMEBASE_TIMER), TIM_IT_CC4);
    timerCallback4();
    capture_val = TIM_GetCapture4(MAKENAME(TIM,TIMEBASE_TIMER));
    clock_val = TIM_GetCounter(MAKENAME(TIM,TIMEBASE_TIMER));
    gap_val = (TIMEBASE_SIZE) (SystemCoreClock/TIMEBASE_CLOCK_PSC  / TIMEBASE_INT4_FREQ - 1);
    next_capture_val = capture_val + gap_val;
    while( (((clock_val > next_capture_val) || (clock_val < capture_val)) && (capture_val < next_capture_val)) || ((clock_val > next_capture_val) && (clock_val < capture_val)) )
    {
      //increment capture and next_capture by gap_val
      capture_val += gap_val;
      next_capture_val = capture_val + gap_val;
      clock_val = TIM_GetCounter(MAKENAME(TIM,TIMEBASE_TIMER));
    }
    TIM_SetCompare4(MAKENAME(TIM,TIMEBASE_TIMER), next_capture_val);
  }
  #endif //TIMEBASE_INT4_FREQ
  #endif //defined(TIMEBASE_INT3_FREQ) && TIMEBASE_TIMER != 12 && TIMEBASE_TIMER != 15
  #endif //defined(TIMEBASE_INT2_FREQ) && TIMEBASE_TIMER != 13 && TIMEBASE_TIMER != 14 && TIMEBASE_TIMER != 16 && TIMEBASE_TIMER != 17
  #endif //TIMEBASE_TIMER != 6 || TIMEBASE_TIMER != 7 || TIMEBASE_TIMER != 18

}
#endif //TIMEBASE_TIMER
