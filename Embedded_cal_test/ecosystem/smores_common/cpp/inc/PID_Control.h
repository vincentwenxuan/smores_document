/**
  *****************************************************************************************
  * @file    PID_Control.h 
  * @author  Chao Liu (chao.liu0307@gmail.com)
  * @version V2
  * @date    11-November-2014
  * @brief   This header file is used for Trajectory PID Control on STM32F303
  *****************************************************************************************
*/

#ifndef PID_CONTROL_H_
#define PID_CONTROL_H_

#include <math.h>
#include <stdlib.h>
#include "system_clock.h"
#include "usb_interface.h"

typedef struct _pid_state {
	int16_t Position;
	int32_t Speed;
}PID_State;

class PID_Controller {
public:
	PID_Controller(float Kp, float Ki, float Kd, int8_t motor_cmd_min, uint8_t motor_cmd_max);
	void updateState(PID_State currentState);
	void setStateGoal(PID_State stateGoal);
	PID_State returnCurrentState(void);
	int8_t updateStateCmd(UsbInterface *debug);

private:
	float _Kp;
	float _Ki;
	float _Kd;

	int8_t _motor_cmd_min;
	uint8_t _motor_cmd_max;

	struct Time currentTime;
	struct Time previousTime;

	float previous_error;
	float integral;
	
	PID_State _stateGoal;
	PID_State _currentState;
};


#endif
