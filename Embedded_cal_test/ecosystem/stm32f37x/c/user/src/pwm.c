#include "pwm.h"
#include "common_peripherals.h"
#include STM32FXXX_RCC_H

#if defined(USING_PWM1)

uint32_t PwmOneMs;
PWM_SIZE PwmPeriod;

void InitPWM(void)
{
  
  PwmOneMs = SystemCoreClock/1000/PWM_CLOCK_PSC;

  PwmPeriod = (SystemCoreClock/PWM_CLOCK_PSC  / PWM_FREQUENCY - 1);


  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_OCInitTypeDef TIM_OCInitStructure;
  uint16_t PrescalerValue = 0;
  
  /* TIM clock enable */
  #if PWM_TIMER == 15 || PWM_TIMER == 16 || PWM_TIMER == 17 || PWM_TIMER == 19
    RCC_APB2PeriphClockCmd(MAKENAME(RCC_APB2Periph_TIM,PWM_TIMER), ENABLE);
  #else
    RCC_APB1PeriphClockCmd(MAKENAME(RCC_APB1Periph_TIM,PWM_TIMER), ENABLE);
  #endif
  
  /* Compute the prescaler value */
  PrescalerValue = (uint16_t) (PWM_CLOCK_PSC - 1);
  TIM_TimeBaseStructure.TIM_Period = PwmPeriod;
  
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(MAKENAME(TIM,PWM_TIMER), &TIM_TimeBaseStructure);

  /* Prescaler configuration */
  TIM_PrescalerConfig(MAKENAME(TIM,PWM_TIMER), PrescalerValue, TIM_PSCReloadMode_Immediate);

  /* PWM1 GPIO clock enable */
  RCC_AHBPeriphClockCmd(MAKENAME(RCC_AHBPeriph_GPIO,PWM1_PORT), ENABLE);

  /* PWM1 GPIO config */
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin   = MAKENAME(GPIO_Pin_,PWM1_PIN);
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(MAKENAME(GPIO,PWM1_PORT), &GPIO_InitStructure);

  /* Connect TIM pin PWM1 to AF */
  GPIO_PinAFConfig(MAKENAME(GPIO,PWM1_PORT), MAKENAME(GPIO_PinSource,PWM1_PIN), MAKENAME(GPIO_AF_,PWM1_AF));

  /* Output Compare Timing Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 0;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OC1Init(MAKENAME(TIM,PWM_TIMER), &TIM_OCInitStructure);
  TIM_OC1PreloadConfig(MAKENAME(TIM,PWM_TIMER), TIM_OCPreload_Disable);

  #if defined(USING_PWM2) && PWM_TIMER != 13 && PWM_TIMER != 14 && PWM_TIMER != 16 && PWM_TIMER != 17
  /* Output Compare Timing Mode configuration: Channel2 */
  // TIM_OCInitStructure already setup for channel 1
  TIM_OC2Init(MAKENAME(TIM,PWM_TIMER), &TIM_OCInitStructure);

  TIM_OC2PreloadConfig(MAKENAME(TIM,PWM_TIMER), TIM_OCPreload_Disable);
  
  /* PWM2 GPIO clock enable */
  RCC_AHBPeriphClockCmd(MAKENAME(RCC_AHBPeriph_GPIO,PWM2_PORT), ENABLE);
  
  /* PWM2 GPIO config */
  GPIO_InitStructure.GPIO_Pin   = MAKENAME(GPIO_Pin_,PWM2_PIN);
  GPIO_Init(MAKENAME(GPIO,PWM2_PORT), &GPIO_InitStructure);
  
  /* Connect TIM pin PWM2 to AF */
  GPIO_PinAFConfig(MAKENAME(GPIO,PWM2_PORT), MAKENAME(GPIO_PinSource,PWM2_PIN), MAKENAME(GPIO_AF_,PWM2_AF));
///----------------------------------------------------------
  #if defined(USING_PWM3) && PWM_TIMER != 12 && PWM_TIMER != 15
  /* Output Compare Timing Mode configuration: Channel3 */
  // TIM_OCInitStructure already setup for channel 1
  TIM_OC3Init(MAKENAME(TIM,PWM_TIMER), &TIM_OCInitStructure);

  TIM_OC3PreloadConfig(MAKENAME(TIM,PWM_TIMER), TIM_OCPreload_Disable);
  
  /* PWM3 GPIO clock enable */
  RCC_AHBPeriphClockCmd(MAKENAME(RCC_AHBPeriph_GPIO,PWM3_PORT), ENABLE);
  
  /* PWM3 GPIO config */
  GPIO_InitStructure.GPIO_Pin   = MAKENAME(GPIO_Pin_,PWM3_PIN);
  GPIO_Init(MAKENAME(GPIO,PWM3_PORT), &GPIO_InitStructure);
  
  /* Connect TIM pin PWM3 to AF */
  GPIO_PinAFConfig(MAKENAME(GPIO,PWM3_PORT), MAKENAME(GPIO_PinSource,PWM3_PIN), MAKENAME(GPIO_AF_,PWM3_AF));
///----------------------------------------------------------
  #ifdef USING_PWM4
  /* Output Compare Timing Mode configuration: Channel4 */
  // TIM_OCInitStructure already setup for channel 1
  TIM_OC4Init(MAKENAME(TIM,PWM_TIMER), &TIM_OCInitStructure);

  TIM_OC4PreloadConfig(MAKENAME(TIM,PWM_TIMER), TIM_OCPreload_Disable);
   
  /* PWM4 GPIO clock enable */
  RCC_AHBPeriphClockCmd(MAKENAME(RCC_AHBPeriph_GPIO,PWM4_PORT), ENABLE);
  
  /* PWM4 GPIO config */
  GPIO_InitStructure.GPIO_Pin   = MAKENAME(GPIO_Pin_,PWM4_PIN);
  GPIO_Init(MAKENAME(GPIO,PWM4_PORT), &GPIO_InitStructure);
  
  /* Connect TIM pin PWM4 to AF */
  GPIO_PinAFConfig(MAKENAME(GPIO,PWM4_PORT), MAKENAME(GPIO_PinSource,PWM4_PIN), MAKENAME(GPIO_AF_,PWM4_AF));
  #endif //USING_PWM4
  #endif //defined(USING_PWM3) && PWM_TIMER != 12 && PWM_TIMER != 15
  #endif //defined(USING_PWM2) && PWM_TIMER != 13 && PWM_TIMER != 14 && PWM_TIMER != 16 && PWM_TIMER != 17
}

