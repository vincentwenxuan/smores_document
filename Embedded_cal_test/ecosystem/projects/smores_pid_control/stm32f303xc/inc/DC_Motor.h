/**
  *****************************************************************************************
  * @file    DC_Motor.h 
  * @author  Chao Liu (chao.liu0307@gmail.com)
  * @version V1
  * @date    21-October-2014
  * @brief   This header file is used for DC Motor Control on STM32F303
  *****************************************************************************************
*/

#ifndef DC_MOTOR_H_
#define DC_MOTOR_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "stm32f30x.h"
#include "stm32f30x_gpio.h"
#include "stm32f30x_rcc.h"
#include "stm32f30x_tim.h"

typedef struct
{
	GPIO_TypeDef*     GPIO_Direction_GPIOx;
	uint16_t          GPIO_Direction_Pin;
    uint32_t          GPIO_Direction_RCC;
	GPIO_TypeDef*     GPIO_PWM_GPIOx;
	uint16_t          GPIO_PWM_Pin;
	uint16_t          GPIO_PWM_PinSource;
	uint8_t           GPIO_PWM_AF;
    uint32_t          GPIO_PWM_RCC;
	TIM_TypeDef*      TIMx;
	uint16_t          TIMx_Channel;
	uint32_t          TIMx_RCC;
	uint32_t          TIMx_Mode;
	BitAction         direction;      ///< Bit_RESET or Bit_SET
	uint8_t           speed;            ///< range 0 -- 100
} Motor;

void Motor_Init(Motor motor);
void set_motor_direction(Motor motor, BitAction direction);
void set_motor_speed(Motor motor, uint8_t speed);
    
#ifdef __cplusplus
}
#endif

#endif
