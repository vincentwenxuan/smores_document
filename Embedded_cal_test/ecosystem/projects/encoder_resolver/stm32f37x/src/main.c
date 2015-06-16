/** 
** Encoder resolver.  This project mimics a resolver from a magnetic rotary encoder.
**/

#include "stm32f37x.h"
#include "mGeneral.h"
#include "common_peripherals.h"
#include "sin_4096_float.h"
#include "cos_4096_float.h"

/// Include your custom headers here

/// Initialize variables here
#define ADC2V (0.0008056640625f)
#define V2DAC (1241.2121212121212121212121212121f)
#define MEAN_V (1.65f)
float e_r1_r2;//, e_s1_s3, e_s2_s4;
uint32_t theta;

void DAC_Config(void)
{
  DAC_InitTypeDef    DAC_InitStructure;
  GPIO_InitTypeDef   GPIO_InitStructure;

  /* Enable GPIOA clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  
  /* Configure PA.04 (DAC1_OUT1) in analog mode -------------------------*/
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  /* Configure PA.05 (DAC1_OUT2) in analog mode -------------------------*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  /* Enable DAC clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC1, ENABLE);
  
  /* DAC1 channel 1 and 2 Configuration */
  DAC_DeInit(DAC1);  
  DAC_StructInit(&DAC_InitStructure);
  DAC_InitStructure.DAC_Trigger = DAC_Trigger_None;
  DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
  DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable;
  DAC_Init(DAC1, DAC_Channel_1, &DAC_InitStructure);
  DAC_Init(DAC1, DAC_Channel_2, &DAC_InitStructure);
  
  /* Enable DAC1 Channel 1 and 2 */
  DAC_Cmd(DAC1, DAC_Channel_1, ENABLE);
  DAC_Cmd(DAC1, DAC_Channel_2, ENABLE);
}

void ADC_Config(void)
{  
  ADC_InitTypeDef    ADC_InitStructure;
  GPIO_InitTypeDef   GPIO_InitStructure;

  /* Enable GPIOA clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  
  /* ADC1 Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
  
  /* Configure ADC Channel5 (PA5) as analog input */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  /* ADC1 DeInit */
  ADC_DeInit(ADC1);
  ADC_StructInit(&ADC_InitStructure);
  
  /* Configure the ADC1 in continuous mode */
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfChannel = 1;
  ADC_Init(ADC1, &ADC_InitStructure);

  /* ADC1 Regular Channel6 Config */ 
  ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 1, ADC_SampleTime_28Cycles5); // APB2 = 36mHz, 1/(36mHz/(28.5+1.5+1)) = 1.16666 us

  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);     

  /* ADC1 Start Conversion */ 
  ADC_SoftwareStartConv(ADC1);
}

// main function
int main(void)
{
  InitPeripherals();  // configure GPIO, USB, I2C, ect based on peripherals.h
  //blue LED set to CSn
  /// Initialize code here
  /* DAC configuration ---------------------------------------------*/
  DAC_Config();

  /* ADC configuration ---------------------------------------------*/
  ADC_Config();
  
  // MagDec in, Pullup on
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_PinLockConfig(GPIOB, GPIO_Pin_5);
  
  mWhiteON;
  DelaySeconds(1);
  mWhiteOFF;
  
  // main loop
  while(1)
  {
    // Get ADC
    e_r1_r2 = ((float)ADC_GetConversionValue(ADC1)) * ADC2V - MEAN_V;
      
    if(GPIOB->IDR & GPIO_Pin_5)
    {
      // Get Encoder
      theta = encoderReadTicks();
      mGreenON;
      mAmberOFF;
    }
    else
    {
      theta = 0;
      mAmberON;
      mGreenOFF;
    }
    
    DAC_SetDualChannelData(DAC1, DAC_Align_12b_R, (uint16_t)((e_r1_r2*cos_4096_float[theta] + MEAN_V)*V2DAC), (uint16_t)((e_r1_r2*sin_4096_float[theta] + MEAN_V)*V2DAC));

    mWhiteTOGGLE;
  }
  
  return(0);
}
