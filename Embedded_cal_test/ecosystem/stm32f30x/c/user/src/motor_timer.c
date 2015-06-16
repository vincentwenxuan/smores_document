#include "motor_timer.h"
#include STM32FXXX_RCC_H
#include STM32FXXX_TIM_H
#include STM32FXXX_GPIO_H
#include STM32FXXX_MISC_H

#if defined(MOTOR_TIMER)
void InitMotorTimer()
{
  //Initialize initialization structures
  GPIO_InitTypeDef GPIO_InitStructure;
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  // Enable timer (timers 1 and 8 are both on APB2)
  RCC_APB2PeriphClockCmd(MAKENAME(RCC_APB2Periph_TIM,MOTOR_TIMER), ENABLE);

  // Config options for channel outputs
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  
  // CH1 High GPIO Setup
  /* HIGH1 GPIO clock enable */
  RCC_AHBPeriphClockCmd(MAKENAME(RCC_AHBPeriph_GPIO,MOTOR_TIMER_HIGH1_PORT), ENABLE);
  
  /* HIGH1 GPIO config */
  GPIO_InitStructure.GPIO_Pin   = MAKENAME(GPIO_Pin_,MOTOR_TIMER_HIGH1_PIN);
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(MAKENAME(GPIO,MOTOR_TIMER_HIGH1_PORT), &GPIO_InitStructure);
  
  /* Connect TIM pin HIGH1 to AF */
  GPIO_PinAFConfig(MAKENAME(GPIO,MOTOR_TIMER_HIGH1_PORT), MAKENAME(GPIO_PinSource,MOTOR_TIMER_HIGH1_PIN), MAKENAME(GPIO_AF_,MOTOR_TIMER_HIGH1_AF));
  
  // CH1 Low GPIO Setup
  /* LOW1 GPIO clock enable */
  RCC_AHBPeriphClockCmd(MAKENAME(RCC_AHBPeriph_GPIO,MOTOR_TIMER_LOW1_PORT), ENABLE);
  
  /* LOW1 GPIO config */
  GPIO_InitStructure.GPIO_Pin   = MAKENAME(GPIO_Pin_,MOTOR_TIMER_LOW1_PIN);
  GPIO_Init(MAKENAME(GPIO,MOTOR_TIMER_LOW1_PORT), &GPIO_InitStructure);
  
  /* Connect TIM pin LOW1 to AF */
  GPIO_PinAFConfig(MAKENAME(GPIO,MOTOR_TIMER_LOW1_PORT), MAKENAME(GPIO_PinSource,MOTOR_TIMER_LOW1_PIN), MAKENAME(GPIO_AF_,MOTOR_TIMER_LOW1_AF));
  
#if MOTOR_TIMER_CHANNELS > 1
  // CH2 High GPIO Setup
  /* HIGH2 GPIO clock enable */
  RCC_AHBPeriphClockCmd(MAKENAME(RCC_AHBPeriph_GPIO,MOTOR_TIMER_HIGH2_PORT), ENABLE);
  
  /* HIGH2 GPIO config */
  GPIO_InitStructure.GPIO_Pin   = MAKENAME(GPIO_Pin_,MOTOR_TIMER_HIGH2_PIN);
  GPIO_Init(MAKENAME(GPIO,MOTOR_TIMER_HIGH2_PORT), &GPIO_InitStructure);
  
  /* Connect TIM pin HIGH2 to AF */
  GPIO_PinAFConfig(MAKENAME(GPIO,MOTOR_TIMER_HIGH2_PORT), MAKENAME(GPIO_PinSource,MOTOR_TIMER_HIGH2_PIN), MAKENAME(GPIO_AF_,MOTOR_TIMER_HIGH2_AF));
  
  // CH2 Low GPIO Setup
  /* LOW2 GPIO clock enable */
  RCC_AHBPeriphClockCmd(MAKENAME(RCC_AHBPeriph_GPIO,MOTOR_TIMER_LOW2_PORT), ENABLE);
  
  /* LOW2 GPIO config */
  GPIO_InitStructure.GPIO_Pin   = MAKENAME(GPIO_Pin_,MOTOR_TIMER_LOW2_PIN);
  GPIO_Init(MAKENAME(GPIO,MOTOR_TIMER_LOW2_PORT), &GPIO_InitStructure);
  
  /* Connect TIM pin LOW2 to AF */
  GPIO_PinAFConfig(MAKENAME(GPIO,MOTOR_TIMER_LOW2_PORT), MAKENAME(GPIO_PinSource,MOTOR_TIMER_LOW2_PIN), MAKENAME(GPIO_AF_,MOTOR_TIMER_LOW2_AF));
  
#if MOTOR_TIMER_CHANNELS > 2

  // CH3 High GPIO Setup
  /* HIGH3 GPIO clock enable */
  RCC_AHBPeriphClockCmd(MAKENAME(RCC_AHBPeriph_GPIO,MOTOR_TIMER_HIGH3_PORT), ENABLE);
  
  /* HIGH3 GPIO config */
  GPIO_InitStructure.GPIO_Pin   = MAKENAME(GPIO_Pin_,MOTOR_TIMER_HIGH3_PIN);
  GPIO_Init(MAKENAME(GPIO,MOTOR_TIMER_HIGH3_PORT), &GPIO_InitStructure);
  
  /* Connect TIM pin HIGH3 to AF */
  GPIO_PinAFConfig(MAKENAME(GPIO,MOTOR_TIMER_HIGH3_PORT), MAKENAME(GPIO_PinSource,MOTOR_TIMER_HIGH3_PIN), MAKENAME(GPIO_AF_,MOTOR_TIMER_HIGH3_AF));
  
  // CH3 Low GPIO Setup
  /* LOW3 GPIO clock enable */
  RCC_AHBPeriphClockCmd(MAKENAME(RCC_AHBPeriph_GPIO,MOTOR_TIMER_LOW3_PORT), ENABLE);
  
  /* LOW3 GPIO config */
  GPIO_InitStructure.GPIO_Pin   = MAKENAME(GPIO_Pin_,MOTOR_TIMER_LOW3_PIN);
  GPIO_Init(MAKENAME(GPIO,MOTOR_TIMER_LOW3_PORT), &GPIO_InitStructure);
  
  /* Connect TIM pin LOW3 to AF */
  GPIO_PinAFConfig(MAKENAME(GPIO,MOTOR_TIMER_LOW3_PORT), MAKENAME(GPIO_PinSource,MOTOR_TIMER_LOW3_PIN), MAKENAME(GPIO_AF_,MOTOR_TIMER_LOW3_AF));
#endif //MOTOR_TIMER_CHANNELS > 2
#endif //MOTOR_TIMER_CHANNELS > 1
  /* Time base configuration */
  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
  TIM_TimeBaseStructure.TIM_Prescaler         = 0;
  TIM_TimeBaseStructure.TIM_CounterMode       = TIM_CounterMode_CenterAligned3;
  TIM_TimeBaseStructure.TIM_Period            = 65535; // modified elsewhere
  TIM_TimeBaseStructure.TIM_ClockDivision     = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(MAKENAME(TIM,MOTOR_TIMER), &TIM_TimeBaseStructure);
  
  //////////////////////////////////////////////////////////////////////////////
  // Output Compare Configuration

  // common configuration parameters
  TIM_OCInitTypeDef  TIM_OCInitStructure;
  TIM_OCStructInit(&TIM_OCInitStructure);
  TIM_OCInitStructure.TIM_OCMode        = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState   = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OutputNState  = TIM_OutputNState_Enable;
  TIM_OCInitStructure.TIM_Pulse         = 0;

  // modify channel-by-channel parameters and initialize individual channels
  #if MOTOR_TIMER_CHANNELS == 1 || MOTOR_TIMER_CHANNELS == 2 || MOTOR_TIMER_CHANNELS == 3
    TIM_OCInitStructure.TIM_OCPolarity    = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_OCNPolarity   = TIM_OCNPolarity_Low;
    TIM_OCInitStructure.TIM_OCIdleState   = TIM_OCIdleState_Reset;
    TIM_OCInitStructure.TIM_OCNIdleState  = TIM_OCIdleState_Set;
    TIM_OC1Init(MAKENAME(TIM,MOTOR_TIMER), &TIM_OCInitStructure);
    TIM_OC1PreloadConfig(MAKENAME(TIM,MOTOR_TIMER),TIM_OCPreload_Enable);
  #endif

  #if MOTOR_TIMER_CHANNELS == 2 || MOTOR_TIMER_CHANNELS == 3
    TIM_OCInitStructure.TIM_OCPolarity    = TIM_OCPolarity_Low;
    TIM_OCInitStructure.TIM_OCNPolarity   = TIM_OCNPolarity_High;
    TIM_OCInitStructure.TIM_OCIdleState   = TIM_OCIdleState_Set;
    TIM_OCInitStructure.TIM_OCNIdleState  = TIM_OCIdleState_Reset;
    TIM_OC2Init(MAKENAME(TIM,MOTOR_TIMER), &TIM_OCInitStructure);
    TIM_OC2PreloadConfig(MAKENAME(TIM,MOTOR_TIMER),TIM_OCPreload_Enable);
  #endif

  #if MOTOR_TIMER_CHANNELS == 3
    TIM_OCInitStructure.TIM_OCPolarity    = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_OCNPolarity   = TIM_OCNPolarity_Low;
    TIM_OCInitStructure.TIM_OCIdleState   = TIM_OCIdleState_Reset;
    TIM_OCInitStructure.TIM_OCNIdleState  = TIM_OCIdleState_Set;
    TIM_OC3Init(MAKENAME(TIM,MOTOR_TIMER), &TIM_OCInitStructure);
    TIM_OC3PreloadConfig(MAKENAME(TIM,MOTOR_TIMER),TIM_OCPreload_Enable);
  #endif

  // Enable preload. All modifications to PWM frequency and duty are made 
  // through the preload shadow registers
  TIM_ARRPreloadConfig(MAKENAME(TIM,MOTOR_TIMER),ENABLE);


  //////////////////////////////////////////////////////////////////////////////
  // Dead Time Configuration
  // break and lock features are currently unused

  TIM_BDTRInitTypeDef TIM_BDTRInitStructure;
  TIM_BDTRStructInit(&TIM_BDTRInitStructure);
  TIM_BDTRInitStructure.TIM_DeadTime        = MOTOR_TIMER_DEADTIME;
  TIM_BDTRConfig(MAKENAME(TIM,MOTOR_TIMER), &TIM_BDTRInitStructure);
  
  //////////////////////////////////////////////////////////////////////////////
  // Interrupt Configuration

  #if MOTOR_TIMER==1
    NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_TIM16_IRQn;
  #elif MOTOR_TIMER==8
    NVIC_InitStructure.NVIC_IRQChannel = TIM8_UP_IRQn;
  #endif //MOTOR_TIMER==?
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  TIM_ITConfig(MAKENAME(TIM,MOTOR_TIMER), TIM_IT_Update, ENABLE);

  //////////////////////////////////////////////////////////////////////////////
  // Initialize and Enable

  TIM_SetCompare1(MAKENAME(TIM,MOTOR_TIMER), 0);
  TIM_SetCompare2(MAKENAME(TIM,MOTOR_TIMER), 0);
  TIM_SetCompare3(MAKENAME(TIM,MOTOR_TIMER), 0);
  
  TIM_Cmd(MAKENAME(TIM,MOTOR_TIMER), ENABLE);   // TIM1 counter enable
  TIM_CtrlPWMOutputs(MAKENAME(TIM,MOTOR_TIMER), ENABLE);   // Main Output Enable
}




