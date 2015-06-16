/**
  *****************************************************************************************
  * @file    Wheel_Control.cpp 
  * @author  Chao Liu (chao.liu0307@gmail.com)
  * @version V2
  * @date    10-November-2014
  * @brief   This source file is used for Wheel Control on STM32F303
  *****************************************************************************************
*/

#include "Wheel_Control.h"

Wheel_Controller_OneMotor::Wheel_Controller_OneMotor(PID_Controller *Wheel_Control, Motor motor)
{
  _Wheel_Control = Wheel_Control;
  _motor = motor;
}

//**************************************************************************************
//
//!  updateState
//!
//!  @param    currentState
//!          
//!  @return   none
//!
//!  @brief    This function is used to update the current state for the one-motor wheel
//!            controller.   
//
//**************************************************************************************
void Wheel_Controller_OneMotor::updateState(PID_State currentState)
{
  _Wheel_Control->updateState(currentState);
}

//**************************************************************************************
//
//!  setStateGoal
//!
//!  @param    stateGoal
//!          
//!  @return   none
//!
//!  @brief    This function is used to set the desired state for the one-motor wheel
//!            controller.   
//
//**************************************************************************************
void Wheel_Controller_OneMotor::setStateGoal(PID_State stateGoal)
{
  _Wheel_Control->setStateGoal(stateGoal);
}

//**************************************************************************************
//
//!  returnCurrentState
//!
//!  @param    none
//!          
//!  @return   current state
//!
//!  @brief    This function is used to return the current state for the one-motor wheel
//!            controller.   
//
//**************************************************************************************
PID_State Wheel_Controller_OneMotor::returnCurrentState(void)
{
  return _Wheel_Control->returnCurrentState();
}

//**************************************************************************************
//
//!  updateStateCmd
//!
//!  @param    *debug
//!          
//!  @return   cmd
//!
//!  @brief    This function is used to output the cmd from the one-motor wheel
//!            controller.   
//
//**************************************************************************************
int8_t Wheel_Controller_OneMotor::updateStateCmd(UsbInterface *debug)
{
  int8_t cmd = _Wheel_Control->updateStateCmd(debug);
  //debug->printf("pos cmd = %d\n\r", cmd);
  //debug->SendNow();
  if(cmd < 0)
  {
    set_motor_direction(_motor, Bit_SET);
    set_motor_speed(_motor, -cmd);
  }
  else
  {
    set_motor_direction(_motor, Bit_RESET);
    set_motor_speed(_motor, cmd);
  }
  return cmd;
}

Wheel_Controller_Pan::Wheel_Controller_Pan(PID_Controller *Wheel_Control, Motor motor_left, Motor motor_right)
{
  _Wheel_Control = Wheel_Control;
  _motor_right = motor_right;
  _motor_left = motor_left;
}

//**************************************************************************************
//
//!  updateState
//!
//!  @param    currentState
//!
//!  @return   none
//!
//!  @brief    This function is used to update the current state for the pan wheel
//!            controller.
//
//**************************************************************************************
void Wheel_Controller_Pan::updateState(PID_State currentState)
{
  _Wheel_Control->updateState(currentState);
}

//**************************************************************************************
//
//!  setStateGoal
//!
//!  @param    stateGoal
//!
//!  @return   none
//!
//!  @brief    This function is used to set the desired state for the pan wheel
//!            controller.
//
//**************************************************************************************
void Wheel_Controller_Pan::setStateGoal(PID_State stateGoal)
{
  _Wheel_Control->setStateGoal(stateGoal);
}

//**************************************************************************************
//
//!  returnCurrentState
//!
//!  @param    none
//!
//!  @return   current state
//!
//!  @brief    This function is used to return the current state for the pan wheel
//!            controller.
//
//**************************************************************************************
PID_State Wheel_Controller_Pan::returnCurrentState(void)
{
  return _Wheel_Control->returnCurrentState();
}

