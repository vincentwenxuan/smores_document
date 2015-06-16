/*
 * Cedric Destin
 * Date 6/10
 * IR & ADC
 * Hardware:
 * 	M4
 * 	IR = Sharp - GP2Y0A02YK 
 */
#include "stm32f37x.h"
#include "mGeneral.h"
#include "common_peripherals.h"
#include "stdio.h"
/// Include your custom headers here
#include "usb_interface.h"
#include "types.h"
/// Initialize variables here
float  ADC1ConvertedVoltage = 0;
const uint16_t period = 10; // ms
const uint16_t blink_duration = 1; // ms
// main function
int main(void){
  InitPeripherals();  // configure GPIO, USB, I2C, ect based on peripherals.h
  
  uint8_t tx_buf[136];
  uint8_t tx_len;
  
  // initialize USB interface object
  struct UsbInterface usb_interface;
  InitUsbInterface(&usb_interface);

  mGreenON;
    
  __asm__(".global _printf_float");
  __asm__(".global _scanf_float");
  
  uint16_t  ADC1ConvertedValue = 0;
  //Runuint16_t  ADC1ConvertedValue = 0;
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
  printf("\rADC CLK set\n");
  /* 
   * Defines the ADC clk divider for ADC
   */

  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE); 
  printf("\rAHB enabled\n");
  /*
   * Enables AHB
   * AHB is Advanced Microcontroller Bus Architecture
   * High PERFORMANCE BUS
   * 
   * 
  */
  
  /* Configure ADC Channel9 as analog input */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;		/*Analog mode*/
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  printf("\rPB1 set as ADC\n");
  
  /* ADC1 Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); 
  /*
   * Advanced Peripheral Bus
   * I think this choses to turn on the clock for ADC1 (enabling)
   */
  printf("\rADC1 CLK enabed\n");
  /*
   * ADC1 = PB1
   */
  
  /* ADCs DeInit */  
  ADC_DeInit(ADC1);
  /*
   * DeInit = reset
   */
  printf("\r Deinit\n");
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
  printf("\rContinuous mode set\n");
  
  /* Convert the ADC1 Channel 9 with 55.5 Cycles as sampling time */ 
  ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 1, ADC_SampleTime_55Cycles5);
  /*
   * Sets the ADC1,
   * Sets the rank
   * Channel is in datasheet
   * Rank = ? The rank in the regular group sequencer. This
      parameter must be between 1 to 16.
   * * Sets the Sample time
   */
  printf("\rSampling time and bla blah blah...\n");
  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE); // Enables ADC1
  /*
   * Enable ADC1
   */
  printf("\rEnable ADC1 = PB1\n");
  /* ADC1 reset calibration register */   
  ADC_ResetCalibration(ADC1); // This is for tempereature registers
  /*
   * Reset calibration register????
   */
  printf("\rReset ADC1\n");
  
  while(ADC_GetResetCalibrationStatus(ADC1));{
    printf("\r...\n");
  }
  printf("\rCalibration passed\n");
  
  /* ADC1 calibration start */
  ADC_StartCalibration(ADC1);
  
  while(ADC_GetCalibrationStatus(ADC1)); 
  printf("\rCalibrated\n");

  
  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);
  printf("\rADC1 Enabled\n");
  
  /// Initialize code here
  
  while(1){ //9in 0.76in = ir 10 object 10-0.76 = 9.64  2150 = 9.64in 
			//				  x    = 1in
    /*
     * 20cm = 7.87402in
     * 
     * 150cm = 59.0551in
     */
    /* Test EOC flag */
    while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
    
    /* Get ADC1 converted data */
    ADC1ConvertedValue =ADC_GetConversionValue(ADC1);
    
    /* Compute the voltage */
    float ADC1Voltage = (float)(ADC1ConvertedValue);
    
    float ADC1ConvertedVoltage = (ADC1Voltage - 1196) / 967;
    
    float Length = (123.4 / (ADC1ConvertedVoltage * ADC1ConvertedVoltage + 3.543 * ADC1ConvertedVoltage + 4.712)) * 2.54;
    
    float Voltage = 3.3 * ADC1Voltage /4096;
    
    //float convertedvalue = ADC1ConvertedVoltage;
    
    //printf("\rlength: %f\t voltage: %f\n", Length, Voltage);
        
  
    // Set up CDC
    uint8_t msg_type = 1;
    
    // Packing data into struct
    MsgMyData data_out;
    data_out.float_var = Length;
    
    // Packetize the data
    FormPacket(msg_type, (uint8_t*)&data_out, sizeof(MsgMyData),
               tx_buf, &tx_len);

    // Send
    CDC_Send_DATA(tx_buf, tx_len);
    
  }
  return(0);
}