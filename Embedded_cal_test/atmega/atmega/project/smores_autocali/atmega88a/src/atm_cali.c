/* Name: main.c
 * Author: <Xiaojun Sun>
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

//#define BOTTOM_FACE
#include "atm_cali.h"


uint16_t adc0_min = 1000;
uint16_t adc1_min = 1000;
uint16_t adc0_max = 0;
uint16_t adc1_max = 0;
uint16_t adc0_0 = 0;
uint16_t adc1_0 = 0;
uint16_t adc0_180 = 0;
uint16_t adc1_180 = 0;

void cali_mode() {
	uint16_t adc0_raw = return_adc0();
	uint16_t adc1_raw = return_adc1();
	uint16_t adc0 = adc0_raw;
	uint16_t adc1 = adc1_raw;
	uint16_t adc0_pre = 0;
	uint16_t adc1_pre = 0;
	unsigned char messageBuf[TWI_BUFFER_SIZE];

	//store min max	
	//should use the interrupt for the loop
	m_blue(ON);
	messageBuf[0] = 0xA0;
	//find the max and the min value of adc0 and adc1


	while (messageBuf[0] != CALI_SWITCH) {
		if ( TWI_statusReg.RxDataInBuf )
            TWI_Get_Data_From_Transceiver(messageBuf, 1);

		//raw data
		adc0_raw = return_adc0();
		adc1_raw = return_adc1();
		
		adc0_pre = adc0;
		adc1_pre = adc1;
		
		//boundary condition
		if (adc0_raw <=15) adc0_pre = 0;
		else if (adc0 <= 15) adc0_pre = adc0_raw;
		if (adc1_raw <=15) adc1_pre = 0;
		else if (adc1 <= 15) adc1_pre = adc1_raw;
		
		//low pass filter
		adc0 = (2*adc0_raw + 8*adc0_pre) /10;
		adc1 = (2*adc1_raw + 8*adc1_pre) /10;

		if (adc0_min > adc0 && adc0 >15) adc0_min = adc0;
		if (adc0_max < adc0) adc0_max = adc0;
		if (adc1_min > adc1 && adc1 >15) adc1_min = adc1;
		if (adc1_max < adc1) adc1_max = adc1;
	}

	m_blue(OFF);

	float error_0, error_0_temp;
	float error_180, error_180_temp;
	error_0 = 1.0*adc0 / (adc0_max - adc0_min) + 1.0*adc1 / (adc1_max - adc1_min) - 1;
	if (error_0 < 0) error_0 = - error_0;
	error_180 = error_0;
	error_0_temp = error_0;
	error_180_temp = error_0;

	//store adc_0 adc_180
	m_green(ON);

	adc0_raw = return_adc0();
	adc1_raw = return_adc1();
	adc0 = adc0_raw;
	adc1 = adc1_raw;

	//find the adc values at 0 degree and 180 degree 
	while (messageBuf[0] != CALI_OFF) {
		if ( TWI_statusReg.RxDataInBuf )
            TWI_Get_Data_From_Transceiver(messageBuf, 1);
		
		//raw data
		adc0_raw = return_adc0();
		adc1_raw = return_adc1();

		adc0_pre = adc0;
		adc1_pre = adc1;
		
		//boundary condition
		if (adc0_raw <=15) adc0_pre = 0;
		else if (adc0 <= 15) adc0_pre = adc0_raw;
		if (adc1_raw <=15) adc1_pre = 0;
		else if (adc1 <= 15) adc1_pre = adc1_raw;

		//low pass filter
		adc0 = (2*adc0_raw + 8*adc0_pre) /10;
		adc1 = (2*adc1_raw + 8*adc1_pre) /10;

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
	eeprom_write_word((uint16_t*) 4, adc0_0);
	m_wait(100);
	eeprom_write_word((uint16_t*) 8, adc1_0);
	m_wait(100);
	eeprom_write_word((uint16_t*) 12, adc0_180);
	m_wait(100);
	eeprom_write_word((uint16_t*) 16, adc1_180);
	m_wait(100);

	m_green(OFF);
}

void cali_off_mode() {
	float resulo_0 = 1.0*(adc0_180 - adc0_0)/180;
	float resulo_1 = 1.0*(adc1_180 - adc0_0)/180;
	eeprom_write_float((float*) 24, resulo_0);
	m_wait(100);
	eeprom_write_float((float*) 32, resulo_1);
	m_wait(100);
}

// For tilt calibration, only adc0 is recorded
void cali_on_tilt(int address) {
	int j;
	m_blue(ON);
	for(j=0;j<4;j++) {
    	m_blue(TOGGLE);
    	m_wait(100);
    }
	adc0_0 = return_adc0();
	eeprom_write_word((uint16_t*) address, adc0_0);
	m_wait(100);
	if (address == 12) {
		float resulo_0 = 1.0*(return_adc0_180() - return_adc0_0())/180;
		eeprom_write_float((float*) 24, resulo_0);
		m_wait(100);
	}
	m_blue(OFF);
}

int return_adc0_0() {
	return eeprom_read_word((uint16_t*) 4); //adc0_0; uncomment if you wanna return the value without eeprom
}

int return_adc1_0() {
	return eeprom_read_word((uint16_t*) 8); //adc1_0; uncomment if you wanna return the value without eeprom
}

int return_adc0_180() {
	return eeprom_read_word((uint16_t*) 12); //adc0_180; uncomment if you wanna return the value without eeprom
}

int return_adc1_180() {
	return eeprom_read_word((uint16_t*) 16); //adc1_180; uncomment if you wanna return the value without eeprom
}

float return_resolution_0() {
	return eeprom_read_float((float*) 24);
}

float return_resolution_1() {
	return eeprom_read_float((float*) 32);
}

