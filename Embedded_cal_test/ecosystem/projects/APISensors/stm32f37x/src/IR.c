/**
 * Cedric Destin
 **/ 
#include "IR.h"
/** Initialize variables here
 * 	ret: Use to check successes/failures
 * 	GPIO_InitStructure
 * 	ADC_InitStructure
 **/ 

void IRConfig(void){
  // configure GPIO, USB, I2C, ect based on peripherals.h

  ADC_InitTypeDef     ADC_InitStructure; 
  /*
   * ADC struct, 
   * look at stm32f37x_adc.c l219
   */
  GPIO_InitTypeDef    GPIO_InitStructure;
  /*
   * GPIO Struct, you know this
   */
  //InitPeripherals();  
  /* 
   * Configure GPIO, USB, I2C, ect based on peripherals.h Jimmy's
   * You should look more into this
   */
  
  RCC_ADCCLKConfig(RCC_PCLK2_Div4); 
  /* 
   * Defines the ADC clk divider for ADC
   */

  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE); 
  /*
   * Enables AHB
   * AHB is Advanced Microcontroller Bus Architecture
   * High PERFORMANCE BUS
   * 
   * 
  */
  
  /* Configure ADC Channel9 as analog input */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;		/*Analog mode*/
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  /* ADC1 Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); 
  /*
   * Advanced Peripheral Bus
   * I think this choses to turn on the clock for ADC1 (enabling)
   */
  /*
   * ADC1 = PB1
   */
  
  /* ADCs DeInit */  
  ADC_DeInit(ADC1);
  /*
   * DeInit = reset
   */
  /* Initialize ADC structure */
  ADC_StructInit(&ADC_InitStructure);
  /*
   * Look into stm32f37x_adc l219
   * 	-1) Scan mode
   *   -2) Continuous conversion mode
   * 	-3) External 
   * 	-4 Data align
   * 	-5) Nbr channel
   */
  
  /* Configure the ADC1 in continuous mode */
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfChannel = 1;
  ADC_Init(ADC1, &ADC_InitStructure);
  
  /* Convert the ADC1 Channel 8 with 55.5 Cycles as sampling time */ 
  ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_55Cycles5);
  /*
   * Sets the ADC1,
   * Sets the rank
   * Channel is in datasheet
   * Rank = ? The rank in the regular group sequencer. This
      parameter must be between 1 to 16.
   * * Sets the Sample time
   */
  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE); // Enables ADC1
  /*
   * Enable ADC1
   */
  /* ADC1 reset calibration register */   
  ADC_ResetCalibration(ADC1); // This is for tempereature registers
  /*
   * Reset calibration register????
   */
  
  while(ADC_GetResetCalibrationStatus(ADC1));{
  }
  
  /* ADC1 calibration start */
  ADC_StartCalibration(ADC1);
  
  while(ADC_GetCalibrationStatus(ADC1)); 
  
  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);
}
void IR_range(float* Len_Vol){
   while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
    
    /* Get ADC1 converted data */
    uint16_t ADC1ConvertedValue = ADC_GetConversionValue(ADC1);
    
    /* Compute the voltage */
    float ADC1Voltage = (float)(ADC1ConvertedValue);
    
    float ADC1ConvertedVoltage = (ADC1Voltage - 1196) / 967;
    
    float Length = (123.4 / (ADC1ConvertedVoltage * ADC1ConvertedVoltage + 3.543 * ADC1ConvertedVoltage + 4.712)) * 2.54;
    
    float Voltage = 3.3 * ADC1Voltage /4096;
    
    Len_Vol[0] = Length;
    
    Len_Vol[1] = Voltage;
}