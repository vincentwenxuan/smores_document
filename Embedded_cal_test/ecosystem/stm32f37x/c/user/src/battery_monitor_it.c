#include "common_peripherals.h"
#include "battery_monitor_it.h"
#include "system_clock.h"
#include "common_message_types.h"
#include "common_peripherals.h"
#include "biquad_lpf_c.h"

#include STM32FXXX_SDADC_H
#include STM32FXXX_PWR_H
#include STM32FXXX_MISC_H
#include STM32FXXX_RCC_H

#include <stdint.h>
#include <string.h>

#ifdef USING_BATTERY_MONITOR

////////////////////////////////////////////////////////////////////////////////
// User Configuration Parameters

// Supported SDADC1 External Injected Triggers
// TIM3,  CC1
// TIM4,  CC1
// TIM13, CC1
// TIM14, CC1
// TIM15, CC2
// TIM19, CC2
#define SDADC_TRIGGER_TIM_NUM     14
#define SDADC_TRIGGER_TIM_OC_NUM  1

////////////////////////////////////////////////////////////////////////////////
// Private Convenience Definitions

#define SDADC_TRIGGER_TIM           MAKENAME(TIM,SDADC_TRIGGER_TIM_NUM)
#define SDADC_TRIGGER_PERIOD_TICKS  6000
#define SDADC_TRIGGER_FREQ          (72000000.0f/SDADC_TRIGGER_PERIOD_TICKS)
#define SDADC_TRIGGER_PERIOD        (1.0f/SDADC_TRIGGER_FREQ)

////////////////////////////////////////////////////////////////////////////////
// Other Configuration Parameters

// Voltage Sense on SDADC1
#define BATTV_GPIO_PORT     GPIOE
#define BATTV_GPIO_PIN      GPIO_Pin_8
#define BATTV_GPIO_CLK      RCC_AHBPeriph_GPIOE
#define BATTV_SDADC_CHANNEL SDADC_Channel_8

// Current Sense on SDADC2
#define BATTI_GPIO_PORT     GPIOB
#define BATTI_GPIO_PIN      GPIO_Pin_2
#define BATTI_GPIO_CLK      RCC_AHBPeriph_GPIOB
#define BATTI_SDADC_CHANNEL SDADC_Channel_6

#define SDADC_INIT_TIMEOUT   30 /* ~ about two SDADC clock cycles after INIT is set */
#define SDADC_CAL_TIMEOUT    4*30720 /*  ~5.12 ms at 6 MHz  in a single calibration sequence */
#define SDADC_VREF           3.3f  /* SDADC external reference is set to 3.3V */

////////////////////////////////////////////////////////////////////////////////

const float kCornerFreq = 250.0f;   // corner frequency for filters, Hz

// measured quantities
static volatile int32_t volts_raw   = 0;
static volatile float   volts       = 0;
static volatile float   volts_filt  = 0;

static volatile int32_t amps_raw    = 0;
static volatile float   amps        = 0;
static volatile float   amps_filt   = 0;

static volatile float   watts       = 0;
static volatile float   watts_filt  = 0;

static volatile float   joules      = 0;

// calibration parameters
static float volts_gain  = (108.7/20.0) * (SDADC_VREF / 65535);
static float amps_gain   = (0.05f) * (SDADC_VREF / 65535);
static float amps_bias   = -32768.0f;

// low pass filter machinery
static volatile struct BiquadLpfC volts_filter;
static volatile struct BiquadLpfC amps_filter;
static volatile struct BiquadLpfC watts_filter;

void BattItGetBatteryFilt(float* v, float* i, float* p, float* e) {
  *v = volts_filt;
  *i = amps_filt;
  *p = watts_filt;
  *e = joules;
}

