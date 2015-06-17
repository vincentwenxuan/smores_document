#include "stm_cali.h"

char slave_cmd;

// when the calibration cmd is received, the program falls into a time loop.
void cali_cmd(Motor left_wheel_motor, Motor right_wheel_motor, Motor right_pan_tilt_motor, Motor left_pan_tilt_motor) {
	// set the Atmega to calibration mode
	slave_cmd = CALI_ON;
	mBusWriteNoAdd(BOTTOM_FACE_I2C_ADDRESS << 1, slave_cmd, 1);
	mBusWriteNoAdd(LEFT_FACE_I2C_ADDRESS << 1, slave_cmd, 1);
	mBusWriteNoAdd(RIGHT_FACE_I2C_ADDRESS << 1, slave_cmd, 1);
	mBusWriteNoAdd(TOP_FACE_I2C_ADDRESS << 1, slave_cmd, 1);
	
	// control the motor
	set_motor_direction(left_wheel_motor, Bit_RESET);
	set_motor_direction(right_wheel_motor, Bit_SET);
	set_motor_direction(left_pan_tilt_motor, Bit_SET);
	set_motor_direction(right_pan_tilt_motor, Bit_SET);
	set_motor_speed(left_wheel_motor, 60);
	set_motor_speed(right_wheel_motor, 60);
	set_motor_speed(left_pan_tilt_motor, 60);
	set_motor_speed(right_pan_tilt_motor, 60);

	//wait some seconds
	DelayMilliseconds(10000);

	//stop
	set_motor_speed(left_wheel_motor, 0);
	set_motor_speed(right_wheel_motor, 0);

	//wait some seconds
	DelayMilliseconds(1000);

	// tell the Atmega to finish the calibration mode
	slave_cmd = CALI_OFF;
	mBusWriteNoAdd(BOTTOM_FACE_I2C_ADDRESS << 1, slave_cmd, 1);
	mBusWriteNoAdd(LEFT_FACE_I2C_ADDRESS << 1, slave_cmd, 1);
	mBusWriteNoAdd(RIGHT_FACE_I2C_ADDRESS << 1, slave_cmd, 1);
	mBusWriteNoAdd(TOP_FACE_I2C_ADDRESS << 1, slave_cmd, 1);
	
	//wait some seconds
	DelayMilliseconds(1000);

	//check if the EEPROM get the calibration parameter
	//mBusRead.....
}
