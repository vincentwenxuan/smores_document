/**
 * Cedric Destin
 **/ 
/** Include your custom headers here
 * 
 **/
#include "MB1010.h"
/** Initialize variables here
 * 	ret: Use to check successes/failures
 * 	GPIO_InitStructure
 * 	TIM_ICInitStructure
 * 	ADC_InitStructure
 * 	USART_InitStructure
 **/ 
GPIO_InitTypeDef GPIO_InitStructure;
TIM_ICInitTypeDef TIM_ICInitStructure;
USART_InitTypeDef USART_InitStructure;
void MB1010Config_AN(void){
  ADC_InitTypeDef     ADC_InitStructure; 
  GPIO_InitTypeDef    GPIO_InitStructure;
  RCC_ADCCLKConfig(RCC_PCLK2_Div4);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
  ADC_DeInit(ADC1);
  ADC_StructInit(&ADC_InitStructure);
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfChannel = 1;
  ADC_Init(ADC1, &ADC_InitStructure);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 1, ADC_SampleTime_55Cycles5);
  ADC_Cmd(ADC1, ENABLE);
  ADC_ResetCalibration(ADC1);
  while(ADC_GetResetCalibrationStatus(ADC1));
  ADC_Cmd(ADC1, ENABLE);
}
void MB1010Config_PW(void){
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_1);
  TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
  TIM_ICInitStructure.TIM_ICFilter = 0x0;
  TIM_PWMIConfig(TIM2, &TIM_ICInitStructure);
  TIM_SelectInputTrigger(TIM2, TIM_TS_TI2FP2);
  TIM_SelectSlaveMode(TIM2, TIM_SlaveMode_Reset);
  TIM_SelectMasterSlaveMode(TIM2, TIM_MasterSlaveMode_Enable);
  TIM_Cmd(TIM2, ENABLE);
  TIM_ITConfig(TIM2, TIM_IT_CC2, ENABLE);
}
void MB1010Range(float* PW_AN){
  uint32_t timer = TIM_GetCapture1(TIM2);
  int Lengthp = 0.0002456*timer + 8.985;     
  while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
  uint16_t ADC1ConvertedValue = ADC_GetConversionValue(ADC1);
  int Lengtha = 0.3225*ADC1ConvertedValue + 17.6;
  PW_AN[0] = Lengthp;
  PW_AN[1] = Lengtha;
}
//TODO: USART