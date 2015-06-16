//////////////////////////////////////////////////////////////////////
// This program sets two timers to act synchronously.
// This allows the control of a three phase inverter or dual H-bridges
// TODO:: Only supports 3 half-Hs.  Add the 4th.  Make it a variable #

#include "sync_timer.h"
#include STM32FXXX_RCC_H
#include STM32FXXX_TIM_H
#include STM32FXXX_GPIO_H
#include STM32FXXX_MISC_H
#include "common_peripherals.h"
#include <stdint.h>

#if defined(SYNC_TIMER_LOW) && defined(SYNC_TIMER_HIGH)
void SetSyncTimers(/*uint16_t psc*/){
  //Initialize initialization structures
  GPIO_InitTypeDef GPIO_InitStructure;
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  
  // Enable high timer
  #if SYNC_TIMER_HIGH == 19
    RCC_APB2PeriphClockCmd(MAKENAME(RCC_APB2Periph_TIM,SYNC_TIMER_HIGH), ENABLE);
  #else
    RCC_APB1PeriphClockCmd(MAKENAME(RCC_APB1Periph_TIM,SYNC_TIMER_HIGH), ENABLE);
  #endif
  
  // Enable low timer
  #if SYNC_TIMER_LOW == 19
    RCC_APB2PeriphClockCmd(MAKENAME(RCC_APB2Periph_TIM,SYNC_TIMER_LOW), ENABLE);
  #else
    RCC_APB1PeriphClockCmd(MAKENAME(RCC_APB1Periph_TIM,SYNC_TIMER_LOW), ENABLE);
  #endif
  
  /* HIGH1 GPIO clock enable */
  RCC_AHBPeriphClockCmd(MAKENAME(RCC_AHBPeriph_GPIO,SYNC_TIMER_HIGH1_PORT), ENABLE);
  
  /* HIGH1 GPIO config */
  GPIO_InitStructure.GPIO_Pin   = MAKENAME(GPIO_Pin_,SYNC_TIMER_HIGH1_PIN);
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(MAKENAME(GPIO,SYNC_TIMER_HIGH1_PORT), &GPIO_InitStructure);
  
  /* Connect TIM pin HIGH1 to AF */
  GPIO_PinAFConfig(MAKENAME(GPIO,SYNC_TIMER_HIGH1_PORT), MAKENAME(GPIO_PinSource,SYNC_TIMER_HIGH1_PIN), MAKENAME(GPIO_AF_,SYNC_TIMER_HIGH1_AF));
  
  /* HIGH2 GPIO clock enable */
  RCC_AHBPeriphClockCmd(MAKENAME(RCC_AHBPeriph_GPIO,SYNC_TIMER_HIGH2_PORT), ENABLE);
  
  /* HIGH2 GPIO config */
  GPIO_InitStructure.GPIO_Pin   = MAKENAME(GPIO_Pin_,SYNC_TIMER_HIGH2_PIN);
  GPIO_Init(MAKENAME(GPIO,SYNC_TIMER_HIGH2_PORT), &GPIO_InitStructure);
  
  /* Connect TIM pin HIGH2 to AF */
  GPIO_PinAFConfig(MAKENAME(GPIO,SYNC_TIMER_HIGH2_PORT), MAKENAME(GPIO_PinSource,SYNC_TIMER_HIGH2_PIN), MAKENAME(GPIO_AF_,SYNC_TIMER_HIGH2_AF));
  
  /* HIGH3 GPIO clock enable */
  RCC_AHBPeriphClockCmd(MAKENAME(RCC_AHBPeriph_GPIO,SYNC_TIMER_HIGH3_PORT), ENABLE);
  
  /* HIGH3 GPIO config */
  GPIO_InitStructure.GPIO_Pin   = MAKENAME(GPIO_Pin_,SYNC_TIMER_HIGH3_PIN);
  GPIO_Init(MAKENAME(GPIO,SYNC_TIMER_HIGH3_PORT), &GPIO_InitStructure);
  
  /* Connect TIM pin HIGH3 to AF */
  GPIO_PinAFConfig(MAKENAME(GPIO,SYNC_TIMER_HIGH3_PORT), MAKENAME(GPIO_PinSource,SYNC_TIMER_HIGH3_PIN), MAKENAME(GPIO_AF_,SYNC_TIMER_HIGH3_AF));
  
  /* LOW1 GPIO clock enable */
  RCC_AHBPeriphClockCmd(MAKENAME(RCC_AHBPeriph_GPIO,SYNC_TIMER_LOW1_PORT), ENABLE);
  
  /* LOW1 GPIO config */
  GPIO_InitStructure.GPIO_Pin   = MAKENAME(GPIO_Pin_,SYNC_TIMER_LOW1_PIN);
  GPIO_Init(MAKENAME(GPIO,SYNC_TIMER_LOW1_PORT), &GPIO_InitStructure);
  
  /* Connect TIM pin LOW1 to AF */
  GPIO_PinAFConfig(MAKENAME(GPIO,SYNC_TIMER_LOW1_PORT), MAKENAME(GPIO_PinSource,SYNC_TIMER_LOW1_PIN), MAKENAME(GPIO_AF_,SYNC_TIMER_LOW1_AF));
  
  /* LOW2 GPIO clock enable */
  RCC_AHBPeriphClockCmd(MAKENAME(RCC_AHBPeriph_GPIO,SYNC_TIMER_LOW2_PORT), ENABLE);
  
  /* LOW2 GPIO config */
  GPIO_InitStructure.GPIO_Pin   = MAKENAME(GPIO_Pin_,SYNC_TIMER_LOW2_PIN);
  GPIO_Init(MAKENAME(GPIO,SYNC_TIMER_LOW2_PORT), &GPIO_InitStructure);
  
  /* Connect TIM pin LOW2 to AF */
  GPIO_PinAFConfig(MAKENAME(GPIO,SYNC_TIMER_LOW2_PORT), MAKENAME(GPIO_PinSource,SYNC_TIMER_LOW2_PIN), MAKENAME(GPIO_AF_,SYNC_TIMER_LOW2_AF));
  
  /* LOW3 GPIO clock enable */
  RCC_AHBPeriphClockCmd(MAKENAME(RCC_AHBPeriph_GPIO,SYNC_TIMER_LOW3_PORT), ENABLE);
  
  /* LOW3 GPIO config */
  GPIO_InitStructure.GPIO_Pin   = MAKENAME(GPIO_Pin_,SYNC_TIMER_LOW3_PIN);
  GPIO_Init(MAKENAME(GPIO,SYNC_TIMER_LOW3_PORT), &GPIO_InitStructure);
  
  /* Connect TIM pin LOW3 to AF */
  GPIO_PinAFConfig(MAKENAME(GPIO,SYNC_TIMER_LOW3_PORT), MAKENAME(GPIO_PinSource,SYNC_TIMER_LOW3_PIN), MAKENAME(GPIO_AF_,SYNC_TIMER_LOW3_AF));
  
  // timer setup
  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
  TIM_OCStructInit(&TIM_OCInitStructure);
  
  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 65535;
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_CenterAligned2; 
  
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 0xFFFF;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
  
  // High timer setup
  TIM_TimeBaseInit(MAKENAME(TIM,SYNC_TIMER_HIGH), &TIM_TimeBaseStructure);
  
  //TIM output compare
  TIM_OC1Init(MAKENAME(TIM,SYNC_TIMER_HIGH), &TIM_OCInitStructure);
  TIM_OC2Init(MAKENAME(TIM,SYNC_TIMER_HIGH), &TIM_OCInitStructure);
  TIM_OC3Init(MAKENAME(TIM,SYNC_TIMER_HIGH), &TIM_OCInitStructure);
  TIM_OC4Init(MAKENAME(TIM,SYNC_TIMER_HIGH), &TIM_OCInitStructure);
  
  //TIM preload (shadow) register enable
  TIM_OC1PreloadConfig(MAKENAME(TIM,SYNC_TIMER_HIGH),TIM_OCPreload_Enable);
  TIM_OC2PreloadConfig(MAKENAME(TIM,SYNC_TIMER_HIGH),TIM_OCPreload_Enable);
  TIM_OC3PreloadConfig(MAKENAME(TIM,SYNC_TIMER_HIGH),TIM_OCPreload_Enable);
  TIM_OC4PreloadConfig(MAKENAME(TIM,SYNC_TIMER_HIGH),TIM_OCPreload_Enable);
  
  TIM_ARRPreloadConfig(MAKENAME(TIM,SYNC_TIMER_HIGH),ENABLE);
  
  TIM_SelectOutputTrigger(MAKENAME(TIM,SYNC_TIMER_HIGH), TIM_TRGOSource_Enable);
  TIM_SelectMasterSlaveMode(MAKENAME(TIM,SYNC_TIMER_HIGH), TIM_MasterSlaveMode_Enable);
  
  // Low timer setup
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_TimeBaseInit(MAKENAME(TIM,SYNC_TIMER_LOW), &TIM_TimeBaseStructure);
  //TIM output compare
  TIM_OC1Init(MAKENAME(TIM,SYNC_TIMER_LOW), &TIM_OCInitStructure);
  TIM_OC2Init(MAKENAME(TIM,SYNC_TIMER_LOW), &TIM_OCInitStructure);
  TIM_OC3Init(MAKENAME(TIM,SYNC_TIMER_LOW), &TIM_OCInitStructure);
  
  //TIM preload (shadow) register enable
  TIM_OC1PreloadConfig(MAKENAME(TIM,SYNC_TIMER_LOW),TIM_OCPreload_Enable);
  TIM_OC2PreloadConfig(MAKENAME(TIM,SYNC_TIMER_LOW),TIM_OCPreload_Enable);
  TIM_OC3PreloadConfig(MAKENAME(TIM,SYNC_TIMER_LOW),TIM_OCPreload_Enable);
  
  TIM_ARRPreloadConfig(MAKENAME(TIM,SYNC_TIMER_LOW),ENABLE);
  
  TIM_SelectInputTrigger(MAKENAME(TIM,SYNC_TIMER_LOW), SYNC_TIMER_ITR);
  TIM_SelectSlaveMode(MAKENAME(TIM,SYNC_TIMER_LOW), TIM_SlaveMode_Trigger);
  
  // Interrupt setup
  NVIC_InitStructure.NVIC_IRQChannel = MAKENAME(TIM,MAKENAME(SYNC_TIMER_HIGH,_IRQn));
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  TIM_ITConfig(MAKENAME(TIM,SYNC_TIMER_HIGH), TIM_IT_CC4, ENABLE);

  
  //TIM_Cmd(MAKENAME(TIM,SYNC_TIMER_LOW), ENABLE); // Enabled by the high side enable trigger
  TIM_Cmd(MAKENAME(TIM,SYNC_TIMER_HIGH), ENABLE);
}

