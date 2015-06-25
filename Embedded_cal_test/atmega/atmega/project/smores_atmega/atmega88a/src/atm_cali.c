/* Name: main.c
 * Author: <insert your name here>
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

//#define BOTTOM_FACE
#include "atm_cali.h"

int adc0_min = 1000;
int adc1_min = 1000;
int adc0_max = 0;
int adc1_max = 0;
int adc0_0 = 0;
int adc1_0 = 0;
int adc0_180 = 0;
int adc1_180 = 0;
int adc0_pre = 0;
int adc1_pre = 0;

void cali_on_mode() {
	int adc0 = 0;
	int adc1 = 0;
	int i,j;
	//store min max	
	//should use the interrupt for the loop
	m_blue(ON);
	for(i=0; i<40; i++) {
		for(j=0; j<10000; j++) {
			// raw data
			// adc0 = return_adc0();
			// adc1 = return_adc1();

			//low pass filter
			adc0_pre = adc0;
			adc1_pre = adc1;
			adc0 = (int) (0.2*return_adc0() + 0.8*adc0_pre);
			adc1 = (int) (0.2*return_adc1() + 0.8*adc1_pre);
			
			if (adc0_min > adc0 && adc0 >15) adc0_min = adc0;
			if (adc0_max < adc0) adc0_max = adc0;
			if (adc1_min > adc1 && adc1 >15) adc1_min = adc1;
			if (adc1_max < adc1) adc1_max = adc1;
		}
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
	for(i=0; i<1; i++) {
		for(j=0; j<18000; j++) {
			// raw data
			// adc0 = return_adc0();
			// adc1 = return_adc1();

			// low pass filter
			adc0_pre = adc0;
			adc1_pre = adc1;
			adc0 = (int) (0.2*return_adc0() + 0.8*adc0_pre);
			adc1 = (int) (0.2*return_adc1() + 0.8*adc1_pre);

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
	m_green(OFF);
}

void cali_off_mode() {
	float resulo_0 = 1.0*(adc0_180 - adc0_0)/180;
	float resulo_1 = 1.0*(adc1_180 - adc0_0)/180;
}

int return_adc0_0() {
	return adc0_0;
}

int return_adc1_0() {
	return adc1_0;
}

int return_adc0_180() {
	return adc0_180;
}

int return_adc1_180() {
	return adc1_180;
}

