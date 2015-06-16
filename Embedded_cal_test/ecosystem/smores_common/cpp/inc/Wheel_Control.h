/**
  *****************************************************************************************
  * @file    Wheel_Control.h 
  * @author  Chao Liu (chao.liu0307@gmail.com)
  * @version V2
  * @date    10-November-2014
  * @brief   This header file is used for Wheel Control on STM32F303
  *****************************************************************************************
*/

#ifndef WHEEL_CONTROL_H_
#define WHEEL_CONTROL_H_

#include "PID_Control.h"
#include "DC_Motor.h"

class Wheel_Controller_OneMotor {
public:
	Wheel_Controller_OneMotor(PID_Controller *Wheel_Control, Motor motor);
	void updateState(PID_State currentState);
	void setStateGoal(PID_State stateGoal);
	PID_State returnCurrentState(void);
	int8_t updateStateCmd(UsbInterface *debug);
	

private:
	PID_Controller *_Wheel_Control;
	Motor _motor;
};

class Wheel_Controller_Pan {
public:
	Wheel_Controller_Pan(PID_Controller *Wheel_Control, Motor motor_left, Motor motor_right);
	void updateState(PID_State currentState);
	void setStateGoal(PID_State stateGoal);
	PID_State returnCurrentState(void);
	int8_t updateStateCmd(UsbInterface *debug);

private:
	PID_Controller *_Wheel_Control;
	Motor _motor_right;
	Motor _motor_left;
};

class Wheel_Controller_Tilt {
public:
        Wheel_Controller_Tilt(PID_Controller *Wheel_Control, Motor motor_left, Motor motor_right);
	void updateState(PID_State currentState);
	void setStateGoal(PID_State stateGoal);
	PID_State returnCurrentState(void);
        int8_t updateStateCmd(UsbInterface *debug);

private:
	PID_Controller *_Wheel_Control;
	Motor _motor_right;
	Motor _motor_left;
};

class Wheel_Controller_Pan_Tilt {
public:
    Wheel_Controller_Pan_Tilt(PID_Controller *Wheel_Control_Pan, PID_Controller *Wheel_Control_Tilt, Motor motor_left, Motor motor_right);
    void updateState(PID_State currentStatePan, PID_State currentStateTilt);
    void setStateGoal(PID_State stateGoalPan, PID_State stateGoalTilt);
    PID_State returnCurrentPanState(void);
    PID_State returnCurrentTiltState(void);
    void updateStateCmd(int8_t *motor_cmd_left, int8_t *motor_cmd_right, UsbInterface *debug);

private:
    PID_Controller *_Wheel_Control_Pan;
    PID_Controller *_Wheel_Control_Tilt;
    Motor _motor_right;
    Motor _motor_left;
};


#endif
