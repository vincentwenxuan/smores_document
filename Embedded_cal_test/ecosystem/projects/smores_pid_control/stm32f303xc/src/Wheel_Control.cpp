/**
  *****************************************************************************************
  * @file    Wheel_Control.cpp 
  * @author  Chao Liu (chao.liu0307@gmail.com)
  * @version V1
  * @date    29-October-2014
  * @brief   This source file is used for Wheel Control on STM32F303
  *****************************************************************************************
*/

#include "Wheel_Control.h"

Wheel_Controller_OneMotor::Wheel_Controller_OneMotor(PID_Controller_Position *Wheel_Position_Control, PID_Controller_Speed *Wheel_Speed_Control, Motor motor)
{
  _Wheel_Position_Control = Wheel_Position_Control;
  _Wheel_Speed_Control = Wheel_Speed_Control;
  _motor = motor;
}

//**************************************************************************************
//
//!  updatePosition
//!
//!  @param    currentPosition
//!          
//!  @return   none
//!
//!  @brief    This function is used to update the current position for the one-motor wheel
//!            controller.   
//
//**************************************************************************************
void Wheel_Controller_OneMotor::updatePosition(int16_t currentPosition)
{
  _Wheel_Position_Control->updatePosition(currentPosition);
}

//**************************************************************************************
//
//!  updateSpeed
//!
//!  @param    currentSpeed
//!          
//!  @return   none
//!
//!  @brief    This function is used to update the current speed for the one-motor wheel
//!            controller.   
//
//**************************************************************************************
void Wheel_Controller_OneMotor::updateSpeed(int16_t currentSpeed)
{
  _Wheel_Speed_Control->updateSpeed(currentSpeed);
}

//**************************************************************************************
//
//!  setPositionGoal
//!
//!  @param    positionGoal
//!          
//!  @return   none
//!
//!  @brief    This function is used to set the desired position for the one-motor wheel
//!            controller.   
//
//**************************************************************************************
void Wheel_Controller_OneMotor::setPositionGoal(int16_t positionGoal)
{
  _Wheel_Position_Control->setPositionGoal(positionGoal);
}

//**************************************************************************************
//
//!  setSpeedGoal
//!
//!  @param    speedGoal
//!          
//!  @return   none
//!
//!  @brief    This function is used to set the desired speed for the ont-motor wheel
//!            controller.   
//
//**************************************************************************************
void Wheel_Controller_OneMotor::setSpeedGoal(int16_t speedGoal)
{
  _Wheel_Speed_Control->setSpeedGoal(speedGoal);
}

//**************************************************************************************
//
//!  returnCurrentPosition
//!
//!  @param    none
//!          
//!  @return   current position
//!
//!  @brief    This function is used to return the current position for the one-motor wheel
//!            controller.   
//
//**************************************************************************************
int16_t Wheel_Controller_OneMotor::returnCurrentPosition(void)
{
  return _Wheel_Position_Control->returnCurrentPosition();
}

//**************************************************************************************
//
//!  returnCurrentSpeed
//!
//!  @param    none
//!          
//!  @return   current speed
//!
//!  @brief    This function is used to return the current speed for the one-motor wheel
//!            controller.   
//
//**************************************************************************************
int16_t Wheel_Controller_OneMotor::returnCurrentSpeed(void)
{
  return _Wheel_Speed_Control->returnCurrentSpeed();
}

//**************************************************************************************
//
//!  updatePositionCmd
//!
//!  @param    *debug
//!          
//!  @return   cmd
//!
//!  @brief    This function is used to output the position cmd from the one-motor wheel
//!            controller.   
//
//**************************************************************************************
int8_t Wheel_Controller_OneMotor::updatePositionCmd(UsbInterface *debug)
{
  int8_t cmd = _Wheel_Position_Control->updatePositionCmd(debug);
  //debug->printf("pos cmd = %d\n\r", cmd);
  //debug->SendNow();
  if(cmd < 0)
  {
    set_motor_direction(_motor, Bit_RESET);
    set_motor_speed(_motor, -cmd);
  }
  else
  {
    set_motor_direction(_motor, Bit_SET);
    set_motor_speed(_motor, cmd);
  }
  return cmd;
}

//**************************************************************************************
//
//!  updateSpeedCmd
//!
//!  @param    *debug
//!          
//!  @return   cmd
//!
//!  @brief    This function is used to output the speed cmd from the one-motor wheel
//!            controller.   
//
//**************************************************************************************
int8_t Wheel_Controller_OneMotor::updateSpeedCmd(UsbInterface *debug)
{
  int8_t cmd = _Wheel_Speed_Control->updateSpeedCmd(debug);
  //debug->printf("sp cmd = %d\n\r", cmd);
  //debug->SendNow();
  if(cmd < 0)
  {
    set_motor_direction(_motor, Bit_RESET);
    set_motor_speed(_motor, -cmd);
  }
  else
  {
    set_motor_direction(_motor, Bit_SET);
    set_motor_speed(_motor, cmd);
  }
  return cmd;
}

