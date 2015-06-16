#include "stm32f37x.h"
#include "stm32f37x_gpio.h"
#include "stm32f37x_rcc.h"
#include "stm32f37x_tim.h"

void TIM3_Config()
{
  GPIO_InitTypeDef GPIO_InitStructure;
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;

  uint16_t CCR1_Val = 0;
  uint16_t CCR2_Val = 0;
  uint16_t CCR3_Val = 0;
  uint16_t CCR4_Val = 0;

  uint16_t PrescalerValue = 0;

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 |GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_2);  // t3c1
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_2);  // t3c2
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_10); // t3c3
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_10); // t3c4

  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);

  TIM_OCStructInit(&TIM_OCInitStructure);

  /* Compute the prescaler value */
  //uint32_t timer3_freq = 18000000; // 18 KHz
  uint32_t timer3_freq = 1000000; // 1 KHz
  // SystemCoreClock = 72Mhz;

  PrescalerValue = (uint16_t) (SystemCoreClock / timer3_freq) - 1;

  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 1000-1;
  TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

  /* Output Compare Active Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = CCR1_Val;
  TIM_OC1Init(TIM3, &TIM_OCInitStructure);

  TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
  TIM_ARRPreloadConfig(TIM3, DISABLE);

  /* Output Compare Active Mode configuration: Channel2 */
  TIM_OCInitStructure.TIM_Pulse = CCR2_Val;
  TIM_OC2Init(TIM3, &TIM_OCInitStructure);

  TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);

  /* Output Compare Active Mode configuration: Channel3 */
  TIM_OCInitStructure.TIM_Pulse = CCR3_Val;
  TIM_OC3Init(TIM3, &TIM_OCInitStructure);

  TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);

  /* Output Compare Active Mode configuration: Channel4 */
  TIM_OCInitStructure.TIM_Pulse = CCR4_Val;
  TIM_OC4Init(TIM3, &TIM_OCInitStructure);

  TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);

  /* TIM3 enable counter */
  TIM_Cmd(TIM3, ENABLE);

  TIM_GenerateEvent(TIM3, TIM_EventSource_Update);
}