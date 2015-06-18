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

void cali_on_mode(void) {
	int adc0 = 0;
	int adc1 = 0;
	int i;
	//store min max	
	//should use the interrupt for the loop
	for(i=0; i<10000; i++) {
		adc0 = return_adc0();
		adc1 = return_adc1();
		if (adc0_min > adc0 && adc0 >15) adc0_min = adc0;
		if (adc0_max < adc0) adc0_max = adc0;
		if (adc1_min > adc1 && adc1 >15) adc1_min = adc1;
		if (adc1_max < adc1) adc1_max = adc1;
	}

	float error_0, error_0_temp;
	float error_180, error_180_temp;
	error_0 = 1.0*adc0 / (adc0_max - adc0_min) + 1.0*adc1 / (adc1_max - adc1_min) - 1;
	if (error_0 < 0) error_0 = - error_0;
	error_180 = error_0;
	error_0_temp = error_0;
	error_180_temp = error_0;

	//store adc_0 adc_180
	for(i=0; i<10000; i++) {
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
	float resulo_0 = 1.0*(adc0_180 - adc0_0)/180;
	float resulo_1 = 1.0*(adc1_180 - adc0_0)/180;
	USART_Transmit_int(adc0_0);
	USART_Transmit_char(' ');
	USART_Transmit_int(adc0_180);
	USART_Transmit_char(' ');
	USART_Transmit_int(adc1_0);
	USART_Transmit_char(' ');
	USART_Transmit_int(adc1_180);
    USART_Transmit_string("\n\r");
}

