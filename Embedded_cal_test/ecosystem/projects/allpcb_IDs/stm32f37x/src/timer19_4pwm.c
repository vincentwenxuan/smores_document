#include "stm32f37x.h"
#include "stm32f37x_gpio.h"
#include "stm32f37x_rcc.h"
#include "stm32f37x_tim.h"

void TIM19_Config()
{
  GPIO_InitTypeDef GPIO_InitStructure;
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;

  uint16_t PrescalerValue = 0;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM19, ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_11); // t19c3
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_11); // t19c4

  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);

  TIM_OCStructInit(&TIM_OCInitStructure);

  /* Compute the prescaler value */
  //uint32_t timer4_freq = 18000000; // 18 KHz
  uint32_t timer19_freq = 1000000; // 1 KHz
  // SystemCoreClock = 72Mhz;

  PrescalerValue = (uint16_t) (SystemCoreClock / timer19_freq) - 1;

  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 1000-1;
  TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM19, &TIM_TimeBaseStructure);

  /* Output Compare Active Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 0;
  TIM_OC1Init(TIM19, &TIM_OCInitStructure);

  TIM_OC1PreloadConfig(TIM19, TIM_OCPreload_Enable);
  TIM_ARRPreloadConfig(TIM19, DISABLE);

  /* Output Compare Active Mode configuration: Channel2 */
  TIM_OCInitStructure.TIM_Pulse = 0;
  TIM_OC2Init(TIM19, &TIM_OCInitStructure);

  TIM_OC2PreloadConfig(TIM19, TIM_OCPreload_Enable);

  /* Output Compare Active Mode configuration: Channel3 */
  TIM_OCInitStructure.TIM_Pulse = 0;
  TIM_OC3Init(TIM19, &TIM_OCInitStructure);

  TIM_OC3PreloadConfig(TIM19, TIM_OCPreload_Enable);

  /* Output Compare Active Mode configuration: Channel4 */
  TIM_OCInitStructure.TIM_Pulse = 0;
  TIM_OC4Init(TIM19, &TIM_OCInitStructure);

  TIM_OC4PreloadConfig(TIM19, TIM_OCPreload_Enable);

  /* TIM19 enable counter */
  TIM_Cmd(TIM19, ENABLE);

  TIM_GenerateEvent(TIM19, TIM_EventSource_Update);
}