#if MOTOR_TIMER_CHANNELS == 1
void SetMotorPWM(uint16_t arr, uint16_t dty1)
{
  MAKENAME(TIM,MOTOR_TIMER)->ARR = arr;
  
  if (arr < duty1)
  {
    MAKENAME(TIM,MOTOR_TIMER)->CCR1 = 0;
  }
  else
  {
    MAKENAME(TIM,MOTOR_TIMER)->CCR1 = dty1;
  }
}  
#elif MOTOR_TIMER_CHANNELS == 2
void SetMotorPWM(uint16_t arr, uint16_t dty1, uint16_t dty2)
{
  MAKENAME(TIM,MOTOR_TIMER)->ARR = arr;
  
  if (arr < duty1)
  {
    MAKENAME(TIM,MOTOR_TIMER)->CCR1 = 0;
  }
  else
  {
    MAKENAME(TIM,MOTOR_TIMER)->CCR1 = dty1;
  }
  if (arr < duty2)
  {
    MAKENAME(TIM,MOTOR_TIMER)->CCR2 = 0;
  }
  else
  {
    MAKENAME(TIM,MOTOR_TIMER)->CCR2 = dty2;
  }
}
#elif MOTOR_TIMER_CHANNELS == 3
void SetMotorPWM(uint16_t arr, uint16_t duty1, uint16_t duty2, uint16_t duty3)
{
  MAKENAME(TIM,MOTOR_TIMER)->ARR = arr;
  
  if (arr < duty1)
  {
    MAKENAME(TIM,MOTOR_TIMER)->CCR1 = 0;
  }
  else
  {
    MAKENAME(TIM,MOTOR_TIMER)->CCR1 = duty1;
  }
  if (arr < duty2)
  {
    MAKENAME(TIM,MOTOR_TIMER)->CCR2 = 0;
  }
  else
  {
    MAKENAME(TIM,MOTOR_TIMER)->CCR2 = duty2;
  }
  if (arr < duty3)
  {
    MAKENAME(TIM,MOTOR_TIMER)->CCR3 = 0;
  }
  else
  {
    MAKENAME(TIM,MOTOR_TIMER)->CCR3 = duty3;
  }
}
#endif //MOTOR_TIMER_CHANNELS == ?

#if MOTOR_TIMER == 1
void TIM1_UP_TIM16_IRQHandler(void)
#elif MOTOR_TIMER == 8
void TIM8_UP_IRQHandler(void)
#endif //MOTOR_TIMER == ?
{
  if (TIM_GetITStatus(MAKENAME(TIM,MOTOR_TIMER), TIM_IT_Update) != RESET) {
    TIM_ClearITPendingBit(MAKENAME(TIM,MOTOR_TIMER), TIM_IT_Update);
  }
  MotorTimerCallback();
}

#endif //defined(MOTOR_TIMER)