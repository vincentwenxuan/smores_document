#include "common_peripherals.h"
#include "battery_monitor_it.h"
#include "system_clock.h"
#include "common_message_types.h"
// #include "common_peripherals.h"
#include "biquad_lpf_dynamic_c.h"

#include STM32FXXX_PWR_H
#include STM32FXXX_MISC_H
#include STM32FXXX_RCC_H
#include STM32FXXX_ADC_H
#include STM32FXXX_DMA_H


#include "stdint.h"

// ADC Config:
#define ADC_CHANNEL ADC_Channel_5
#define GROUP_RANK 1
#define SAMPLE_TIME ADC_SampleTime_181Cycles5

// #define ADC4_DR_Address ADC4_BASE
#define ADC4_DR_Address (uint32_t)&ADC4->DR






#ifdef USING_BATTERY_MONITOR


const float kFilterF0 = 250.0f; // corner frequency for filters, Hz

static volatile struct Time volts_sample_time;
static volatile struct Time amps_sample_time;


// measured quantities
uint16_t adc_value_buffer[ADC_BUFFER_SIZE] ; //dma buffer where adc values are stored
static volatile int32_t volts_raw   = 3.7; 
static volatile float   volts       = 3.7;
static volatile float   volts_filt  = 3.7;

static volatile int32_t amps_raw    = 0;
static volatile float   amps        = 0;
static volatile float   amps_filt   = 0;

static volatile float   watts       = 0;
static volatile float   watts_filt  = 0;

static volatile float   joules      = 0;

// calibration parameters
static float volts_gain  = 1;
static float amps_gain   = 1;
static float amps_bias   = -32768.0f;

// low pass filter machinery 
struct BiquadLpfDynamicC volts_filter; // storage class changed to extern
static volatile struct BiquadLpfDynamicC amps_filter;
static volatile struct BiquadLpfDynamicC watts_filter;

void BattItGetBatteryFilt(float* v, float* i, float* p, float* e) {
  *v = BiquadLpfDynamicCGet(&volts_filter);
  *i = 0;
  *p = 0;
  *e = 0;
}

// Battery Voltage
int32_t BattItGetVoltsRaw() {
  volts_raw = adc_value;
  return volts_raw;
}
float BattItGetVolts() {
  
  volts = voltage_unfiltered;
  // volts = 2.0 * (TO_VOLTS(adc_value) + BIAS_VOLTS) ;
  // volts_filt = volts; //  done so that efficiency msg shows the needed batt voltage
  return volts;
}
float BattItGetVoltsFilt() {
  volts_filt = BiquadLpfDynamicCGet(&volts_filter);
  return volts_filt;
}
float BattItGetVoltsGain() {
  return volts_gain;
}
void BattItSetVoltsGain(float gain) {
  volts_gain = gain;
}
void BattItCalculateVoltsGain(float truth) {
  volts_gain = truth / volts_raw;
}

// Start ADC conversion:
void BattStartADC()
{
  ADC_StartConversion(ADC4);

}



// Battery Current
int32_t BattItGetAmpsRaw() {
  return amps_raw;
}
float BattItGetAmps() {
  return amps;
}
float BattItGetAmpsFilt() {
  return amps_filt;
}
float BattItGetAmpsBias() {
  return amps_bias;
}
void BattItSetAmpsBias(float bias) {
  amps_bias = bias;
}
float BattItGetAmpsGain() {
  return amps_gain;
}
void BattItSetAmpsGain(float gain) {
  amps_gain = gain;
}

// Battery Power
float BattItGetWatts() {
  return watts;
}
float BattItGetWattsFilt() {
  return watts_filt;
}

// Battery Energy
float BattItGetJoules() {
  return joules;
}
void BattItResetJoules() {
  joules = 0;
}