Wheel_Controller_Pan::Wheel_Controller_Pan(PID_Controller_Position *Wheel_Position_Control, PID_Controller_Speed *Wheel_Speed_Control, Motor motor_left, Motor motor_right)
{
  _Wheel_Position_Control = Wheel_Position_Control;
  _Wheel_Speed_Control = Wheel_Speed_Control;
  _motor_right = motor_right;
  _motor_left = motor_left;
}

//**************************************************************************************
//
//!  updatePosition
//!
//!  @param    currentPosition
//!          
//!  @return   none
//!
//!  @brief    This function is used to update the current position for the pan wheel
//!            controller.   
//
//**************************************************************************************
void Wheel_Controller_Pan::updatePosition(int16_t currentPosition)
{
  _Wheel_Position_Control->updatePosition(currentPosition);
}

//**************************************************************************************
//
//!  updateSpeed
//!
//!  @param    currentSpeed
//!          
//!  @return   none
//!
//!  @brief    This function is used to update the current speed for the pan wheel
//!            controller.   
//
//**************************************************************************************
void Wheel_Controller_Pan::updateSpeed(int16_t currentSpeed)
{
  _Wheel_Speed_Control->updateSpeed(currentSpeed);
}

//**************************************************************************************
//
//!  setPositionGoal
//!
//!  @param    positionGoal
//!          
//!  @return   none
//!
//!  @brief    This function is used to set the desired position for the pan wheel
//!            controller.   
//
//**************************************************************************************
void Wheel_Controller_Pan::setPositionGoal(int16_t positionGoal)
{
  _Wheel_Position_Control->setPositionGoal(positionGoal);
}

//**************************************************************************************
//
//!  setSpeedGoal
//!
//!  @param    speedGoal
//!          
//!  @return   none
//!
//!  @brief    This function is used to set the desired speed for the pan wheel
//!            controller.   
//
//**************************************************************************************
void Wheel_Controller_Pan::setSpeedGoal(int16_t speedGoal)
{
  _Wheel_Speed_Control->setSpeedGoal(speedGoal);
}

//**************************************************************************************
//
//!  returnCurrentPositon
//!
//!  @param    none
//!          
//!  @return   current position
//!
//!  @brief    This function is used to return the current position for the pan wheel
//!            controller.   
//
//**************************************************************************************
int16_t Wheel_Controller_Pan::returnCurrentPosition(void)
{
  return _Wheel_Position_Control->returnCurrentPosition();
}

//**************************************************************************************
//
//!  returnCurrentSpeed
//!
//!  @param    none
//!          
//!  @return   current speed
//!
//!  @brief    This function is used to return the current speed for the pan wheel
//!            controller.   
//
//**************************************************************************************
int16_t Wheel_Controller_Pan::returnCurrentSpeed(void)
{
  return _Wheel_Speed_Control->returnCurrentSpeed();
}

//**************************************************************************************
//
//!  updatePositionCmd
//!
//!  @param    *debug
//!          
//!  @return   cmd
//!
//!  @brief    This function is used to output the position cmd for the pan wheel
//!            controller.   
//
//**************************************************************************************
int8_t Wheel_Controller_Pan::updatePositionCmd(UsbInterface *debug)
{
  int8_t cmd = _Wheel_Position_Control->updatePositionCmd(debug);
  if(cmd < 0)
  {
    set_motor_direction(_motor_left, Bit_RESET);
    set_motor_direction(_motor_right, Bit_RESET);
    set_motor_speed(_motor_left, -cmd);
    set_motor_speed(_motor_right, -cmd);
  }
  else
  {
    set_motor_direction(_motor_left, Bit_SET);
    set_motor_direction(_motor_right, Bit_SET);
    set_motor_speed(_motor_left, cmd);
    set_motor_speed(_motor_right, cmd);
  }
  return cmd;
}

//**************************************************************************************
//
//!  updateSpeedCmd
//!
//!  @param    *debug
//!          
//!  @return   cmd
//!
//!  @brief    This function is used to output the speed cmd from pan wheel controller.   
//
//**************************************************************************************
int8_t Wheel_Controller_Pan::updateSpeedCmd(UsbInterface *debug)
{
  int8_t cmd = _Wheel_Speed_Control->updateSpeedCmd(debug);
  if(cmd < 0)
  {
    set_motor_direction(_motor_left, Bit_RESET);
    set_motor_direction(_motor_right, Bit_RESET);
    set_motor_speed(_motor_left, -cmd);
    set_motor_speed(_motor_right, -cmd);
  }
  else
  {
    set_motor_direction(_motor_left, Bit_SET);
    set_motor_direction(_motor_right, Bit_SET);
    set_motor_speed(_motor_left, cmd);
    set_motor_speed(_motor_right, cmd);
  }
  return cmd;
}