////////////////////////////////////////////////////////////////////////////////
// Battery Voltage
int32_t BattItGetVoltsRaw() {
  return volts_raw;
}
float BattItGetVolts() {
  return volts;
}
float BattItGetVoltsFilt() {
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

////////////////////////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////////////////////////////
// Battery Power
float BattItGetWatts() {
  return watts;
}
float BattItGetWattsFilt() {
  return watts_filt;
}

////////////////////////////////////////////////////////////////////////////////
// Battery Energy
float BattItGetJoules() {
  return joules;
}
void BattItResetJoules() {
  joules = 0;
}

////////////////////////////////////////////////////////////////////////////////


// Timer generates synchronized sampling frequency
void ConfigSyncSampling() {
  // Enable timer clock
  #if defined(STM32F303xC)
    #if SDADC_TRIGGER_TIM_NUM == 2 || SDADC_TRIGGER_TIM_NUM == 3 || SDADC_TRIGGER_TIM_NUM == 4 || SDADC_TRIGGER_TIM_NUM == 6 || SDADC_TRIGGER_TIM_NUM == 7
      RCC_APB1PeriphClockCmd(MAKENAME(RCC_APB1Periph_TIM,SDADC_TRIGGER_TIM_NUM), ENABLE);
    #elif SDADC_TRIGGER_TIM_NUM == 1 || SDADC_TRIGGER_TIM_NUM == 8 || SDADC_TRIGGER_TIM_NUM == 15 || SDADC_TRIGGER_TIM_NUM == 16 || SDADC_TRIGGER_TIM_NUM == 17
      RCC_APB1PeriphClockCmd(MAKENAME(RCC_APB2Periph_TIM,SDADC_TRIGGER_TIM_NUM), ENABLE);
    #endif

  #elif defined(STM32F37X)
    #if SDADC_TRIGGER_TIM_NUM == 15 || SDADC_TRIGGER_TIM_NUM == 16 || SDADC_TRIGGER_TIM_NUM == 17 || SDADC_TRIGGER_TIM_NUM == 19
      RCC_APB2PeriphClockCmd(MAKENAME(RCC_APB2Periph_TIM,SDADC_TRIGGER_TIM_NUM), ENABLE);
    #else
      RCC_APB1PeriphClockCmd(MAKENAME(RCC_APB1Periph_TIM,SDADC_TRIGGER_TIM_NUM), ENABLE);
    #endif
  #else
    #error("Unrecognized target. The Makefile should define one of the following: {STM32F37X, STM32F303xC}")
  #endif

  // TIM Configuration
  TIM_DeInit(SDADC_TRIGGER_TIM);

  // Time base configuration
  TIM_TimeBaseInitTypeDef  tb;
  TIM_TimeBaseStructInit(&tb);
  tb.TIM_Period         = SDADC_TRIGGER_PERIOD_TICKS - 1;
  tb.TIM_Prescaler      = 0;
  tb.TIM_ClockDivision  = 0;
  tb.TIM_CounterMode    = TIM_CounterMode_Up;
  TIM_TimeBaseInit(SDADC_TRIGGER_TIM, &tb);
  
  // TIM output compare
  TIM_OCInitTypeDef  oc;
  TIM_OCStructInit(&oc);
  oc.TIM_OCMode       = TIM_OCMode_PWM1;
  oc.TIM_OutputState  = TIM_OutputState_Enable;
  oc.TIM_Pulse        = SDADC_TRIGGER_PERIOD_TICKS / 2;
  oc.TIM_OCPolarity   = TIM_OCPolarity_High;
  // eg TIM_OC2Init(TIM15, &TIM_OCInitStructure)
  MAKENAME(MAKENAME(TIM_OC,SDADC_TRIGGER_TIM_OC_NUM),Init)(SDADC_TRIGGER_TIM, &oc);

  // Interrupt setup, not required for normal operation
  // This interrupt could be used to verify the sample frequency with a scope
  // NVIC_InitTypeDef nv;
  // nv.NVIC_IRQChannel                    = TIM14_IRQn;
  // nv.NVIC_IRQChannelPreemptionPriority  = 0;
  // nv.NVIC_IRQChannelCmd                 = ENABLE;
  // NVIC_Init(&nv);
  // TIM_ITConfig(SDADC_TRIGGER_TIM, MAKENAME(TIM_IT_CC,SDADC_TRIGGER_TIM_OC_NUM), ENABLE);

  // start timer
  TIM_Cmd(SDADC_TRIGGER_TIM, ENABLE);
}


// Voltage Hardware on SDADC1
uint32_t InitVoltageHardware() {

  //////////////////////////////////////////////////////////////////////////////
  // Configure GPIO

  // enable gpio clock
  RCC_AHBPeriphClockCmd(BATTV_GPIO_CLK, ENABLE);

  // configure pin for analog input
  GPIO_InitTypeDef gp;
  gp.GPIO_Mode  = GPIO_Mode_AN;
  gp.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  gp.GPIO_Pin   = BATTV_GPIO_PIN;
  GPIO_Init(BATTV_GPIO_PORT, &gp);

  //////////////////////////////////////////////////////////////////////////////
  // Configure the SDADC Hardware

  // enable SDADC APB interface clock to get write access to SDADC registers
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SDADC1, ENABLE);

  // PWR APB1 interface clock enable
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

  // enable SDADC analog interface
  PWR_SDADCAnalogCmd(PWR_SDADCAnalog_1, ENABLE);

  // Set the SDADC divider: The SDADC should run at <= 6MHz
  RCC_SDADCCLKConfig(RCC_SDADCCLK_SYSCLK_Div12);

  // Select external reference (one common setting for all SDADC peripherals)
  SDADC_VREFSelect(SDADC_VREF_Ext);

  DelayMilliseconds(5);

  // Enable the SDADC peripheral
  SDADC_Cmd(SDADC1, ENABLE);

  // Enter initialization mode
  SDADC_InitModeCmd(SDADC1, ENABLE);
  uint32_t SDADCTimeout = SDADC_INIT_TIMEOUT;
  while((SDADC_GetFlagStatus(SDADC1, SDADC_FLAG_INITRDY) == RESET) && (--SDADCTimeout != 0));
  if(SDADCTimeout == 0) {
    mRedON;
    return 1; // INITRDY flag can not set
  }

  // Configure Analog Inputs for Conf_0
  SDADC_AINStructTypeDef ai;
  ai.SDADC_InputMode  = SDADC_InputMode_SEZeroReference;
  ai.SDADC_Gain       = SDADC_Gain_1;
  ai.SDADC_CommonMode = SDADC_CommonMode_VSSA;
  ai.SDADC_Offset     = 0;
  SDADC_AINInit(SDADC1, SDADC_Conf_0, &ai);

  // Associate configuration Conf_0 to channel
  SDADC_ChannelConfig(SDADC1, BATTV_SDADC_CHANNEL, SDADC_Conf_0);

  // Select channel for injected conversion
  SDADC_InjectedChannelSelect(SDADC1, BATTV_SDADC_CHANNEL);

  // Select trigger
  #define SDADC_ExternalTrigInjecConv_TX_CCY MAKENAME(MAKENAME(MAKENAME(SDADC_ExternalTrigInjecConv_T,SDADC_TRIGGER_TIM_NUM),_CC),SDADC_TRIGGER_TIM_OC_NUM)
  SDADC_ExternalTrigInjectedConvConfig(SDADC1, SDADC_ExternalTrigInjecConv_TX_CCY);
  SDADC_ExternalTrigInjectedConvEdgeConfig( SDADC1, SDADC_ExternalTrigInjecConvEdge_Rising);

  // Exit initialization mode
  SDADC_InitModeCmd(SDADC1, DISABLE);

  //////////////////////////////////////////////////////////////////////////////
  // Configure Interrupts

  // Enable end of injected conversion interrupt
  SDADC_ITConfig(SDADC1, SDADC_IT_JEOC, ENABLE);

  // Initialize interrupt
  NVIC_InitTypeDef nv;
  nv.NVIC_IRQChannel                    = SDADC1_IRQn;
  nv.NVIC_IRQChannelPreemptionPriority  = 0;
  nv.NVIC_IRQChannelSubPriority         = 0;
  nv.NVIC_IRQChannelCmd                 = ENABLE;
  NVIC_Init(&nv);

  // configure calibration to be performed on conf0
  SDADC_CalibrationSequenceConfig(SDADC1, SDADC_CalibrationSequence_1);
  SDADC_StartCalibration(SDADC1);
  SDADCTimeout = SDADC_CAL_TIMEOUT; // Set calibration timeout: 5.12 ms at 6 MHz in a single calibration sequence
  while((SDADC_GetFlagStatus(SDADC1, SDADC_FLAG_EOCAL) == RESET) && (--SDADCTimeout != 0));
  if(SDADCTimeout == 0) {
    mRedON;
    return 2; // EOCAL flag can not set
  }

  //SDADC_SoftwareStartInjectedConv(SDADC1); // Start a software start conversion

  return 0;
}


