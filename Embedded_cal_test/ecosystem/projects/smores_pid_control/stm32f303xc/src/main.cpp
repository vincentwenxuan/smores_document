/**
  *****************************************************************************************
  * @file    main.cpp
  * @author  Chao Liu (chao.liu0307@gmail.com)
  * @version V1
  * @date    29-October-2014
  * @brief   This file is used for DC Motor Control with PID controller on STM32F303
  *****************************************************************************************
*/

#include "common_peripherals.h"
#include "system_clock.h"
// common components 
#include "common_message_types.h" 
// common messages
#include "usb_interface.h"        
// usb interface
#include "mBus.h"

// common librarie
#include "project_message_types.h"
#include "error_reporting.h"
#include <string.h>

#include "Wheel_Control.h"
//needed for memset
#define   mBUS 1 


int main (void){
    InitPeripherals();
    // usb interface configuration
    UsbInterface usb = UsbInterface();
    usb.Init();
    // Motor initialization
    Motor left_wheel_motor;
    left_wheel_motor.GPIO_Direction_GPIOx = GPIOA;
    left_wheel_motor.GPIO_Direction_Pin = GPIO_Pin_0;
	left_wheel_motor.GPIO_Direction_RCC = RCC_AHBPeriph_GPIOA;
    left_wheel_motor.GPIO_PWM_GPIOx = GPIOA;
    left_wheel_motor.GPIO_PWM_Pin = GPIO_Pin_1;
    left_wheel_motor.GPIO_PWM_PinSource = GPIO_PinSource1;
    left_wheel_motor.GPIO_PWM_AF = GPIO_AF_1;
	left_wheel_motor.GPIO_PWM_RCC = RCC_AHBPeriph_GPIOA;
    left_wheel_motor.TIMx = TIM2;
    left_wheel_motor.TIMx_Channel = TIM_Channel_2;
    left_wheel_motor.TIMx_RCC = RCC_APB1Periph_TIM2;
    left_wheel_motor.TIMx_Mode = TIM_OCMode_PWM1;
    left_wheel_motor.direction = Bit_RESET;
    left_wheel_motor.speed = 0;
  
    Motor_Init(left_wheel_motor);
    // Position PID controller configuration
    int8_t motor_position_cmd_min = -100;
    uint8_t motor_position_cmd_max = 100;
    
    float left_wheel_position_Kp = 1;
    float left_wheel_position_Ki = 0;
    float left_wheel_position_Kd = 0;
    PID_Controller_Position left_wheel_position_pid(left_wheel_position_Kp, left_wheel_position_Ki, left_wheel_position_Kd, motor_position_cmd_min, motor_position_cmd_max);
    // Speed PID controller configuration
    uint8_t motor_speed_cmd_min = -100;
    uint8_t motor_speed_cmd_max = 100;

    float left_wheel_speed_Kp = 1;
    float left_wheel_speed_Ki = 0;
    float left_wheel_speed_Kd = 0;
    PID_Controller_Speed left_wheel_speed_pid(left_wheel_speed_Kp, left_wheel_speed_Ki, left_wheel_speed_Kd, motor_speed_cmd_min, motor_speed_cmd_max);

    Wheel_Controller_OneMotor left_wheel_control(&left_wheel_position_pid, &left_wheel_speed_pid, left_wheel_motor);

    Motor right_wheel_motor;
    right_wheel_motor.GPIO_Direction_GPIOx = GPIOA;
    right_wheel_motor.GPIO_Direction_Pin = GPIO_Pin_10;
	right_wheel_motor.GPIO_Direction_RCC = RCC_AHBPeriph_GPIOA;
    right_wheel_motor.GPIO_PWM_GPIOx = GPIOA;
    right_wheel_motor.GPIO_PWM_Pin = GPIO_Pin_9;
    right_wheel_motor.GPIO_PWM_PinSource = GPIO_PinSource9;
    right_wheel_motor.GPIO_PWM_AF = GPIO_AF_10;
	right_wheel_motor.GPIO_PWM_RCC = RCC_AHBPeriph_GPIOA;
    right_wheel_motor.TIMx = TIM2;
    right_wheel_motor.TIMx_Channel = TIM_Channel_3;
    right_wheel_motor.TIMx_RCC = RCC_APB1Periph_TIM2;
    right_wheel_motor.TIMx_Mode = TIM_OCMode_PWM1;
    right_wheel_motor.direction = Bit_RESET;
    right_wheel_motor.speed = 0;

    Motor_Init(right_wheel_motor);
       
    float right_wheel_position_Kp = 1;
    float right_wheel_position_Ki = 0;
    float right_wheel_position_Kd = 0;
    PID_Controller_Position right_wheel_position_pid(right_wheel_position_Kp, right_wheel_position_Ki, right_wheel_position_Kd, motor_position_cmd_min, motor_position_cmd_max);

    float right_wheel_speed_Kp = 1;
    float right_wheel_speed_Ki = 0;
    float right_wheel_speed_Kd = 0;
    PID_Controller_Speed right_wheel_speed_pid(right_wheel_speed_Kp, right_wheel_speed_Ki, right_wheel_speed_Kd, motor_speed_cmd_min, motor_speed_cmd_max);

    Wheel_Controller_OneMotor right_wheel_control(&right_wheel_position_pid, &right_wheel_speed_pid, right_wheel_motor);
    
    Motor right_pan_tilt_motor;
    right_pan_tilt_motor.GPIO_Direction_GPIOx = GPIOB;
    right_pan_tilt_motor.GPIO_Direction_Pin = GPIO_Pin_2;
	right_pan_tilt_motor.GPIO_Direction_RCC = RCC_AHBPeriph_GPIOB;
    right_pan_tilt_motor.GPIO_PWM_GPIOx = GPIOA;
    right_pan_tilt_motor.GPIO_PWM_Pin = GPIO_Pin_8;
    right_pan_tilt_motor.GPIO_PWM_PinSource = GPIO_PinSource8;
    right_pan_tilt_motor.GPIO_PWM_AF = GPIO_AF_6;
	right_pan_tilt_motor.GPIO_PWM_RCC = RCC_AHBPeriph_GPIOA;
    right_pan_tilt_motor.TIMx = TIM1;
    right_pan_tilt_motor.TIMx_Channel = TIM_Channel_1;
    right_pan_tilt_motor.TIMx_RCC = RCC_APB2Periph_TIM1;
    right_pan_tilt_motor.TIMx_Mode = TIM_OCMode_PWM1;
    right_pan_tilt_motor.direction = Bit_RESET;
    right_pan_tilt_motor.speed = 0;
    
    Motor_Init(right_pan_tilt_motor);

    Motor left_pan_tilt_motor;
    left_pan_tilt_motor.GPIO_Direction_GPIOx = GPIOB;
    left_pan_tilt_motor.GPIO_Direction_Pin = GPIO_Pin_15;
	left_pan_tilt_motor.GPIO_Direction_RCC = RCC_AHBPeriph_GPIOB;
    left_pan_tilt_motor.GPIO_PWM_GPIOx = GPIOB;
    left_pan_tilt_motor.GPIO_PWM_Pin = GPIO_Pin_14;
    left_pan_tilt_motor.GPIO_PWM_PinSource = GPIO_PinSource14;
    left_pan_tilt_motor.GPIO_PWM_AF = GPIO_AF_1;
	left_pan_tilt_motor.GPIO_PWM_RCC = RCC_AHBPeriph_GPIOB;
    left_pan_tilt_motor.TIMx = TIM15;
    left_pan_tilt_motor.TIMx_Channel = TIM_Channel_1;
    left_pan_tilt_motor.TIMx_RCC = RCC_APB2Periph_TIM15;
    left_pan_tilt_motor.TIMx_Mode = TIM_OCMode_PWM1;
    left_pan_tilt_motor.direction = Bit_RESET;
    left_pan_tilt_motor.speed = 0;
    
    Motor_Init(left_pan_tilt_motor);

    float pan_position_Kp = 1;
    float pan_position_Ki = 0;
    float pan_position_Kd = 0;
    PID_Controller_Position pan_position_control(pan_position_Kp, pan_position_Ki, pan_position_Kd, motor_position_cmd_min, motor_position_cmd_max);

    float pan_speed_Kp = 1;
    float pan_speed_Ki = 0;
    float pan_speed_Kd = 0;
    PID_Controller_Speed pan_speed_control(pan_speed_Kp, pan_speed_Ki, pan_speed_Kd, motor_speed_cmd_min, motor_speed_cmd_max);

    Wheel_Controller_Pan pan_control(&pan_position_control, &pan_speed_control, left_pan_tilt_motor, right_pan_tilt_motor);

    float tilt_position_Kp = 1;
    float tilt_position_Ki = 0;
    float tilt_position_Kd = 0;
    PID_Controller_Position tilt_position_control(tilt_position_Kp, tilt_position_Ki, tilt_position_Kd, motor_position_cmd_min, motor_position_cmd_max);

    float tilt_speed_Kp = 1;
    float tilt_speed_Ki = 0;
    float tilt_speed_Kd = 0;
    PID_Controller_Speed tilt_speed_control(tilt_speed_Kp, tilt_speed_Ki, tilt_speed_Kd, motor_speed_cmd_min, motor_speed_cmd_max);

    Wheel_Controller_Tilt tilt_control(&tilt_position_control, &tilt_speed_control, left_pan_tilt_motor, right_pan_tilt_motor);

    mYellowON; mRedON; mGreenON; mWhiteON; mBlueON;
	DelayMilliseconds(1000);
    mYellowOFF;mRedOFF;mGreenOFF; mWhiteOFF; mBlueOFF;

    // int16_t left_wheel_position_goal = -16385;
    // left_wheel_position_pid.setPositionGoal(left_wheel_position_goal);

    // uint16_t left_wheel_speed_goal = -1000;
    // left_wheel_speed_pid.setSpeedGoal(left_wheel_speed_goal);

    int16_t left_wheel_position_goal = -32760;
    left_wheel_control.setPositionGoal(left_wheel_position_goal);

    int16_t left_wheel_speed_goal = -500;
    left_wheel_control.setSpeedGoal(left_wheel_speed_goal);

    int16_t right_wheel_position_goal = -22764;
    right_wheel_control.setPositionGoal(right_wheel_position_goal);

    int16_t right_wheel_speed_goal = -500;
    right_wheel_control.setSpeedGoal(right_wheel_speed_goal);

    int16_t pan_position_goal = -17000;
    pan_control.setPositionGoal(pan_position_goal);

    int16_t pan_speed_goal = -1300;
    pan_control.setSpeedGoal(pan_speed_goal);

    int16_t tilt_position_goal = 3000;
    tilt_control.setPositionGoal(tilt_position_goal);

    int16_t tilt_speed_goal = 36000;
    tilt_control.setSpeedGoal(tilt_speed_goal);
  //////////////////////////////////////////////////////////////////////////////
  // Main Loop
    while(1) {

      // int16_t left_wheel_current_position = +16385;
      // left_wheel_position_pid.updatePosition(left_wheel_current_position);
      // int8_t left_wheel_position_cmd = left_wheel_position_pid.updatePositionCmd(&usb);
      // if(left_wheel_position_cmd < 0)
      // {
      //   set_motor_direction(left_wheel_motor, Bit_RESET);
      //   set_motor_speed(left_wheel_motor, -left_wheel_position_cmd);
      // }
      // else
      // {
      //   set_motor_direction(left_wheel_motor, Bit_SET);
      //   set_motor_speed(left_wheel_motor, left_wheel_position_cmd);
      // }
      
      // int16_t left_wheel_current_speed = -950;
      // left_wheel_speed_pid.updateSpeed(left_wheel_current_speed);
      // int8_t left_wheel_speed_cmd = left_wheel_speed_pid.updateSpeedCmd(&usb);
      // set_motor_speed(left_wheel_motor, left_wheel_speed_cmd);

      int16_t left_wheel_current_position = +32766;
      left_wheel_control.updatePosition(left_wheel_current_position);
      int8_t left_position_cmd = left_wheel_control.updatePositionCmd(&usb);

      int16_t left_wheel_current_speed = -950;
      left_wheel_control.updateSpeed(left_wheel_current_speed);
      int8_t left_speed_cmd = left_wheel_control.updateSpeedCmd(&usb);

      int16_t right_wheel_current_position = +32766;
      right_wheel_control.updatePosition(right_wheel_current_position);
      int8_t right_position_cmd = right_wheel_control.updatePositionCmd(&usb);

      int16_t right_wheel_current_speed = -950;
      right_wheel_control.updateSpeed(right_wheel_current_speed);
      int8_t right_speed_cmd = right_wheel_control.updateSpeedCmd(&usb);

      int16_t pan_current_position = 32767;
      pan_control.updatePosition(pan_current_position);
      int8_t pan_position_cmd = pan_control.updatePositionCmd(&usb);

      int16_t pan_current_speed = -350;
      pan_control.updateSpeed(pan_current_speed);
      int8_t pan_speed_cmd = pan_control.updateSpeedCmd(&usb);

      int16_t tilt_current_position = 1000;
      tilt_control.updatePosition(tilt_current_position);
      int8_t tilt_position_cmd = tilt_control.updatePositionCmd(&usb);

      int16_t tilt_current_speed = 3000;
      tilt_control.updateSpeed(tilt_current_speed);
      int8_t tilt_speed_cmd = tilt_control.updateSpeedCmd(&usb);
      
      usb.printf("lp cmd = %d\n\r", left_position_cmd);
      usb.printf("ls cmd = %d\n\r", left_speed_cmd);
      usb.printf("rp cmd = %d\n\r", right_position_cmd);
      usb.printf("rs cmd = %d\n\r", right_speed_cmd);
      usb.printf("pp cmd = %d\n\r", pan_position_cmd);
      usb.printf("ps cmd = %d\n\r", pan_speed_cmd);
      usb.printf("tp cmd = %d\n\r", tilt_position_cmd);
      usb.printf("ts cmd = %d\n\r", tilt_speed_cmd);
      usb.SendNow();
      
      DelayMilliseconds(10);
      // mRedTOGGLE;
      // mYellowTOGGLE;
      // mGreenTOGGLE;
      // mWhiteTOGGLE;
      // mBlueTOGGLE;
 
    } // while(1)
    return(0);
}