Wheel_Controller_Tilt::Wheel_Controller_Tilt(PID_Controller_Position *Wheel_Position_Control, PID_Controller_Speed *Wheel_Speed_Control, Motor motor_left, Motor motor_right)
{
  _Wheel_Position_Control = Wheel_Position_Control;
  _Wheel_Speed_Control = Wheel_Speed_Control;
  _motor_right = motor_right;
  _motor_left = motor_left;
}

//**************************************************************************************
//
//!  updatePosition
//!
//!  @param    currentPosition
//!          
//!  @return   none
//!
//!  @brief    This function is used to update the current position for tilt wheel
//!            controller.   
//
//**************************************************************************************
void Wheel_Controller_Tilt::updatePosition(int16_t currentPosition)
{
  _Wheel_Position_Control->updatePosition(currentPosition);
}

//**************************************************************************************
//
//!  updateSpeed
//!
//!  @param    currentSpeed
//!          
//!  @return   none
//!
//!  @brief    This function is used to update the current speed for tilt wheel
//!            controller.   
//
//**************************************************************************************
void Wheel_Controller_Tilt::updateSpeed(int16_t currentSpeed)
{
  _Wheel_Speed_Control->updateSpeed(currentSpeed);
}

//**************************************************************************************
//
//!  setPositionGoal
//!
//!  @param    positionGoal
//!          
//!  @return   none
//!
//!  @brief    This function is used to set the desired position for tilt wheel
//!            controller.   
//
//**************************************************************************************
void Wheel_Controller_Tilt::setPositionGoal(int16_t positionGoal)
{
  _Wheel_Position_Control->setPositionGoal(positionGoal);
}

//**************************************************************************************
//
//!  setSpeedGoal
//!
//!  @param    speedGoal
//!          
//!  @return   none
//!
//!  @brief    This function is used to set the desired speed for tilt wheel
//!            controller.   
//
//**************************************************************************************
void Wheel_Controller_Tilt::setSpeedGoal(int16_t speedGoal)
{
  _Wheel_Speed_Control->setSpeedGoal(speedGoal);
}

//**************************************************************************************
//
//!  returnCurrentPosition
//!
//!  @param    none
//!          
//!  @return   current position
//!
//!  @brief    This function is used to return the current position for tilt wheel
//!            controller.   
//
//**************************************************************************************
int16_t Wheel_Controller_Tilt::returnCurrentPosition(void)
{
  return _Wheel_Position_Control->returnCurrentPosition();
}

//**************************************************************************************
//
//!  returnCurrentSpeed
//!
//!  @param    none
//!          
//!  @return   current speed
//!
//!  @brief    This function is used to return the current speed for tilt wheel
//!            controller.   
//
//**************************************************************************************
int16_t Wheel_Controller_Tilt::returnCurrentSpeed(void)
{
  return _Wheel_Speed_Control->returnCurrentSpeed();
}

//**************************************************************************************
//
//!  updatePositionCmd
//!
//!  @param    *debug
//!          
//!  @return   cmd
//!
//!  @brief    This function is used to output position cmd for tilt wheel
//!            controller.   
//
//**************************************************************************************
int8_t Wheel_Controller_Tilt::updatePositionCmd(UsbInterface *debug)
{
  int8_t cmd = _Wheel_Position_Control->updatePositionCmd(debug);
  if(cmd < 0)
  {
    set_motor_direction(_motor_left, Bit_RESET);
    set_motor_direction(_motor_right, Bit_SET);
    set_motor_speed(_motor_left, -cmd);
    set_motor_speed(_motor_right, -cmd);
  }
  else
  {
    set_motor_direction(_motor_left, Bit_SET);
    set_motor_direction(_motor_right, Bit_RESET);
    set_motor_speed(_motor_left, cmd);
    set_motor_speed(_motor_right, cmd);
  }
  return cmd;
}

//**************************************************************************************
//
//!  updateSpeedCmd
//!
//!  @param    *debug
//!          
//!  @return   cmd
//!
//!  @brief    This function is used to output speed cmd for tilt wheel
//!            controller.   
//
//**************************************************************************************
int8_t Wheel_Controller_Tilt::updateSpeedCmd(UsbInterface *debug)
{
  int8_t cmd = _Wheel_Speed_Control->updateSpeedCmd(debug);
  if(cmd < 0)
  {
    set_motor_direction(_motor_left, Bit_RESET);
    set_motor_direction(_motor_right, Bit_SET);
    set_motor_speed(_motor_left, -cmd);
    set_motor_speed(_motor_right, -cmd);
  }
  else
  {
    set_motor_direction(_motor_left, Bit_SET);
    set_motor_direction(_motor_right, Bit_RESET);
    set_motor_speed(_motor_left, cmd);
    set_motor_speed(_motor_right, cmd);
  }
  return cmd;
}