// Current Hardware on SDADC2
uint32_t InitCurrentHardware() {

  uint32_t SDADCTimeout = 0;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SDADC2, ENABLE); // BATTI_SDADC APB2 interface clock enable
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE); // PWR APB1 interface clock enable
  PWR_SDADCAnalogCmd(PWR_SDADCAnalog_2, ENABLE); // Enable BATTI_SDADC analog interface

  // Set the SDADC divider: The SDADC should run @6MHz
  RCC_SDADCCLKConfig(RCC_SDADCCLK_SYSCLK_Div12); // If Sysclk is 72MHz, SDADC divider should be 12

  RCC_AHBPeriphClockCmd(BATTI_GPIO_CLK, ENABLE); // BATTV_GPIO_CLK Peripheral clock enable

  // BATTI_SDADC channel
  GPIO_InitTypeDef gp;
  gp.GPIO_Mode  = GPIO_Mode_AN;
  gp.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  gp.GPIO_Pin   = BATTI_GPIO_PIN;
  GPIO_Init(BATTI_GPIO_PORT, &gp);

  SDADC_VREFSelect(SDADC_VREF_Ext); // Select external reference

  DelaySeconds(0.005);
  
  SDADC_Cmd(SDADC2, ENABLE); // Enable SDADC

  // Enter initialization mode, wait for INITRDY flag to be set
  SDADC_InitModeCmd(SDADC2, ENABLE);
  SDADCTimeout = SDADC_INIT_TIMEOUT;
  while((SDADC_GetFlagStatus(SDADC2, SDADC_FLAG_INITRDY) == RESET) && (--SDADCTimeout != 0));
  if(SDADCTimeout == 0) {
    return 1; // INITRDY flag can not set
  }

  // Analog Input configuration conf0: use single ended zero reference
  SDADC_AINStructTypeDef ai;
  ai.SDADC_InputMode  = SDADC_InputMode_SEZeroReference;
  ai.SDADC_Gain       = SDADC_Gain_1;
  ai.SDADC_CommonMode = SDADC_CommonMode_VSSA;
  ai.SDADC_Offset     = 0;
  SDADC_AINInit(SDADC2, SDADC_Conf_0, &ai);

  SDADC_ChannelConfig(SDADC2, BATTI_SDADC_CHANNEL, SDADC_Conf_0);  // select channel to use conf0
  SDADC_InjectedChannelSelect(SDADC2, BATTI_SDADC_CHANNEL); // select channel
  SDADC_InjectedSynchroSDADC1(SDADC2, ENABLE); // launch conversion synchronously with SDADC1

  SDADC_InitModeCmd(SDADC2, DISABLE); // Exit initialization mode

  // configure calibration to be performed on conf0
  SDADC_CalibrationSequenceConfig(SDADC2, SDADC_CalibrationSequence_1);
  SDADC_StartCalibration(SDADC2);    // start BATTV_SDADC Calibration
  SDADCTimeout = SDADC_CAL_TIMEOUT;    // Set calibration timeout: 5.12 ms at 6 MHz in a single calibration sequence
  while((SDADC_GetFlagStatus(SDADC2, SDADC_FLAG_EOCAL) == RESET) && (--SDADCTimeout != 0));
  if(SDADCTimeout == 0) {
    return 2; // EOCAL flag can not set
  }

  return 0;
}


