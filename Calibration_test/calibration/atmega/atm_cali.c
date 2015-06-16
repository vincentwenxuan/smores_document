/* Name: main.c
 * Author: <insert your name here>
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

#include "wiper_encoder_cali.h"
#include "USART.h"

//#define BOTTOM_FACE

#define MAGNETS_ON            0xA2  // command to turn on the blue led
#define MAGNETS_OFF           0xA3  // command to turn on green led
#define FACE_PING             0xA5  // Ping request to tell if face is alive
#define SEND_STATES           0xA4
#define SEND_ADC_DATA         0xA6

/*   face i2c address macros:  */
#define BOTTOM_FACE_I2C_ADDRESS 0x0B
#define LEFT_FACE_I2C_ADDRESS   0x0A
#define RIGHT_FACE_I2C_ADDRESS  0x0C
#define TOP_FACE_I2C_ADDRESS    0x0D

unsigned char My_Address;

unsigned char messageBuf[TWI_BUFFER_SIZE];

int adc0_min = 1000;
int adc1_min = 1000;
int adc0_max = 0;
int adc1_max = 0;
int adc0_0 = 0;
int adc1_0 = 0;
int adc0_180 = 0;
int adc1_180 = 0;

void cali_on_mode(void) {
	int adc0 = 0;
	int adc1 = 0;
	
	//store min max	
	//should use the interrupt for the loop
	for(long i=0; i<100000; i++) {
		adc0 = return_adc0();
		adc1 = return_adc1();
		if (adc0_min > adc_0) adc0_min = adc_0;
		if (adc0_max < adc_0) adc0_max = adc_0;
		if (adc1_min > adc_1) adc1_min = adc_1;
		if (adc1_max < adc_1) adc1_max = adc_1;
	}

	float error_0, error_0_temp;
	float error_180, error_180_temp;
	error_0 = 1.0*adc0 / (adc0_max - adc0_min) + 1.0*adc1 / (adc1_max - adc1_min) - 1;
	if (error_0 < 0) error_0 = - error_0;
	error_180 = error_0;
	error_0_temp = error_0;
	error_180_temp = error_0;

	//store adc_0 adc_180
	for(long i=0; i<100000; i++) {
		adc0 = return_adc0();
		adc1 = return_adc1();
		error_0_temp = 1.0*adc0 / (adc0_max - adc0_min) + 1.0*adc1 / (adc1_max - adc1_min) - 1;
		if (error_0_temp < 0) error_0_temp = - error_0_temp;
		if ((error_0_temp < error_0) && (adc0 > adc1)) {
			error_0 = error_0_temp;
			adc0_0 = adc0;
			adc1_0 = adc1;
		}
		error_180_temp = 1.0*adc0 / (adc0_max - adc0_min) + 1.0*adc1 / (adc1_max - adc1_min) - 1;
		if (error_180_temp < 0) error_180_temp = - error_180_temp;
		if ((error_180_temp < error_180) && (adc0 < adc1)) {
			error_180 = error_180_temp;
			adc0_180 = adc0;
			adc1_180 = adc1;
		}
	}

}

void cali_off_mode(void) {
	//save to EEPROM
}

