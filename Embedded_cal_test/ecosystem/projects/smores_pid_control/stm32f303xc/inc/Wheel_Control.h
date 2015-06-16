/**
  *****************************************************************************************
  * @file    Wheel_Control.h 
  * @author  Chao Liu (chao.liu0307@gmail.com)
  * @version V1
  * @date    29-October-2014
  * @brief   This header file is used for Wheel Control on STM32F303
  *****************************************************************************************
*/

#ifndef WHEEL_CONTROL_H_
#define WHEEL_CONTROL_H_

#include "PID_Control.h"
#include "DC_Motor.h"

class Wheel_Controller_OneMotor {
public:
	Wheel_Controller_OneMotor(PID_Controller_Position *Wheel_Position_Control, PID_Controller_Speed *Wheel_Speed_Control, Motor motor);
	void updatePosition(int16_t currentPosition);
	void updateSpeed(int16_t currentSpeed);
	void setPositionGoal(int16_t positionGoal);
	void setSpeedGoal(int16_t speedGoal);
	int16_t returnCurrentPosition(void);
	int16_t returnCurrentSpeed(void);
	int8_t updatePositionCmd(UsbInterface *debug);
	int8_t updateSpeedCmd(UsbInterface *debug);
	

private:
	PID_Controller_Position *_Wheel_Position_Control;
	PID_Controller_Speed *_Wheel_Speed_Control;
	Motor _motor;
};

class Wheel_Controller_Pan {
public:
	Wheel_Controller_Pan(PID_Controller_Position *Wheel_Position_Control, PID_Controller_Speed *Wheel_Speed_Control, Motor motor_left, Motor motor_right);
	void updatePosition(int16_t currentPosition);
	void updateSpeed(int16_t currentSpeed);
	void setPositionGoal(int16_t positionGoal);
	void setSpeedGoal(int16_t speedGoal);
	int16_t returnCurrentPosition(void);
	int16_t returnCurrentSpeed(void);
	int8_t updatePositionCmd(UsbInterface *debug);
	int8_t updateSpeedCmd(UsbInterface *debug);

private:
	PID_Controller_Position *_Wheel_Position_Control;
	PID_Controller_Speed *_Wheel_Speed_Control;
	Motor _motor_right;
	Motor _motor_left;
};

class Wheel_Controller_Tilt {
public:
	Wheel_Controller_Tilt(PID_Controller_Position *Wheel_Position_Control, PID_Controller_Speed *Wheel_Speed_Control, Motor motor_left, Motor motor_right);
	void updatePosition(int16_t currentPosition);
	void updateSpeed(int16_t currentSpeed);
	void setPositionGoal(int16_t positionGoal);
	void setSpeedGoal(int16_t speedGoal);
	int16_t returnCurrentPosition(void);
	int16_t returnCurrentSpeed(void);
    int8_t updatePositionCmd(UsbInterface *debug);
	int8_t updateSpeedCmd(UsbInterface *debug);

private:
	PID_Controller_Position *_Wheel_Position_Control;
	PID_Controller_Speed *_Wheel_Speed_Control;
	Motor _motor_right;
	Motor _motor_left;
};


#endif
