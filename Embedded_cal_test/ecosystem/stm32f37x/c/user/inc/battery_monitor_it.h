// Define the hardware configuration and interrupt methods for the battery 
// monitor component.  Both the settable parameters and latest measured values 
// are available as extern variables declared in the header file.  A timer event 
// triggers the SDADC1 sample of the voltage, which has the SDADC2 sample of 
// the current synchronized to it.  The values are read simultaneously in an 
// interrupt.  The SDADC can not be triggered in this manner above 12.5 kHz.

#ifndef BATTERY_MONITOR_IT_H
#define BATTERY_MONITOR_IT_H

#include "common_peripherals.h"

#ifdef USING_BATTERY_MONITOR

#ifdef __cplusplus
extern "C" {
#endif

uint32_t InitBatteryMonitorHardware();
void BattItGetBatteryFilt(float* v, float* i, float* p, float* w);

// Measurements
int32_t BattItGetVoltsRaw();
float   BattItGetVolts();
float   BattItGetVoltsFilt();

int32_t BattItGetAmpsRaw();
float   BattItGetAmps();
float   BattItGetAmpsFilt();

float BattItGetWatts();
float BattItGetWattsFilt();

float BattItGetJoules();


// Parameters
float BattItGetVoltsGain();
void  BattItSetVoltsGain(float gain);
void  BattItCalculateVoltsGain(float truth);

float BattItGetAmpsBias();
void  BattItSetAmpsBias(float bias);
float BattItGetAmpsGain();
void  BattItSetAmpsGain(float gain);

void BattItResetJoules();

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // USING_BATTERY_MONITOR

#endif // BATTERY_MONITOR_IT_H