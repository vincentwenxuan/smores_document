/**
  *****************************************************************************************
  * @file    PID_Control.h 
  * @author  Chao Liu (chao.liu0307@gmail.com)
  * @version V1
  * @date    24-October-2014
  * @brief   This header file is used for PID Control on STM32F303
  *****************************************************************************************
*/

#ifndef PID_CONTROL_H_
#define PID_CONTROL_H_

#include <math.h>
#include <stdlib.h>
#include "system_clock.h"
#include "usb_interface.h"

class PID_Controller_Position {
public:
	PID_Controller_Position(float Kp, float Ki, float Kd, int8_t motor_cmd_min, uint8_t motor_cmd_max);
	void updatePosition(int16_t currentPosition);
	void setPositionGoal(int16_t positionGoal);
	int16_t returnCurrentPosition(void);
	int8_t updatePositionCmd(UsbInterface *debug);

private:
	float _Kp;
	float _Ki;
	float _Kd;

	int8_t _motor_cmd_min;
	uint8_t _motor_cmd_max;

	struct Time currentTime;
	struct Time previousTime;

	int32_t previous_error;
	float integral;
	
	int16_t _positionGoal;
	int16_t _currentPosition;
};

class PID_Controller_Speed {
public:
	PID_Controller_Speed(float Kp, float Ki, float Kd, int8_t motor_cmd_min, uint8_t motor_cmd_max);
	void updateSpeed(int16_t currentSpeed);
	void setSpeedGoal(int16_t speedGoal);
	uint16_t returnCurrentSpeed(void);
	int8_t updateSpeedCmd(UsbInterface *debug);

private:
	float _Kp;
	float _Ki;
	float _Kd;

	int8_t _motor_cmd_min;
	uint8_t _motor_cmd_max;

	struct Time currentTime;
	struct Time previousTime;

	int32_t previous_error;
	float integral;

	int16_t _speedGoal;
	int16_t _currentSpeed;
};


#endif