//**************************************************************************************
//
//!  updateStateCmd
//!
//!  @param    *debug
//!          
//!  @return   cmd
//!
//!  @brief    This function is used to output the cmd for the pan wheel
//!            controller.   
//
//**************************************************************************************
int8_t Wheel_Controller_Pan::updateStateCmd(UsbInterface *debug)
{
  int8_t cmd = _Wheel_Control->updateStateCmd(debug);
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

Wheel_Controller_Tilt::Wheel_Controller_Tilt(PID_Controller *Wheel_Control, Motor motor_left, Motor motor_right)
{
  _Wheel_Control = Wheel_Control;
  _motor_right = motor_right;
  _motor_left = motor_left;
}

//**************************************************************************************
//
//!  updateState
//!
//!  @param    currentState
//!          
//!  @return   none
//!
//!  @brief    This function is used to update the current state for tilt wheel
//!            controller.   
//
//**************************************************************************************
void Wheel_Controller_Tilt::updateState(PID_State currentState)
{
  _Wheel_Control->updateState(currentState);
}

//**************************************************************************************
//
//!  setStateGoal
//!
//!  @param    stateGoal
//!          
//!  @return   none
//!
//!  @brief    This function is used to set the desired state for tilt wheel
//!            controller.   
//
//**************************************************************************************
void Wheel_Controller_Tilt::setStateGoal(PID_State stateGoal)
{
  _Wheel_Control->setStateGoal(stateGoal);
}

//**************************************************************************************
//
//!  returnCurrentState
//!
//!  @param    none
//!          
//!  @return   current state
//!
//!  @brief    This function is used to return the current state for tilt wheel
//!            controller.   
//
//**************************************************************************************
PID_State Wheel_Controller_Tilt::returnCurrentState(void)
{
  return _Wheel_Control->returnCurrentState();
}

//**************************************************************************************
//
//!  updateStateCmd
//!
//!  @param    *debug
//!          
//!  @return   cmd
//!
//!  @brief    This function is used to output cmd for tilt wheel
//!            controller.   
//
//**************************************************************************************
int8_t Wheel_Controller_Tilt::updateStateCmd(UsbInterface *debug)
{
  int8_t cmd = _Wheel_Control->updateStateCmd(debug);
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

Wheel_Controller_Pan_Tilt::Wheel_Controller_Pan_Tilt(PID_Controller *Wheel_Control_Pan, PID_Controller *Wheel_Control_Tilt, Motor motor_left, Motor motor_right)
{
  _Wheel_Control_Pan = Wheel_Control_Pan;
  _Wheel_Control_Tilt = Wheel_Control_Tilt;
  _motor_right = motor_right;
  _motor_left = motor_left;
}

//**************************************************************************************
//
//!  updateState
//!
//!  @param    currentState
//!          
//!  @return   none
//!
//!  @brief    This function is used to update the current state for pan and tilt wheel
//!            controller.   
//
//**************************************************************************************
void Wheel_Controller_Pan_Tilt::updateState(PID_State currentStatePan, PID_State currentStateTilt)
{
  _Wheel_Control_Pan->updateState(currentStatePan);
  _Wheel_Control_Tilt->updateState(currentStateTilt);
}

//**************************************************************************************
//
//!  setStateGoal
//!
//!  @param    stateGoal
//!          
//!  @return   none
//!
//!  @brief    This function is used to set the desired state for pan tilt wheel
//!            controller.   
//
//**************************************************************************************
void Wheel_Controller_Pan_Tilt::setStateGoal(PID_State stateGoalPan, PID_State stateGoalTilt)
{
    _Wheel_Control_Pan->setStateGoal(stateGoalPan);
    _Wheel_Control_Tilt->setStateGoal(stateGoalTilt);
}

//**************************************************************************************
//
//!  returnCurrentState
//!
//!  @param    none
//!          
//!  @return   current state
//!
//!  @brief    This function is used to return the current state for pan wheel
//!            controller.   
//
//**************************************************************************************
PID_State Wheel_Controller_Pan_Tilt::returnCurrentPanState(void)
{
    return _Wheel_Control_Pan->returnCurrentState();
}

//**************************************************************************************
//
//!  returnCurrentState
//!
//!  @param    none
//!          
//!  @return   current state
//!
//!  @brief    This function is used to return the current state for tilt wheel
//!            controller.   
//
//**************************************************************************************
PID_State Wheel_Controller_Pan_Tilt::returnCurrentTiltState(void)
{
    return _Wheel_Control_Tilt->returnCurrentState();
}

//**************************************************************************************
//
//!  updateStateCmd
//!
//!  @param    *debug
//!          
//!  @return   cmd
//!
//!  @brief    This function is used to output cmd for tilt wheel
//!            controller.   
//
//**************************************************************************************
void Wheel_Controller_Pan_Tilt::updateStateCmd(int8_t *motor_cmd_left, int8_t *motor_cmd_right, UsbInterface *debug)
{
    int8_t cmd_pan = _Wheel_Control_Pan->updateStateCmd(debug);
    int8_t cmd_tilt = _Wheel_Control_Tilt->updateStateCmd(debug);
    int8_t cmd_left = cmd_pan - cmd_tilt;
    int8_t cmd_right = cmd_pan + cmd_tilt;
    // debug->printf("cp:%d\n\r", cmd_pan);
    // debug->printf("ct:%d\n\r", cmd_tilt);
    // debug->printf("cl:%d\n\r", cmd_left);
    // debug->printf("cr:%d\n\r", cmd_right);
    // debug->SendNow();
    if(cmd_left < 0)
    {
        set_motor_direction(_motor_left, Bit_RESET);
        set_motor_speed(_motor_left, -cmd_left);
    }
    else
    {
        set_motor_direction(_motor_left, Bit_SET);
        set_motor_speed(_motor_left, cmd_left);
    }

    if(cmd_right < 0)
    {
        set_motor_direction(_motor_right, Bit_RESET);
        set_motor_speed(_motor_right, -cmd_right);
    }
    else
    {
        set_motor_direction(_motor_right, Bit_SET);
        set_motor_speed(_motor_right, cmd_right);
    }
    *motor_cmd_left = cmd_left;
    *motor_cmd_right = cmd_right;
}
