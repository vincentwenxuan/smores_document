/**
 * Cedric Destin
 **/ 
/**
 * Headers: 
 * 	stm32f37x
 * 	mGeneral
 * 	common_peripherals
 * 	stdio
 **/
#include "stm32f37x.h"
#include "mGeneral.h"
#include "project_peripherals.h"
#include "stdio.h"
#include "system_clock.h"
/** Initialize variables here
 * 	SRF10: Address of the SRF10
 * 	MIMU: Address of IMU
 **/ 
#define SRF10 0b11100000
#define MIMU 0b11010010
/** Function Declarations
 * 
 **/ 
void IMU_Config(uint8_t I2Cx, uint8_t accel_scale, uint8_t gyro_scale);
uint8_t mBusIMU_accel(uint16_t* raw_data, uint8_t I2Cx);
uint8_t mBusIMU_gyro(uint16_t* raw_data, uint8_t I2Cx);