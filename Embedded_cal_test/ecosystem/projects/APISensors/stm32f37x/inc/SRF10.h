/**
 * Cedric Destin
**/
#include "mGeneral.h"
#include "stm32f37x.h"
#include "mGeneral.h"
#include "project_peripherals.h"
#include "stdio.h"
#include "mBus.h"
#define SRF10Adrs 0b11100000
void SRF10Config(uint8_t I2Cx);
void SRF10Command(uint8_t I2Cx, uint8_t Unit, uint8_t Range);
uint16_t SRF10(uint8_t I2Cx);