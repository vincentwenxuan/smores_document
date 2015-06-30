#ifndef ATM_CALI_
#define ATM_CALI_

#include "wiper_encoder.h"
#include "common_macro.h"
#include "TWI_slave.h"
#include <avr/eeprom.h>

void cali_mode();
void cali_off_mode();
int return_adc0_0();
int return_adc1_0();
int return_adc0_180();
int return_adc1_180();

#endif //ATM_CALI_