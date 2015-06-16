#include "common_peripherals.h"
#include "system_clock.h"
#include "mBus.h"
// common components 
#include "common_message_types.h" 
// common messages
#include "usb_interface.h"        

// common libraries
#include "project_message_types.h"
#include "error_reporting.h"
#include <string.h> 
//needed for memset
//#define   mBUS 1

// user libraries
#include "Wheel_Control.h"

// face ping request messages:
#define LEFT_FACE_PING   121
#define RIGHT_FACE_PING  122
#define TOP_FACE_PING    123
#define BOTTOM_FACE_PING 124

char slave_cmd

// when the calibration cmd is received, the program falls into a time loop.
void spin_cmd(void) {
	// set the Atmega to calibration mode
	slave_cmd = CALI_ON;
	mBusWriteNoAdd(BOTTOM_FACE_I2C_ADDRESS << 1, slave_cmd, 1);
	mBusWriteNoAdd(LEFT_FACE_I2C_ADDRESS << 1, slave_cmd, 1);
	mBusWriteNoAdd(RIGHT_FACE_I2C_ADDRESS << 1, slave_cmd, 1);
	mBusWriteNoAdd(TOP_FACE_I2C_ADDRESS << 1, slave_cmd, 1);
	
	// control the motor
	MsgTorque *rx_msg = ((MsgTorque*) (rx_data+1));
	set_motor_direction(left_wheel_motor, Bit_RESET);
	set_motor_direction(right_wheel_motor, Bit_SET);
	set_motor_speed(left_wheel_motor, rx_msg->speed);
	set_motor_speed(right_wheel_motor, rx_msg->speed);

	//wait some seconds
	m_wait(10000);

	//stop
	set_motor_speed(left_wheel_motor, 0);
	set_motor_speed(right_wheel_motor, 0);

	//wait some seconds
	m_wait(1000);

	// tell the Atmega to finish the calibration mode
	slave_cmd = CALI_OFF;
	mBusWriteNoAdd(BOTTOM_FACE_I2C_ADDRESS << 1, slave_cmd, 1);
	mBusWriteNoAdd(LEFT_FACE_I2C_ADDRESS << 1, slave_cmd, 1);
	mBusWriteNoAdd(RIGHT_FACE_I2C_ADDRESS << 1, slave_cmd, 1);
	mBusWriteNoAdd(TOP_FACE_I2C_ADDRESS << 1, slave_cmd, 1);
	
	//wait some seconds
	m_wait(1000);

	//check if the EEPROM get the calibration parameter
	//mBusRead.....
}
