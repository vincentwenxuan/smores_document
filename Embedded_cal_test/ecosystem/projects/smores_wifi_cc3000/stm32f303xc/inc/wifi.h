#ifndef wifi_H
#define wifi_H

#include "project_message_types.h"
#include "Wheel_Control.h"
#include "Trajectory.h"


#define LEFT_FACE_PING   121
#define RIGHT_FACE_PING  122
#define TOP_FACE_PING    123
#define BOTTOM_FACE_PING 124

/*   face macros:   */
#define LEFT_FACE 21  // left
#define RIGHT_FACE 22  // right
#define TOP_FACE 23  // top
#define BOTTOM_FACE 24  // bottom

/*   face i2c address macros:  */
#define BOTTOM_FACE_I2C_ADDRESS 0x0B
#define LEFT_FACE_I2C_ADDRESS   0x0A
#define RIGHT_FACE_I2C_ADDRESS  0x0C
#define TOP_FACE_I2C_ADDRESS    0x0D

/*   face board macros:   */
#define MAGNETS_ON  0xA2
#define MAGNETS_OFF 0xA3
#define FACE_PING   0xA5

// feecback cmd
#define GET_STATES 0xA4

#define UPDATE_GOAL 10
#define SEND_DATA   20

#define INIT 50
#define UPDATE_PARAMETER 10
#define SEND_PARAMETER 20
#define BEGIN 30
#define STOP  40
#define DOF 21
#define LEFT_DOF 21
#define RIGHT_DOF 22
#define TILT_DOF 23
#define PAN_DOF 24

typedef struct __attribute__ ((__packed__))
{
	MsgBrainIp brain_ip_struct; 
	MsgCmd cmd_struct;
	MsgMagnet magnet_struct;
	MsgModule module_struct;
	MsgStates states_struct;
	MsgControl control_struct;
	MsgCurrentState currentstate_struct;
	MsgTrajectoryPosition trajectory_pos_struct; 
	MsgTrajectoryPositionTX trajectory_postx_struct;
	MsgTrajectoryVelocity trajectory_vel_struct;
	MsgTrajectoryVelocityTX trajectory_veltx_struct;
	MsgTorque torque_struct; 

	Motor* wheel_motor[4]; 
	PID_State* wheel_currentState[4];
	PID_State* wheel_stateGoal[4]; 

	Wheel_Controller_OneMotor* wheel_control[2]; 
	Wheel_Controller_Pan_Tilt* wheel_control_pt; 

	// Trajectory globals // 
	Trajectory* traj[4];
	Polynomial* poly[4]; 
	Polynomial* poly_speed[3]; // pan_tilt combined
	Trajectory* traj_speed[3];


	
	float pos_coeff[6]; 
	float* path_pos[4];
	float* velocity_pos[4];

	int16_t current_position[4];  // check the type 
	int32_t current_velocity[4]; 
	int8_t motor_cmd[4]; 

	float duration[4];
	float startTime[4];
	int timer_counter[4];
	float endTime[4];
	uint16_t sectionNum[4];
	uint16_t update_times[4]; 
	bool init_flag[4]; 


    bool power_on_flag;
    bool counter_power_on[4];
    int16_t counter[4]; 
    int16_t index_position[4]; 
    bool global_flag[4]; 
    
    
    // ** torque control flag ** //
    // initial values are 0 meaning OFF, 1 means turning on torque control mode

    bool torque_flag[3];


} DATA_MANAGER_t;

void wifi_send(void* data_to_send, long data_size);
void wifi_receive_callback(DATA_MANAGER_t* data_manager);

// PID_State left_wheel_stateGoal; 
// PID_State right_wheel_stateGoal; 
// PID_State pan_wheel_stateGoal;
// PID_State tilt_wheel_stateGoal;
// 


// Polynomial polytest[4]; 
// Trajectory trajectory[4];
// Polynomial p_speed[4];
// Trajectory trajectory_speed[4];
// PID_State wheel_currentState[4];

// Polynomial* poly[4]; 
// Trajectory* trajectory[4];
// Polynomial* p_speed[4];
// Trajectory* trajectory_speed[4];
// PID_State* wheel_currentState[4];
// PID_State* wheel_stateGoal[4]; 

// int16_t current_position[4];  // check the type 
// int32_t current_velocity[4]; 
// int8_t motor_cmd[4]; 


#endif