/**
  *****************************************************************************************
  * @file    DC_Motor.c 
  * @author  Chao Liu (chao.liu0307@gmail.com)
  * @version V1
  * @date    21-October-2014
  * @brief   This source file is used for DC Motor Control on STM32F303
  *****************************************************************************************
*/

#include "DC_Motor.h"

GPIO_InitTypeDef GPIO_InitStructure;
TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
TIM_OCInitTypeDef  TIM_OCInitStructure;

void Motor_Direction_Init(Motor motor);
void Motor_PWM_Init(Motor motor);
//**************************************************************************************
//
//!  Motor_Init
//!
//!  @param    motor
//!          
//!  @return   none
//!
//!  @brief    This function is used to initialize a DC motor.
//
//**************************************************************************************
void Motor_Init(Motor motor)
{
  Motor_Direction_Init(motor);
  Motor_PWM_Init(motor);
}

//**************************************************************************************
//
//!  set_motor_direction
//!
//!  @param    motor
//!  @param    direction: SET or RESET
//!          
//!  @return   none
//!
//!  @brief    This function is used to set the direction for a DC motor.
//
//**************************************************************************************
void set_motor_direction(Motor motor, BitAction direction)
{
  GPIO_WriteBit(motor.GPIO_Direction_GPIOx, motor.GPIO_Direction_Pin, direction);
}

//**************************************************************************************
//
//!  set_motor_speed
//!
//!  @param    motor
//!  @param    speed: range from 0 to 100
//!          
//!  @return   none
//!
//!  @brief    This function is used to initialize a DC motor.
//
//**************************************************************************************
void set_motor_speed(Motor motor, uint8_t speed)
{
  switch(motor.TIMx_Channel)
  {
    case TIM_Channel_1:
      TIM_SetCompare1(motor.TIMx, speed);
      break;
    case TIM_Channel_2:
      TIM_SetCompare2(motor.TIMx, speed);
      break;
    case TIM_Channel_3:
      TIM_SetCompare3(motor.TIMx, speed);
      break;
    case TIM_Channel_4:
      TIM_SetCompare4(motor.TIMx, speed);
      break;
  }
}

void Motor_Direction_Init(Motor motor)
{
  RCC_AHBPeriphClockCmd(motor.GPIO_Direction_RCC, ENABLE);
  GPIO_InitStructure.GPIO_Pin = motor.GPIO_Direction_Pin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_3;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(motor.GPIO_Direction_GPIOx, &GPIO_InitStructure);
}


void Motor_PWM_Init(Motor motor)
{
  RCC_AHBPeriphClockCmd(motor.GPIO_PWM_RCC, ENABLE);
  GPIO_InitStructure.GPIO_Pin = motor.GPIO_PWM_Pin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_3;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(motor.GPIO_PWM_GPIOx, &GPIO_InitStructure);

  GPIO_PinAFConfig(motor.GPIO_PWM_GPIOx, motor.GPIO_PWM_PinSource, motor.GPIO_PWM_AF);

  if (motor.TIMx == TIM1) {
    RCC_APB2PeriphClockCmd(motor.TIMx_RCC, ENABLE);
  } else if (motor.TIMx == TIM2) {
    RCC_APB1PeriphClockCmd(motor.TIMx_RCC, ENABLE);
  } else {
    if (IS_RCC_APB2_PERIPH(motor.TIMx_RCC))
    {
      RCC_APB2PeriphClockCmd(motor.TIMx_RCC, ENABLE);
    }
    else
    {
      RCC_APB1PeriphClockCmd(motor.TIMx_RCC, ENABLE);
    }
  }

  TIM_TimeBaseStructure.TIM_Prescaler = 36;//PrescalerValue;
  TIM_TimeBaseStructure.TIM_Period = 100 - 1;   // the frequency is 20KHz
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  
  TIM_TimeBaseInit(motor.TIMx, &TIM_TimeBaseStructure);

  TIM_OCInitStructure.TIM_OCMode = motor.TIMx_Mode;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = motor.speed;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    
  switch (motor.TIMx_Channel)
  {
    case TIM_Channel_1:
      TIM_OC1Init(motor.TIMx, &TIM_OCInitStructure);
      TIM_OC1PreloadConfig(motor.TIMx, TIM_OCPreload_Enable);
      break;
    case TIM_Channel_2:
      TIM_OC2Init(motor.TIMx, &TIM_OCInitStructure);
      TIM_OC2PreloadConfig(motor.TIMx, TIM_OCPreload_Enable);
      break;
    case TIM_Channel_3:
      TIM_OC3Init(motor.TIMx, &TIM_OCInitStructure);
      TIM_OC3PreloadConfig(motor.TIMx, TIM_OCPreload_Enable);
      break;
    case TIM_Channel_4:
      TIM_OC4Init(motor.TIMx, &TIM_OCInitStructure);
      TIM_OC4PreloadConfig(motor.TIMx, TIM_OCPreload_Enable);
      break;
  }
  if (motor.TIMx == TIM1 || TIM8) {
    TIM_CtrlPWMOutputs(motor.TIMx, ENABLE);
  }
  TIM_ARRPreloadConfig(motor.TIMx, DISABLE);
  TIM_Cmd(motor.TIMx, ENABLE);
}
