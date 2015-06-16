/**
  *****************************************************************************************
  * @file    PID_Control.cpp 
  * @author  Chao Liu (chao.liu0307@gmail.com)
  * @version V1
  * @date    24-October-2014
  * @brief   This source file is used for PID Motor Control on STM32F303
  *****************************************************************************************
*/

#include "PID_Control.h"

#define MIN(x,y)  ((x)<=(y))?(x):(y)

PID_Controller_Position::PID_Controller_Position(float Kp, float Ki, float Kd, int8_t motor_cmd_min, uint8_t motor_cmd_max)
{
  _Kp = Kp;
  _Ki = Ki;
  _Kd = Kd;
  _motor_cmd_min = motor_cmd_min;
  _motor_cmd_max = motor_cmd_max;
  currentTime.sec = 0;
  currentTime.nsec = 0;
  previousTime.sec = 0;
  previousTime.nsec = 0;
  _positionGoal = 0;
  _currentPosition = 0;
  previous_error = 0;
  integral = 0;
}

//**************************************************************************************
//
//!  updatePosition
//!
//!  @param    currentPosition
//!          
//!  @return   none
//!
//!  @brief    This function is used to update the current position for the PID controller.   
//
//**************************************************************************************
void PID_Controller_Position::updatePosition(int16_t currentPosition)
{
  _currentPosition = currentPosition;
}

//**************************************************************************************
//
//!  setPositionGoal
//!
//!  @param    positionGoal
//!          
//!  @return   none
//!
//!  @brief    This function is used to set position goal for the PID controller.
//
//**************************************************************************************
void PID_Controller_Position::setPositionGoal(int16_t positionGoal)
{
  _positionGoal = positionGoal;
}

//**************************************************************************************
//
//!  returnCurrentPosition
//!
//!  @param    void
//!          
//!  @return   _currentPosotion
//!
//!  @brief    This function is used to return the current position in the PID controller.
//
//**************************************************************************************
int16_t PID_Controller_Position::returnCurrentPosition(void)
{
  return _currentPosition;
}

//**************************************************************************************
//
//!  updatePositionCmd
//!
//!  @param    *debug
//!          
//!  @return   output
//!
//!  @brief    This function is used to output the cmd from the PID controller.
//
//**************************************************************************************
int8_t PID_Controller_Position::updatePositionCmd(UsbInterface *debug)
{
  struct Time t = GetTime();
  previousTime = currentTime;
  currentTime = t;
  struct Time dt = SubtractTime(currentTime, previousTime);
  float delta_time = dt.sec + (float)dt.nsec/1e9;
  //debug->printf("dt = %f\n\r", delta_time);

  int32_t error = _positionGoal - _currentPosition;
  int32_t error_temp = 65535 - abs(error);
  if(abs(error) >= 32767)
    error = error_temp * (-error/abs(error));
  //  debug->printf("error = %d\n\r", error);
  integral = integral + error * delta_time;
  float derivative = ((float)(error - previous_error))/delta_time;
  previous_error = error;
  int output = _Kp * error + _Ki * integral + _Kd * derivative + 0.5;
  //debug->printf("cmd = %d\n\r", output);

  if(output < _motor_cmd_min)
    output = _motor_cmd_min;
  if(output > _motor_cmd_max)
    output = _motor_cmd_max;

  return output;
}


PID_Controller_Speed::PID_Controller_Speed(float Kp, float Ki, float Kd, int8_t motor_cmd_min, uint8_t motor_cmd_max)
{
  _Kp = Kp;
  _Ki = Ki;
  _Kd = Kd;
  _motor_cmd_min = motor_cmd_min;
  _motor_cmd_max = motor_cmd_max;
  currentTime.sec = 0;
  currentTime.nsec = 0;
  previousTime.sec = 0;
  previousTime.nsec = 0;
  _speedGoal = 0;
  _currentSpeed = 0;
  previous_error = 0;
  integral = 0;
}

//**************************************************************************************
//
//!  updateSpeed
//!
//!  @param    currentSpeed
//!          
//!  @return   none
//!
//!  @brief    This function is used to update the current speed for the PID controller.
//
//**************************************************************************************
void PID_Controller_Speed::updateSpeed(int16_t currentSpeed)
{
  _currentSpeed = currentSpeed;
}

//**************************************************************************************
//
//!  setSpeedGoal
//!
//!  @param    speedGoal
//!          
//!  @return   none
//!
//!  @brief    This function is used to set the speed goal for the PID controller.
//
//**************************************************************************************
void PID_Controller_Speed::setSpeedGoal(int16_t speedGoal)
{
  _speedGoal = speedGoal;
}

//**************************************************************************************
//
//!  returnCurrentSpeed
//!
//!  @param    none
//!          
//!  @return   _currentSpeed
//!
//!  @brief    This function is used to return the current speed in the PID controller.
//
//**************************************************************************************
uint16_t PID_Controller_Speed::returnCurrentSpeed(void)
{
  return _currentSpeed;
}

//**************************************************************************************
//
//!  updateSpeedCmd
//!
//!  @param    *debug
//!          
//!  @return   output
//!
//!  @brief    This function is used to output the cmd from the PID controller.
//
//**************************************************************************************
int8_t PID_Controller_Speed::updateSpeedCmd(UsbInterface *debug)
{
  struct Time t = GetTime();
  previousTime = currentTime;
  currentTime = t;
  struct Time dt = SubtractTime(currentTime, previousTime);
  float delta_time = dt.sec + (float)dt.nsec/1e9;
  //debug->printf("dt = %f\n\r", delta_time);
  int32_t error = _speedGoal - _currentSpeed;
  //debug->printf("error = %d\n\r", error);
  integral = integral + error * delta_time;
  float derivative = ((float)(error - previous_error))/delta_time;
  previous_error = error;
  float setPoint = 0;
  if(_speedGoal >= 0)
    setPoint = (float)_speedGoal/32767*_motor_cmd_max;
  else
    setPoint = (float)_speedGoal/32767*abs(_motor_cmd_min);
  int output = setPoint + _Kp * error + _Ki * integral + _Kd * derivative + 0.5;
  //debug->printf("output = %d\n\r", output);

  if(output < _motor_cmd_min)
    output = _motor_cmd_min;
  if(output > _motor_cmd_max)
    output = _motor_cmd_max;

  return output;
}