void SetSyncPWM(uint16_t arr, uint16_t dty1, uint16_t dty2, uint16_t dty3){
	MAKENAME(TIM,SYNC_TIMER_HIGH)->ARR = arr;
	MAKENAME(TIM,SYNC_TIMER_LOW)->ARR = arr;
  
  MAKENAME(TIM,SYNC_TIMER_HIGH)->CCR4 = 1000; // CCR4 is the commutation interrupt time.  On down count, the interrupt fires at this time value.  The commutation has this much time to do its math. 
  
  if (arr < dty1)
  {
    MAKENAME(TIM,SYNC_TIMER_LOW)->CCR1 = 0; // Shut off low side fet
    MAKENAME(TIM,SYNC_TIMER_HIGH)->CCR1 = 0; // Always on high side fet
  }
  else if (arr - dty1 < SYNC_TIMER_DEADTIME)
  {
    MAKENAME(TIM,SYNC_TIMER_LOW)->CCR1 = 0; // Shut off low side fet
    MAKENAME(TIM,SYNC_TIMER_HIGH)->CCR1 = arr - dty1; // high side fet on for desired time
  }
  else
  {
    MAKENAME(TIM,SYNC_TIMER_HIGH)->CCR1 = arr - dty1;
    MAKENAME(TIM,SYNC_TIMER_LOW)->CCR1 = arr - dty1 - SYNC_TIMER_DEADTIME;
  }
  
  if (arr < dty2)
  {
    MAKENAME(TIM,SYNC_TIMER_LOW)->CCR2 = 0;
    MAKENAME(TIM,SYNC_TIMER_HIGH)->CCR2 = 0;
  }
  else if (arr - dty2 < SYNC_TIMER_DEADTIME)
  {
    MAKENAME(TIM,SYNC_TIMER_LOW)->CCR2 = 0;
    MAKENAME(TIM,SYNC_TIMER_HIGH)->CCR2 = arr - dty2;
  }
  else
  {
    MAKENAME(TIM,SYNC_TIMER_HIGH)->CCR2 = arr - dty2;
    MAKENAME(TIM,SYNC_TIMER_LOW)->CCR2 = arr - dty2 - SYNC_TIMER_DEADTIME;
  }
  
  if (arr < dty3)
  {
    MAKENAME(TIM,SYNC_TIMER_LOW)->CCR3 = 0;
    MAKENAME(TIM,SYNC_TIMER_HIGH)->CCR3 = 0;
  }
  else if (arr - dty3 < SYNC_TIMER_DEADTIME)
  {
    MAKENAME(TIM,SYNC_TIMER_LOW)->CCR3 = 0;
    MAKENAME(TIM,SYNC_TIMER_HIGH)->CCR3 = arr - dty3;
  }
  else
  {
    MAKENAME(TIM,SYNC_TIMER_HIGH)->CCR3 = arr - dty3;
    MAKENAME(TIM,SYNC_TIMER_LOW)->CCR3 = arr - dty3 - SYNC_TIMER_DEADTIME;
  }
}

