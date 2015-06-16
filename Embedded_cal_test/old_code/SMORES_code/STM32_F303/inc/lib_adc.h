/*
Author Chaitanya Bhargava (cbhar@seas.upenn.edu)
Settings for I2C usage on the STM32F303
mGeneral includes all the ST library files and the wait functions
*/
#ifndef LIB_ADC_H
#define	LIB_ADC_H

#include <stdint.h>

#ifdef	__cplusplus
extern "C" {
#endif

void ADC_A_config(void);
	
#ifdef	__cplusplus
}
#endif

#endif	/* LIB_ADC_H */
