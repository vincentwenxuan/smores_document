#include "stm32f37x.h"
#include "stm32f37x_gpio.h"
#include "stm32f37x_rcc.h"
#include "stm32f37x_tim.h"

//#define allPCB
#define SMALL_SQUARE

void TIM4_Config()
{
  GPIO_InitTypeDef GPIO_InitStructure;
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;

  uint16_t CCR1_Val = 0;
  uint16_t CCR2_Val = 0;
  uint16_t CCR3_Val = 0;
  uint16_t CCR4_Val = 0;

  uint16_t PrescalerValue = 0;

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOF, ENABLE);
  
  #ifdef SMALL_SQUARE
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  #endif
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOF, &GPIO_InitStructure);
  
  #ifdef allPCB
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  #endif 
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  #ifdef SMALL_SQUARE
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource13, GPIO_AF_10);  // t4c1
  #endif
  
  #ifdef allPCB
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_2);  // t4c1
  #endif
  
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource6, GPIO_AF_2);  // t4c2
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_2); // t4c3
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_2); // t4c4

  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);

  TIM_OCStructInit(&TIM_OCInitStructure);

  /* Compute the prescaler value */
  //uint32_t timer4_freq = 18000000; // 18 KHz
  uint32_t timer4_freq = 1000000; // 1 KHz
  // SystemCoreClock = 72Mhz;

  PrescalerValue = (uint16_t) (SystemCoreClock / timer4_freq) - 1;

  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 1000-1;
  TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

  /* Output Compare Active Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = CCR1_Val;
  TIM_OC1Init(TIM4, &TIM_OCInitStructure);

  TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);
  TIM_ARRPreloadConfig(TIM4, DISABLE);

  /* Output Compare Active Mode configuration: Channel2 */
  TIM_OCInitStructure.TIM_Pulse = CCR2_Val;
  TIM_OC2Init(TIM4, &TIM_OCInitStructure);

  TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);

  /* Output Compare Active Mode configuration: Channel3 */
  TIM_OCInitStructure.TIM_Pulse = CCR3_Val;
  TIM_OC3Init(TIM4, &TIM_OCInitStructure);

  TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);

  /* Output Compare Active Mode configuration: Channel4 */
  TIM_OCInitStructure.TIM_Pulse = CCR4_Val;
  TIM_OC4Init(TIM4, &TIM_OCInitStructure);

  TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);

  /* TIM4 enable counter */
  TIM_Cmd(TIM4, ENABLE);

  TIM_GenerateEvent(TIM4, TIM_EventSource_Update);
}

void updatePWM(uint16_t pwm_m1, uint16_t pwm_m2, uint16_t pwm_m3, uint16_t pwm_m4)
{
#ifdef allPCB
  // All motors on Timer 4
  // Ch3: Motor1, Ch4: Motor2, Ch1: Motor3, Ch2: Motor4
  TIM_SetCompare1(TIM4, pwm_m1);
  TIM_SetCompare4(TIM4, pwm_m2);
  TIM_SetCompare2(TIM4, pwm_m3);
  TIM_SetCompare3(TIM4, pwm_m4);
#endif
  
#ifdef SMALL_SQUARE
// All motors on Timer 4
  // Ch3: Motor1, Ch4: Motor2, Ch1: Motor3, Ch2: Motor4
  TIM_SetCompare1(TIM4, pwm_m3);
  TIM_SetCompare4(TIM4, pwm_m2);
  TIM_SetCompare2(TIM4, pwm_m4);
  TIM_SetCompare3(TIM4, pwm_m1);
#endif

/*#else
  // Ch2: Motor1, Ch3: Motor2, Ch4: Motor3, Ch1: Motor4
  //TIM_SetCompare2(TIM4, pwm_m1);
  TIM_SetCompare4(TIM19, pwm_m1);
  TIM_SetCompare3(TIM4, pwm_m2); 
  TIM_SetCompare4(TIM4, pwm_m3); 
  //TIM_SetCompare1(TIM4, pwm_m4);
  TIM_SetCompare3(TIM19, pwm_m4);
#endif*/
  
}
