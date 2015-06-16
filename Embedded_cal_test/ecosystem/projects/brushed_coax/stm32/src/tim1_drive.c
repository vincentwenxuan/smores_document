#include "tim1_drive.h"
#include "common_peripherals.h"
#include STM32FXXX_TIM_H
#include STM32FXXX_RCC_H
#include STM32FXXX_MISC_H
#include <stdint.h>
#include "arm_math.h"

//#define TIMER_PERIOD ((SystemCoreClock / 20000) - 1)
#define TIMER_PERIOD 3600

////////////////////////////////////////////////////////////////////////////////
// Default Position Callback
float DefaultPositionCallback(void)
{
  return 0.0;
}

////////////////////////////////////////////////////////////////////////////////
// State Variables
float inv_supply_volts = 1/3.7f;

float cmd_volts_mean_1      = 0;
float cmd_volts_amplitude_1 = 0;
float cmd_phase_1           = 0;
float (*PositionCallback1)(void) = DefaultPositionCallback;

float cmd_volts_mean_2      = 0;
float cmd_volts_amplitude_2 = 0;;
float cmd_phase_2           = 0;
float (*PositionCallback2)(void) = DefaultPositionCallback;

////////////////////////////////////////////////////////////////////////////////
// Common Initialization
void tim1_init() {
  
  // CH 1, 2, on GPIO A8, A9
  // CH 1N,2N on GPIO B13, B14

  // GPIO clocks enable
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB, ENABLE);

  // TIM1 clock enable
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

  // Config options for channel outputs
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

  // GPIOA Config: 1, 2 and 3 as alternate function push-pull
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_6);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_6);
  GPIO_PinLockConfig(GPIOA, GPIO_Pin_8 | GPIO_Pin_9);
   
  // GPIOB Config: 1N, 2N, 3N as alternate function push-pull
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_6); 
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_6);
  GPIO_PinLockConfig(GPIOB, GPIO_Pin_13 | GPIO_Pin_14);

  // configure Time Base
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_TimeBaseStructure.TIM_Prescaler         = 0;
  TIM_TimeBaseStructure.TIM_CounterMode       = TIM_CounterMode_CenterAligned1;
  TIM_TimeBaseStructure.TIM_Period            = TIMER_PERIOD;
  TIM_TimeBaseStructure.TIM_ClockDivision     = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

  // configure PWM mode for each channel
  TIM_OCInitTypeDef  TIM_OCInitStructure;
  TIM_OCInitStructure.TIM_OCMode        = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState   = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OutputNState  = TIM_OutputNState_Enable;
  TIM_OCInitStructure.TIM_Pulse         = 0;
  TIM_OCInitStructure.TIM_OCPolarity    = TIM_OCPolarity_Low;
  TIM_OCInitStructure.TIM_OCNPolarity   = TIM_OCNPolarity_Low;
  TIM_OCInitStructure.TIM_OCIdleState   = TIM_OCIdleState_Set;
  TIM_OCInitStructure.TIM_OCNIdleState  = TIM_OCIdleState_Reset;
  TIM_OC1Init(TIM1, &TIM_OCInitStructure);
  TIM_OC2Init(TIM1, &TIM_OCInitStructure);

  // dead time and lock configuration
  TIM_BDTRInitTypeDef TIM_BDTRInitStructure;
  TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;
  TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;
  TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_1;
  TIM_BDTRInitStructure.TIM_DeadTime = 0;
  TIM_BDTRConfig(TIM1, &TIM_BDTRInitStructure);

  // Interrupt setup
  NVIC_InitTypeDef nv;
  nv.NVIC_IRQChannel                    = TIM1_UP_TIM16_IRQn;
  nv.NVIC_IRQChannelPreemptionPriority  = 0;
  nv.NVIC_IRQChannelCmd                 = ENABLE;
  NVIC_Init(&nv);
  TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);

  TIM_SetCompare1(TIM1, 0);
  TIM_SetCompare2(TIM1, 0);

  // TIM1 counter enable
  TIM_Cmd(TIM1, ENABLE);

  // Main Output Enable
  TIM_CtrlPWMOutputs(TIM1, ENABLE);
}


////////////////////////////////////////////////////////////////////////////////
// Shared Parameters
void tim1_set_supply_volts(float volts) {
  if(volts > 0.0f && volts < 15.0f)
  inv_supply_volts = 1.0f/volts;
}

////////////////////////////////////////////////////////////////////////////////
// Motor 1 Control
void tim1_set_duty1(float duty) {
  if(duty < 0.0f)
      duty = 0.0f;
  else if(duty > 1.0f)
    duty = 1.0f;
  uint16_t pulse = (uint16_t)(duty * (TIMER_PERIOD -1));
  TIM_SetCompare1(TIM1, pulse);
}
//void tim1_set_position_callback_1(void *ptr) {
void tim1_set_position_callback_1(float (*ptr) ()) {
  PositionCallback1 = ptr;
}
void tim1_set_cmd_volts_mean_1(float volts) {
  cmd_volts_mean_1 = volts;
}
void tim1_set_cmd_volts_amplitude_1(float volts) {
  cmd_volts_amplitude_1 = volts;
}
void tim1_set_cmd_phase_1(float radians) {
  cmd_phase_1 = radians;
}

////////////////////////////////////////////////////////////////////////////////
// Motor 2 Control
void tim1_set_duty2(float duty) {
  if(duty < 0.0f)
    duty = 0.0f;
  else if(duty > 1.0f)
    duty = 1.0f;
  uint16_t pulse = (uint16_t)(duty * (TIMER_PERIOD -1));
  TIM_SetCompare2(TIM1, pulse);
}
void tim1_set_position_callback_2(float (*ptr) ()) {
  PositionCallback2 = ptr;
}
void tim1_set_cmd_volts_mean_2(float volts) {
  cmd_volts_mean_2 = volts;
}
void tim1_set_cmd_volts_amplitude_2(float volts) {
  cmd_volts_amplitude_2 = volts;
}
void tim1_set_cmd_phase_2(float radians) {
  cmd_phase_2 = radians;
}

////////////////////////////////////////////////////////////////////////////////
// High Frequency Interrupt
void TIM1_UP_TIM16_IRQHandler(void) {
  if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET) {
    TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
  }
  float volts;

  volts = cmd_volts_mean_1 + cmd_volts_amplitude_1 * arm_cos_f32(cmd_phase_1 + PositionCallback1());
  tim1_set_duty1(volts * inv_supply_volts);

  volts = cmd_volts_mean_2 + cmd_volts_amplitude_2 * arm_cos_f32(cmd_phase_2 + PositionCallback2());
  tim1_set_duty2(volts * inv_supply_volts);
}