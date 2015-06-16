/**
  *****************************************************************************************
  * @file    lib_adc.c 
  * @author  Chaitanya Bhargava (cbhar)
  * @version V1.1.0
  * @date    06-June-2014
  * @brief   This file contains the function to turn on ADC for a particular channel
  *****************************************************************************************
*/
#include "lib_adcplatform_config.h"
#include "lib_adc.h"
#include "mGeneral.h"
//**************************************************************************************
//
//!  ADC_A_config
//!
//!  @param    none
//!   
//!  @return   none
//!
//!  @brief    This will turn on an ADC channel based of the specifications in the 
//!            lib_adcplatform_config.h. This function can be copied to turn on more than 1
//!            ADC channel at a time. To see how to read the ADC value see the corresponding
//!            main.c in fundanmental_library/ADC
//
//**************************************************************************************
void ADC_A_config()
{
  __IO uint32_t TimingDelay = 0, calibration_value = 0;
   /* Configure the ADC clock */
  RCC_ADCCLKConfig(RCC_ADC12PLLCLK_Div2);
  
  /* Enable ADC_A_NUM clock */
  RCC_AHBPeriphClockCmd(ADC_A_RCC, ENABLE);
      
  /* Setup SysTick Timer for 1 µsec interrupts  */
  // if (SysTick_Config(SystemCoreClock / 1000000))
  // { 
  //   /* Capture error */ 
  //   while (1)
  //   {}
  // }
  ADC_InitTypeDef       ADC_InitStructure;  
  ADC_CommonInitTypeDef ADC_CommonInitStructure;
  GPIO_InitTypeDef    GPIO_InitStructure;
  
  /* ADC Channel configuration */

  /* Configure ADC Channel7 as analog input */
  GPIO_InitStructure.GPIO_Pin = ADC_A_PIN ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(ADC_A_PORT, &GPIO_InitStructure);
  
  ADC_StructInit(&ADC_InitStructure);

  /* Calibration procedure */ 
  ADC_VoltageRegulatorCmd(ADC_A_NUM, ENABLE);
  
  /* Insert delay equal to 10 µs */
  mWaitus(10);
  
  ADC_SelectCalibrationMode(ADC_A_NUM, ADC_CalibrationMode_Single);
  ADC_StartCalibration(ADC_A_NUM);
  
  while(ADC_GetCalibrationStatus(ADC_A_NUM) != RESET );
  calibration_value = ADC_GetCalibrationValue(ADC_A_NUM);
     
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;                                                                    
  ADC_CommonInitStructure.ADC_Clock = ADC_Clock_AsynClkMode;                    
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;             
  ADC_CommonInitStructure.ADC_DMAMode = ADC_DMAMode_OneShot;                  
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = 0;          
  
  ADC_CommonInit(ADC_A_NUM, &ADC_CommonInitStructure);
  
  ADC_InitStructure.ADC_ContinuousConvMode = ADC_ContinuousConvMode_Enable;
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b; 
  ADC_InitStructure.ADC_ExternalTrigConvEvent = ADC_ExternalTrigConvEvent_0;         
  ADC_InitStructure.ADC_ExternalTrigEventEdge = ADC_ExternalTrigEventEdge_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_OverrunMode = ADC_OverrunMode_Disable;   
  ADC_InitStructure.ADC_AutoInjMode = ADC_AutoInjec_Disable;  
  ADC_InitStructure.ADC_NbrOfRegChannel = 1;
  ADC_Init(ADC_A_NUM, &ADC_InitStructure);
  
  /* ADC_A_NUM regular channel7 configuration */ 
  ADC_RegularChannelConfig(ADC_A_NUM, ADC_A_CHANNEL, 1, ADC_SampleTime_7Cycles5);
   
  /* Enable ADC_A_NUM */
  ADC_Cmd(ADC_A_NUM, ENABLE);
  
  /* wait for ADRDY */
  while(!ADC_GetFlagStatus(ADC_A_NUM, ADC_FLAG_RDY));
  
  /* Start ADC_A_NUM Software Conversion */ 
  ADC_StartConversion(ADC_A_NUM);   
}