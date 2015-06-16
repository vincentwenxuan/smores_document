#include "get_position.h"
#include "common_peripherals.h"
#include "system_clock.h"
#include STM32FXXX_GPIO_H
#include STM32FXXX_MISC_H
#include STM32FXXX_RCC_H
#include STM32FXXX_TIM_H
#include STM32FXXX_SYSCFG_H
#include STM32FXXX_EXTI_H

const float kPi     = 3.141592653589793238462643383279502884f;
const float k2Pi    = 6.283185307179586476925286766559005768f;

struct Time prev_time; // time of last sensor interrupt

float velocity = 0.0f; // velocity calculated at last sensor interrupt, rad/s


static inline float WrapRad(float rad) {
  while(rad > kPi) {
    rad = rad - k2Pi;
  };
  while(rad < -kPi) {
    rad = rad + k2Pi;
  };
  return rad;
}


void InitPositionSensor(void)
{
  // RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  
  /* Enable SYSCFG clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  
  // initialise the hall_sensor input pin (PB1):
  GPIO_InitTypeDef hall_input_init;
  hall_input_init.GPIO_Pin = GPIO_Pin_1;
  hall_input_init.GPIO_Mode = GPIO_Mode_IN;
  hall_input_init.GPIO_Speed = GPIO_Speed_50MHz;
  hall_input_init.GPIO_PuPd = GPIO_PuPd_NOPULL; //Pulled -up by default, assuming the presence of magnet pulls the line low
  GPIO_Init(GPIOB,&hall_input_init);

  // Select the input source pin for the EXTI line using SYSCFG_EXTILineConfig().
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB,EXTI_PinSource1); //Pin PB1

   //Interrupt Setup:
  EXTI_InitTypeDef   EXTI_InitStructure;
  /* Configure EXTI0 line */
 // GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource1);
  EXTI_InitStructure.EXTI_Line = EXTI_Line1;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  EXTI_ClearITPendingBit(EXTI_Line1);
 
  // Configure NVIC IRQ channel mapped to the EXTI line using NVIC_Init().
  //Definition in ecosystem/stm32f30x/stm32f30x_misc.h
  NVIC_InitTypeDef   NVIC_InitStructure;
  /* Enable and set EXTI0 Interrupt to the highest priority */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);    

}


float get_position(void)
{
  // calculate rotation since last interrupt in radians
  float position = ToSeconds(SubtractTime(GetTime(), prev_time)) * velocity;

  // it is not possible to have rotated further than 2*pi since the last interrupt
  if(position > k2Pi) {
    position = k2Pi;
  }

  // adjust for the known position of the sensor
  position = position - 0.0f;

  // restict to the range [-pi, pi]
  position = WrapRad(position);

  return position;
}


float get_velocity()
{
  return velocity;
}


void EXTI1_IRQHandler(void) //ISR
{
  if(EXTI_GetITStatus(EXTI_Line1) != RESET)
  {
    mRedTOGGLE;
    struct Time t = GetTime(); // get current time

    // if previous time is nonzero, compute velocity from previous time
    if(prev_time.sec != 0 || prev_time.nsec != 0) {
      velocity = k2Pi / ToSeconds(SubtractTime(t, prev_time));
    }

    prev_time = t; // update previous time to reflect this sample
    EXTI_ClearITPendingBit(EXTI_Line1); // clear pending bit
  }
}