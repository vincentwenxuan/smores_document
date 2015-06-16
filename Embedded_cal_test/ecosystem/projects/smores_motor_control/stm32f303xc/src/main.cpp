/**
  *****************************************************************************************
  * @file    main.cpp
  * @author  Chao Liu (chao.liu0307@gmail.com)
  * @version V1
  * @date    21-October-2014
  * @brief   This file is used for DC Motor Control on STM32F303
  *****************************************************************************************
*/

#include "common_peripherals.h"

// common components 
#include "common_message_types.h" 
// common messages
#include "usb_interface.h"        
// usb interface
#include "mBus.h"

// common libraries
#include "project_message_types.h"
#include "error_reporting.h"
#include <string.h> 
//needed for memset
//#define   mBUS 1

#include "DC_Motor.h"


int main (void){
  InitPeripherals(); 
  mYellowON; mRedON; mGreenON; mWhiteON; mBlueON;
  DelayMilliseconds(1000);
  mYellowOFF;mRedOFF;mGreenOFF; mWhiteOFF; mBlueOFF;

  Motor motor_1;
  motor_1.GPIO_Direction_GPIOx = GPIOB;
  motor_1.GPIO_Direction_Pin = GPIO_Pin_2;
  motor_1.GPIO_Direction_RCC = RCC_AHBPeriph_GPIOB;
  motor_1.GPIO_PWM_GPIOx = GPIOA;
  motor_1.GPIO_PWM_Pin = GPIO_Pin_8;
  motor_1.GPIO_PWM_PinSource = GPIO_PinSource8;
  motor_1.GPIO_PWM_AF = GPIO_AF_6;
  motor_1.GPIO_PWM_RCC = RCC_AHBPeriph_GPIOA;
  motor_1.TIMx = TIM1;
  motor_1.TIMx_Channel = TIM_Channel_1;
  motor_1.TIMx_RCC = RCC_APB2Periph_TIM1;
  motor_1.TIMx_Mode = TIM_OCMode_PWM1;
  motor_1.direction = Bit_RESET;
  motor_1.speed = 0;
  
  Motor_Init(motor_1);

  Motor motor_2;
  motor_2.GPIO_Direction_GPIOx = GPIOA;
  motor_2.GPIO_Direction_Pin = GPIO_Pin_10;
  motor_2.GPIO_Direction_RCC = RCC_AHBPeriph_GPIOA;
  motor_2.GPIO_PWM_GPIOx = GPIOB;
  motor_2.GPIO_PWM_Pin = GPIO_Pin_5;
  motor_2.GPIO_PWM_PinSource = GPIO_PinSource5;
  motor_2.GPIO_PWM_AF = GPIO_AF_2;
  motor_2.GPIO_PWM_RCC = RCC_AHBPeriph_GPIOB;
  motor_2.TIMx = TIM3;
  motor_2.TIMx_Channel = TIM_Channel_2;
  motor_2.TIMx_RCC = RCC_APB1Periph_TIM3;
  motor_2.TIMx_Mode = TIM_OCMode_PWM1;
  motor_2.direction = Bit_RESET;
  motor_2.speed = 0;

  Motor_Init(motor_2);
  
  Motor motor_3;
  motor_3.GPIO_Direction_GPIOx = GPIOB;
  motor_3.GPIO_Direction_Pin = GPIO_Pin_15;
  motor_3.GPIO_Direction_RCC = RCC_AHBPeriph_GPIOB;
  motor_3.GPIO_PWM_GPIOx = GPIOB;
  motor_3.GPIO_PWM_Pin = GPIO_Pin_14;
  motor_3.GPIO_PWM_PinSource = GPIO_PinSource14;
  motor_3.GPIO_PWM_AF = GPIO_AF_1;
  motor_3.GPIO_PWM_RCC = RCC_AHBPeriph_GPIOB;
  motor_3.TIMx = TIM15;
  motor_3.TIMx_Channel = TIM_Channel_1;
  motor_3.TIMx_RCC = RCC_APB2Periph_TIM15;
  motor_3.TIMx_Mode = TIM_OCMode_PWM1;
  motor_3.direction = Bit_RESET;
  motor_3.speed = 0;

  Motor_Init(motor_3);

  Motor motor_4;
  motor_4.GPIO_Direction_GPIOx = GPIOA;
  motor_4.GPIO_Direction_Pin = GPIO_Pin_0;
  motor_4.GPIO_Direction_RCC = RCC_AHBPeriph_GPIOA;
  motor_4.GPIO_PWM_GPIOx = GPIOA;
  motor_4.GPIO_PWM_Pin = GPIO_Pin_1;
  motor_4.GPIO_PWM_PinSource = GPIO_PinSource1;
  motor_4.GPIO_PWM_AF = GPIO_AF_1;
  motor_4.GPIO_PWM_RCC = RCC_AHBPeriph_GPIOA;
  motor_4.TIMx = TIM2;
  motor_4.TIMx_Channel = TIM_Channel_2;
  motor_4.TIMx_RCC = RCC_APB1Periph_TIM2;
  motor_4.TIMx_Mode = TIM_OCMode_PWM1;
  motor_4.direction = Bit_RESET;
  motor_4.speed = 0;

  Motor_Init(motor_4);
  
  mGreenON;
  
  while(1) {
    DelayMilliseconds(500);
    mRedON;
    set_motor_direction(motor_1, Bit_SET);
    set_motor_direction(motor_2, Bit_RESET);
    set_motor_direction(motor_3, Bit_SET);
    set_motor_direction(motor_4, Bit_RESET);
    set_motor_speed(motor_1, 20);
    set_motor_speed(motor_2, 40);
    set_motor_speed(motor_3, 60);
    set_motor_speed(motor_4, 80);
    DelayMilliseconds(500);
    mRedOFF;
    set_motor_direction(motor_1, Bit_RESET);
    set_motor_direction(motor_2, Bit_SET);
    set_motor_direction(motor_3, Bit_RESET);
    set_motor_direction(motor_4, Bit_SET);
    // set_motor_speed(motor_1, 30);
    // set_motor_speed(motor_2, 30);
  } // while(1)
  return(0);
}

