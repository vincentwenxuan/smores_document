/**
  *****************************************************************************************
  * @file    PID_Control.cpp 
  * @author  Chao Liu (chao.liu0307@gmail.com)
  * @version V2
  * @date    11-November-2014
  * @brief   This source file is used for Trajectory PID Motor Control on STM32F303
  *****************************************************************************************
*/

#include "PID_Control.h"

#define MIN(x,y)  ((x)<=(y))?(x):(y)

PID_Controller::PID_Controller(float Kp, float Ki, float Kd, int8_t motor_cmd_min, uint8_t motor_cmd_max)
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
  _stateGoal.Position = 0;
  _stateGoal.Speed = 0;
  _currentState.Position = 0;
  _currentState.Speed = 0;
  previous_error = 0;
  integral = 0;
}

//**************************************************************************************
//
//!  updateState
//!
//!  @param    currentState
//!          
//!  @return   none
//!
//!  @brief    This function is used to update the current state for the PID controller.   
//
//**************************************************************************************
void PID_Controller::updateState(PID_State currentState)
{
  _currentState = currentState;
}

//**************************************************************************************
//
//!  setStateGoal
//!
//!  @param    stateGoal
//!          
//!  @return   none
//!
//!  @brief    This function is used to set state goal for the PID controller.
//
//**************************************************************************************
void PID_Controller::setStateGoal(PID_State stateGoal)
{
  _stateGoal = stateGoal;
}

//**************************************************************************************
//
//!  returnCurrentState
//!
//!  @param    void
//!          
//!  @return   _currentState
//!
//!  @brief    This function is used to return the current state in the PID controller.
//
//**************************************************************************************
PID_State PID_Controller::returnCurrentState(void)
{
  return _currentState;
}

//**************************************************************************************
//
//!  updateStateCmd
//!
//!  @param    *debug
//!          
//!  @return   output
//!
//!  @brief    This function is used to output the cmd from the PID controller.
//
//**************************************************************************************
int8_t PID_Controller::updateStateCmd(UsbInterface *debug)
{
  struct Time t = GetTime();
  previousTime = currentTime;
  currentTime = t;
  struct Time dt = SubtractTime(currentTime, previousTime);
  float delta_time = dt.sec + (float)dt.nsec/1e9;
  //debug->printf("dt = %f\n\r", delta_time);
  // debug->printf("gp = %d\n\r", _stateGoal.Position);
  // debug->printf("gs = %d\n\r", _stateGoal.Speed);
  // debug->printf("cp = %d\n\r", _currentState.Position);
  // debug->printf("cs = %d\n\r", _currentState.Speed);
  float error = _stateGoal.Position - _currentState.Position;
  float error_temp = 65535 - abs(error);
  if(abs(error) >= 32767)
    error = error_temp * (-error/abs(error));
  //debug->printf("error = %f\n\r", error);
  integral = integral + error * delta_time;
  float derivative = _stateGoal.Speed - _currentState.Speed;
  previous_error = error;
  int output = _Kp * error + _Ki * integral + _Kd * derivative + 0.5;
  // debug->printf("original cmd = %d\n\r", output);
  //debug->SendNow();
  if(output < _motor_cmd_min)
    output = _motor_cmd_min;
  if(output > _motor_cmd_max)
    output = _motor_cmd_max;

  return output;
}
