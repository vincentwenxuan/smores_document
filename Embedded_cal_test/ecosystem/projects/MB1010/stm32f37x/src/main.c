/**
 ** Cedric Destin
 ** Date 6/15
 ** MB1010 Sonar Sensor using PWM ADC UART
 ** PWM and ADC work, UART is not, I think it has to do with the number of bytes that are sent...
 ** I will try to communicate with it using the FTDI... Prob on Windows...
 ** Still not working... We analyized using a digital analyzer... we saw that the bit were inverted... so we inverted them and still the M4
 ** Does not receive...
 ** However, we are able to send and receive single bytes... my next guess would be look into the DMA... something like that
 ** But it does not make sense honeslty, it does not make sense... I don't really get it...
 ** I' ll move into I2C
 **/
/** Includes
 ** stm32f37x
 ** mGeneral
 ** common_peripherals
 ** stdio
 **/
#include "stm32f37x.h"
#include "mGeneral.h"
#include "common_peripherals.h"
#include "stdio.h"
/** Custom Headers
 ** Created by Jimmy, it lets me send data to a python script...
 **/
#include "usb_interface.h"
#include "types.h"
/** Initialized variables
 ** GPIO_InitStructure
 ** TIM_ICInitStructure
 ** ADC_InitStructure
 ** USART_InitStructure
 ** usb_interface
 ** tx_buf[136]
 ** tx_len
 **/
GPIO_InitTypeDef GPIO_InitStructure;
TIM_ICInitTypeDef TIM_ICInitStructure;
ADC_InitTypeDef ADC_InitStructure;
USART_InitTypeDef USART_InitStructure;
uint8_t tx_buf[136];
uint8_t tx_len;
/** 
 ** Declared Functions
 ** TIM_Config: configure the PWM
 ** ADC_Config: configure ADC1
 ** Usart1Get: reads a byte from certain pin
 ** Usart1Put: Write a byte from certain pin
 **/
void TIM_Config(void);
void ADC_Config(void);
void Usart1Init(void);
void Usart1Put(uint8_t ch);
uint8_t Usart1Get(void);
/**
 ** Main Function
 **/
int main(void){
  InitPeripherals();
  InitUsbInterface();

  /** Description
   ** Very important
   **/
  __asm__(".global _printf_float");
  __asm__(".global _scanf_float");
  //TIM_Config();
  //ADC_Config();
  Usart1Init();
  mGreenON;
//   Usart1Put('A');
//   mWaitms(1000);
//   Usart1Put('R');
//   mWaitms(1000);
//   Usart1Put('M');
//   Usart1Put('A');
//   mWaitms(10);
  uint8_t data;
  
  while(1){
//     uint32_t timer = TIM_GetCapture1(TIM2);
//     int Lengthp = 0.0002456*timer + 8.985;
//     
//     while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
//     uint16_t ADC1ConvertedValue = ADC_GetConversionValue(ADC1);
//     int Lengtha = 0.3225*ADC1ConvertedValue + 17.6;
//     
    mYellowTOGGLE;
    
//     //Set up CDC
//     uint8_t msg_type = 1;
//     
//     // Packing data into struct
//     MsgMyData data_out;
//     data_out.float_var = Length;
//     
//     // Packetize the data
//     FormPacket(msg_type, (uint8_t*)&data_out, sizeof(MsgMyData),
//                tx_buf, &tx_len);
// 
//     //Send
//     CDC_Send_DATA(tx_buf, tx_len);
    
//     printf("\rPW = %d\t Lengthp = %d\n", timer, Lengthp);
//     printf("\rADC = %d\t Lengtha = %d\n", ADC1ConvertedValue, Lengtha);
//     
    uint8_t data = Usart1Get();
    //printf("\r%d\n", data);
    mWaitms(10);
  }
  return 0;
}
void TIM_Config(void){
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
  TIM_SelectMasterSlaveMode(TIM2,TIM_MasterSlaveMode_Enable);
  
  TIM_Cmd(TIM2, ENABLE);
  
  TIM_ITConfig(TIM2, TIM_IT_CC2, ENABLE);
}
void Usart1Init(void){
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; //NOPULL?
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; //NOPULL?
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_7);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_7);
  
  USART_InitStructure.USART_BaudRate = 9600;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  
  USART_Init(USART1, &USART_InitStructure);
  
  USART_Cmd(USART1, ENABLE);
}
void Usart1Put(uint8_t ch){
  USART_SendData(USART1, (uint8_t) ch);
  while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET){
    /* Wait until the transmission flag is good
     * 
     */
    mRedON;
  }
  USART_ClearFlag(USART1, USART_FLAG_TC);
  mRedOFF;
}
uint8_t Usart1Get(void){
    while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET){
      /* 
       * Get status of Receive flag
       */
      mRedON;
    }
    mRedOFF;
    uint8_t receive = (uint8_t)USART_ReceiveData(USART1);
    USART_ClearFlag(USART1, USART_FLAG_RXNE);
    return receive;
}
void ADC_Config(void){
  RCC_ADCCLKConfig(RCC_PCLK2_Div4);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;		/*Analog mode*/
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
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
  
  while(ADC_GetCalibrationStatus(ADC1)); 
  ADC_Cmd(ADC1, ENABLE);
}