uint32_t GetPWMPeriod(void)
{
  return (uint32_t)(PwmPeriod);
}

PWM_SIZE ServoMath(float value)
{
  return (PWM_SIZE)((float)PwmOneMs*value) + PwmOneMs;
}

void EnablePWM(void){
TIM_Cmd(MAKENAME(TIM,PWM_TIMER), ENABLE);
}

void DisablePWM(void){
TIM_Cmd(MAKENAME(TIM,PWM_TIMER), DISABLE);
}

void SetPWM(uint8_t number, float value)
{
  switch(number)
  {
    case 1:
      SetPWM1(value);
      break;
    #ifdef USING_PWM2
    case 2:
      SetPWM2(value);
      break;
    #endif
    #ifdef USING_PWM3
    case 3:
      SetPWM3(value);
      break;
    #endif
    #ifdef USING_PWM4
    case 4:
      SetPWM4(value);
      break;
    #endif
  }
}

void SetServo(uint8_t number, float value)
{
  switch(number)
  {
    case 1:
      SetServo1(value);
      break;
    #ifdef USING_PWM2
    case 2:
      SetServo2(value);
      break;
    #endif
    #ifdef USING_PWM3
    case 3:
      SetServo3(value);
      break;
    #endif
    #ifdef USING_PWM4
    case 4:
      SetServo4(value);
      break;
    #endif
  }
}

void SetPWM1(float value)
{
  MAKENAME(TIM,PWM_TIMER)->CCR1 = (PWM_SIZE)((float)PwmPeriod*value);
}
void SetServo1(float value)
{
  MAKENAME(TIM,PWM_TIMER)->CCR1 = ServoMath(value);
}

#ifdef USING_PWM2
void SetPWM2(float value)
{
MAKENAME(TIM,PWM_TIMER)->CCR2 = (PWM_SIZE)((float)PwmPeriod*value);
}
void SetServo2(float value)
{
  MAKENAME(TIM,PWM_TIMER)->CCR2 = ServoMath(value);
}
#endif //USING_PWM2

#ifdef USING_PWM3
void SetPWM3(float value)
{
  MAKENAME(TIM,PWM_TIMER)->CCR3 = (PWM_SIZE)((float)PwmPeriod*value);
}
void SetServo3(float value)
{
  MAKENAME(TIM,PWM_TIMER)->CCR3 = ServoMath(value);
}
#endif //USING_PWM3

#ifdef USING_PWM4
void SetPWM4(float value)
{
  MAKENAME(TIM,PWM_TIMER)->CCR4 = (PWM_SIZE)((float)PwmPeriod*value);
}
void SetServo4(float value)
{
  MAKENAME(TIM,PWM_TIMER)->CCR4 = ServoMath(value);
}
#endif //USING_PWM4

#endif //defined(USING_PWM1)