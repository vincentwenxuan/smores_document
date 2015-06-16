// Define the hardware configuration and interrupt methods for the battery 
// monitor component.  Both the settable parameters and latest measured values 
// are available as extern variables declared in the header file.

#ifndef BATTERY_MONITOR_IT_H
#define BATTERY_MONITOR_IT_H

#include "common_peripherals.h"
#include "stdint.h"

#ifdef USING_BATTERY_MONITOR

#ifdef __cplusplus
extern "C" {
#endif

// Counts -> voltage conversion parameters
#define MAX_BATTERY_VOLTAGE 4.2 // 2.1 V input to ADC
#define MIN_BATTERY_VOLTAGE 3.4  //1.7 V input to ADC
#define VREF_P 2.78
#define VREF_N 0.0
#define MAX_ADC_VALUE 4095.0

//Macro functions to convert counts to voltage
#define SLOPE ((VREF_P - VREF_N)/MAX_ADC_VALUE)
#define TO_VOLTS(x) (SLOPE * x) 

//Voltage conditioning parameters:
#define BIAS_VOLTS 0.06725 //Volts

// Number of values buffered by DMA before processing
#define ADC_BUFFER_SIZE 3 

//Biquad filter parameters:
#define DT 0.0001 // seconds?


extern unsigned int adc_value;
extern uint16_t adc_value_buffer[ADC_BUFFER_SIZE] ;
extern float voltage_filtered;
extern float voltage_unfiltered; 
extern struct BiquadLpfDynamicC volts_filter; // gives the interrupt module acces to this struct


uint32_t InitBatteryMonitorHardware();
void BattItGetBatteryFilt(float* v, float* i, float* p, float* w);
// My addition
void BattStartADC(void);


// Measurements
int32_t BattItGetVoltsRaw();
float BattItGetVolts();
float BattItGetVoltsFilt();

int32_t BattItGetAmpsRaw();
float BattItGetAmps();
float BattItGetAmpsFilt();

float BattItGetWatts();
float BattItGetWattsFilt();

float BattItGetJoules();



// Parameters
float BattItGetVoltsGain();
void BattItSetVoltsGain(float gain);
void BattItCalculateVoltsGain(float truth);

float BattItGetAmpsBias();
void BattItSetAmpsBias(float bias);
float BattItGetAmpsGain();
void BattItSetAmpsGain(float gain);

void BattItResetJoules();

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // USING_BATTERY_MONITOR

#endif // BATTERY_MONITOR_IT_H