// Main initialization
uint32_t InitBatteryMonitorHardware() {

  BiquadLpfCInit(&volts_filter,  kCornerFreq, SDADC_TRIGGER_FREQ);
  BiquadLpfCInit(&amps_filter,   kCornerFreq, SDADC_TRIGGER_FREQ);
  BiquadLpfCInit(&watts_filter,  kCornerFreq, SDADC_TRIGGER_FREQ);

  InitVoltageHardware();
  InitCurrentHardware();
  ConfigSyncSampling();
  return 0;
}


// Interrupt on end of conversion
void SDADC1_IRQHandler(void)
{
  if(SDADC_GetFlagStatus(SDADC1, SDADC_FLAG_JEOC) != RESET)
  {
    uint32_t word = SDADC_GetInjectedConversionSDADC12Value();
    uint8_t* word_ptr = (uint8_t*)&word;
    int16_t half1, half2;
    memcpy(&half1, word_ptr,    2);
    memcpy(&half2, word_ptr+2,  2);

    volts_raw   = (int32_t)half1 + 32768;
    volts       = volts_raw * volts_gain;
    volts_filt  = BiquadLpfCUpdate(&volts_filter, volts);

    amps_raw    = (int32_t)half2 + 32768;
    amps        = (amps_raw + amps_bias) * amps_gain; // bias = meas1/gain - raw1
    amps_filt = BiquadLpfCUpdate(&amps_filter, amps);

    watts = volts*amps;
    watts_filt = BiquadLpfCUpdate(&watts_filter, watts);

    joules = joules + watts * SDADC_TRIGGER_PERIOD;
  }
}


// Interrupt on timer, not required for normal operation
// This interrupt could be used to verify the sample frequency with a scope
// void TIM14_IRQHandler(void)
// {
//   if (TIM_GetITStatus(SDADC_TRIGGER_TIM, MAKENAME(TIM_IT_CC,SDADC_TRIGGER_TIM_OC_NUM)) != RESET) {
//     TIM_ClearITPendingBit(SDADC_TRIGGER_TIM, MAKENAME(TIM_IT_CC,SDADC_TRIGGER_TIM_OC_NUM));
//   }
// }


#endif // USING_BATTERY_MONITOR