void SetSyncPWMH(uint16_t arr, uint16_t dty1, uint16_t dty2, uint16_t dty3){
	// Auto-reload value, period of PWM
	MAKENAME(TIM,SYNC_TIMER_HIGH)->ARR = arr;
	
	MAKENAME(TIM,SYNC_TIMER_HIGH)->CCR1 = arr - dty1;
  MAKENAME(TIM,SYNC_TIMER_HIGH)->CCR2 = arr - dty2;
	MAKENAME(TIM,SYNC_TIMER_HIGH)->CCR3 = arr - dty3;
}

void SetSyncPWML(uint16_t arr, uint16_t dty1, uint16_t dty2, uint16_t dty3){
	// Auto-reload value, period of PWM
	MAKENAME(TIM,SYNC_TIMER_LOW)->ARR = arr;
	
	MAKENAME(TIM,SYNC_TIMER_LOW)->CCR1 = arr - dty1;
	MAKENAME(TIM,SYNC_TIMER_LOW)->CCR2 = arr - dty2;
	MAKENAME(TIM,SYNC_TIMER_LOW)->CCR3 = arr - dty3;
}

void MAKENAME(TIM,MAKENAME(SYNC_TIMER_HIGH,_IRQHandler))(void)
{
  // start 7.9us
  // start 653ns
  if (TIM_GetITStatus(MAKENAME(TIM,SYNC_TIMER_HIGH), TIM_IT_CC4) != RESET) {
    TIM_ClearITPendingBit(MAKENAME(TIM,SYNC_TIMER_HIGH), TIM_IT_CC4);
  }
  // end 653ns
  // start 7.4us
  SyncTimerCallback();
  // end 7.4us
  // end 7.9us
  // well, this math doesn't add up, but you get the picture
}
#endif