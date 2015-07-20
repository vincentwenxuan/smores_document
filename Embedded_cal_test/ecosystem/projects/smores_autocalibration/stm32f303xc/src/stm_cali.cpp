#include "stm_cali.h"

char slave_cmd;

// when the calibration cmd is received, the program falls into a time loop.
void cali_cmd(char address, Motor motor1, Motor motor2) {
	// set the Atmega to calibration mode
	mBlueON;
	slave_cmd = CALI_ON;
	mBusWriteNoAdd(address << 1, slave_cmd, 1);

	// control the motor
	if (address == LEFT_FACE_I2C_ADDRESS || address == RIGHT_FACE_I2C_ADDRESS) {
		set_motor_direction(motor1, Bit_SET);
		set_motor_speed(motor1, 60);
	}
	else if (address == TOP_FACE_I2C_ADDRESS) {
		set_motor_direction(motor1, Bit_SET);
		set_motor_speed(motor1, 60);
		set_motor_direction(motor2, Bit_SET);
		set_motor_speed(motor2, 60);
	}

	//wait some seconds
	DelayMilliseconds(25000);
	mYellowON;
	slave_cmd = CALI_SWITCH;
	mBusWriteNoAdd(address << 1, slave_cmd, 1);
	DelayMilliseconds(25000);
	mYellowOFF;

	//stop
	set_motor_speed(motor1, 0);
	set_motor_speed(motor2, 0);

	//wait some seconds
	DelayMilliseconds(1000);

	// tell the Atmega to finish the calibration mode
	slave_cmd = CALI_OFF;
	mBusWriteNoAdd(address << 1, slave_cmd, 1);
	
	//wait some seconds
	DelayMilliseconds(1000);
	mBlueOFF;

	//check if the EEPROM get the calibration parameter
	//mBusRead.....
}

// tilt calibration at 0 degrees. When the command received,
// the adc value will be recorded to EEPROM immediately.
void cali_cmd_tilt(char cmd) {
	slave_cmd = cmd;
	mBusWriteNoAdd(BOTTOM_FACE_I2C_ADDRESS << 1, slave_cmd, 1);
}