uint32_t InitVoltageHardware() {

  // Initialise DMA
    DMA_DeInit(DMA2_Channel2);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2,ENABLE);

    DMA_InitTypeDef DMA_ADC;
    DMA_ADC.DMA_PeripheralBaseAddr = ADC4_DR_Address;
    DMA_ADC.DMA_MemoryBaseAddr = (uint32_t)adc_value_buffer    ; // Change here,  this should be the variable yo are writing ot
    DMA_ADC.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_ADC.DMA_BufferSize = ADC_BUFFER_SIZE;
    DMA_ADC.DMA_PeripheralInc = DMA_PeripheralInc_Disable ; 
    DMA_ADC.DMA_MemoryInc = DMA_MemoryInc_Enable; //DMA_MemoryInc_Disable
    DMA_ADC.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_ADC.DMA_MemoryDataSize =  DMA_MemoryDataSize_HalfWord;
    DMA_ADC.DMA_Mode = DMA_Mode_Circular;
    DMA_ADC.DMA_Priority =  DMA_Priority_High;
    DMA_ADC.DMA_M2M = DMA_M2M_Disable ;

    DMA_Init(DMA2_Channel2,&DMA_ADC);
    DMA_SetCurrDataCounter(DMA2_Channel2,ADC_BUFFER_SIZE);
    DMA_ClearFlag(DMA2_FLAG_TC2);
    DMA_ClearFlag(DMA2_FLAG_HT2);
    DMA_ClearFlag(DMA2_FLAG_TE2);
    DMA_ClearFlag(DMA2_FLAG_GL2);
    DMA_ITConfig(DMA2_Channel2,DMA_IT_TC,ENABLE); //should be enabled
    DMA_Cmd(DMA2_Channel2,ENABLE);
    

    // Initialise Interrupt
    NVIC_InitTypeDef   NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel =  DMA2_Channel2_IRQn   ;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    NVIC_EnableIRQ(DMA2_Channel2_IRQn);   

    // Initialise ADC

    ADC_DeInit(ADC4);
    ADC_InitTypeDef batt_volt_init;
    
    // Configure ADC : 
    batt_volt_init.ADC_ContinuousConvMode = ENABLE;
    batt_volt_init.ADC_Resolution = ADC_Resolution_12b;
    batt_volt_init.ADC_DataAlign = ADC_DataAlign_Right;
    // batt_volt_init.ADC_ExternalTrigConvEvent = ADC_ExternalTrigConvEvent_0;
    batt_volt_init.ADC_ExternalTrigEventEdge = ADC_ExternalTrigEventEdge_None;
    batt_volt_init.ADC_OverrunMode = DISABLE ;
    batt_volt_init.ADC_AutoInjMode = DISABLE;
    batt_volt_init.ADC_NbrOfRegChannel = 1;

    ADC_CommonInitTypeDef batt_ADC_common;
    batt_ADC_common.ADC_Mode = ADC_Mode_Independent; 
    batt_ADC_common.ADC_Clock = ADC_Clock_SynClkModeDiv2;
    batt_ADC_common.ADC_DMAAccessMode = ADC_DMAAccessMode_1;
    batt_ADC_common.ADC_DMAMode = ADC_DMAMode_Circular; //ADC_DMAMode_Circular
    // batt_ADC_common.ADC_TwoSamplingDelay  =  0x4; 

    // Configure GPIO:
    GPIO_InitTypeDef voltage_pin_init;
    voltage_pin_init.GPIO_Pin = GPIO_Pin_15;
    voltage_pin_init.GPIO_Mode =  GPIO_Mode_AN;// analog input
    voltage_pin_init.GPIO_PuPd = GPIO_PuPd_NOPULL;


    ADC_VoltageRegulatorCmd(ADC4,ENABLE);
    DelayMicroseconds(10);

    RCC_ADCCLKConfig(RCC_ADC34PLLCLK_Div1); // function defined in stm32f30x_rcc.c
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_ADC34,ENABLE);//function defined in stm32f30x_rcc.c
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE); // voltage input is PB15
    

    GPIO_Init(GPIOB,&voltage_pin_init);
    ADC_CommonInit(ADC4,&batt_ADC_common);
    ADC_Init(ADC4,&batt_volt_init);
    ADC_ITConfig(ADC4,ADC_IT_EOC,DISABLE);


    ADC_RegularChannelSequencerLengthConfig(ADC4,1);
    ADC_RegularChannelConfig(ADC4,ADC_CHANNEL, GROUP_RANK, SAMPLE_TIME);
    // ADC_DMAConfig(ADC4,ADC_DMAMode_Circular);
        
    ADC_DMACmd(ADC4,ENABLE); 

    ADC_SelectCalibrationMode(ADC4,ADC_CalibrationMode_Single);
    ADC_StartCalibration(ADC4);
  
    while(ADC_GetCalibrationStatus(ADC4) == SET ); // wait for calibration to get over
   
    ADC_Cmd(ADC4,ENABLE);

    while( !ADC_GetFlagStatus( ADC4, ADC_FLAG_RDY ) );//NEWLY ADDED
    ADC_StartConversion(ADC4);

  return 0;
}


uint32_t InitCurrentHardware() {
  return 0;
}


uint32_t InitBatteryMonitorHardware() {
  BiquadLpfDynamicCInit(&volts_filter,  kFilterF0);
  BiquadLpfDynamicCInit(&amps_filter,   kFilterF0);
  BiquadLpfDynamicCInit(&watts_filter,  kFilterF0);

  InitVoltageHardware();
  InitCurrentHardware();
  return 0;
}




#endif // USING_BATTERY_MONITOR