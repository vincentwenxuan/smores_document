/**
 * @file TestingStand.c 
** @author Cedric Destin 
** @version V1
** @date 07/30/2013
** @brief
** 	Proposed Method
** 	 Look over quadrature 1 more time
** 	TODO
	Find resistor values for IR
	  DONE
	Read 2 AN channels
	  It seems that it only wants to read one channel
	    Cedric: Read the Ref
	    DONE
	      Matt helped me
	Test the Baro again TODO
	Find weight of cart TODO
	Using timers to create two square waves and read
	Read them using AN
	  To test quadrature
	    I am not sure if the square wave will be too fast.
	The program is acting when I start the timer used for generating the quadrature for some reason the ADC stop working
	TODO: Find the problem between the interface
	  DONE
	TODO: Send packets over USB
	  DONE
	TODO
	  Extract some intelligence from the encoder
	    Also it should be easy for you to integrate the barometer
	    and sonar
	  The IR and sonar are easy to integrate but the barometer is retarted
	  You cannot wait for a whole year until the barometer is ready
	  One thing that you can do is have a "corde sortie" so that you can get out of that trap
	  So try to implement that
	  Oh make sure that you have better comment
	  Oh also you will need better plots fot the barometer
	  And .json is useless outside of python don ever use it again... 
	    Or at least use txt file anyways
	  Oh maybe try to use a .json to .txt file converter
	    Yeah I have an idea... Read the .json into a list then convert the list to string
	    then print them to a txt file
	  Back to the encoder oh change the json lol so that it lets you have IR, Sonar, Baro, Encoder(Displacement, Direction)
	  TODO Get data for baro, and finding the units... clearly the data is changing but I have no idea what the units are....
	  Look at the code for the MPL311 for reference
**/
/** Include common libraries
 * @include stm32f37x
 * 	@brief: Has the libraries related to the stm (GPIO etc...)
 * @include mGeneral
 *	@brief: Has nothing at the moment
 * @include common_peripherals
 *	@brief: Has the MAKENAME to include for the necessary peripherals such as I2C
 * @include stdio
 * 	@brief Has the printf and scanf
 * 		Never looked at it
 **/
#include "stm32f37x.h"
#include "mGeneral.h"
#include "common_peripherals.h"
#include "stdio.h"
/** Include your custom headers here
 * @include timebase
 *	@brief: Used to run a code at an interrupt
 * @include project_message_types
 *	@brief: Sets the data types to be sent
 * @include usb_interface
 * 	@brief: struct interface for cdc_data to run
 * @include MPL311
 * 	@brief
 * @include system_clock
 * 	@brief Has the precise timings
 **/
#include  "MPL311.h"
#include "timebase.h"
#include "project_message_types.h"
#include "usb_interface.h"
#include "system_clock.h"
/** #Defines
 * @var MPL
 * 	@brief: Address of MPL311 for I2C
 **/
#define MPL 0b11000000
// #define Using_IR
//#define Using_Sonar
#define Using_Baro
/** Declare function prototypes 
 * @fn ADCinit
 * @brief Initializes the ADC to read the quadrature encoder
 * 
 * @param GPIO_InitStructure Structure that contains the parameter
 * used to initialize the GPIOs
 * @param GPIOx Port B is the port used for ADC
 * @param GPIO_Pin_x Pins 0 and 1 = QuadA and QuadB respectively
 * 	* 6 maybe for either the IR or the Sonar
 * @param GPIO_Mode Mode will analog therefore: GPIO_Mode_AN
 * @param ADCx ADC1 the ADC1 is used...
 * @param ADC_InitStructure Structure that contains the parameter 
 * 	* used to initialize the ADC
 * @param ADC_ScanConvMode very important it allows to read from multiple
 * 	channels
 * @param ADC_DataAlign the ADC has a resolution of 12bits
 * 	* so we must adjust right ADC_DataAlign_Right
 * @param ADC_NbrOfChannel we want 3 channels
 * @fn ADC_RegularChannelConfig
 * 	* @brief this is a buitl in function that configs
 *	 	* the multiple ADC channels
 * @fn DMAinit
 * @brief This initializes the DMA structure
 * 	* to be used with the ADC
 * @param DMA_InitStructure Structure used to initialize the
 * 	* DMA
 * @param DMA_PeripheralBaseAddr address 1
 * 	* ADC->DR
 * @param DMA_MemoryBaseAddr address 2 
 * 	* ADC_input: Array
 * @param DMA_DIR bus direction
 * 	* Sets ADC as the source
 * @param DMA_BufferSize
 * @param DMA_MemoryInc increment through the array
 * @param DMA_Mode make the mode circular to circulate through
 * 	* the array
 **/
void ADCinit(void);
void TIMinit(void);
void DMAinit(void);
void TIM_Config(void);
void MPLINT(uint8_t mode, uint8_t* Intdata, uint8_t length);
void GetDataBurst(uint8_t length, uint8_t I2Cx, uint8_t* data, uint8_t Regaddr);	// Get Data using Burst Mode
void MPL311_init(void);
void IR_range(float* Len_Vol);
void ChkI2C(uint8_t Adrs, uint8_t I2Cx);
void CalAlt(uint8_t I2Cx);
void GetAlt(uint8_t I2Cx);
void MPL311_init(void);
void GetDataBurst(uint8_t length, uint8_t I2Cx, uint8_t* data, uint8_t Regaddr);
void Start_Baro(uint8_t Starter);
void SkperI2C(uint8_t Adrs, uint8_t I2Cx);
/** Initialize variables here
 * @param GPIO_InitStructure
 * 	@brief Structure for General Purpose Inputs and Outputs
 * @param ADC_InitStructure
 * 	@brief Structure for ADC
 * @param DMA_InitStructure
 * 	@brief Structure for DMA
 * @param TIM_ICInitStructure
 **/
GPIO_InitTypeDef GPIO_InitStructure;	/// Standard GPIO struct
ADC_InitTypeDef ADC_InitStructure;	/// ADC struct
DMA_InitTypeDef DMA_InitStructure;	/// DMA struct
TIM_ICInitTypeDef TIM_ICInitStructure;
/**
 * @var ADC1ConvertedValue
 * 	* Not used
 * @var ADC_input
 * 	* Array which gets the ADC values using DMA
 * @var QuadA, QuadB
 * 	* QuadratureA and QuadratureB
 * @var QuadA_p, QuadB_p
 * 	* Previous values of the QuadratureA and QuadratureB
 * @var i
 * 	* Index used for loops
 * @var counter
 * 	* counts the displacements
 * @var threshold
 * 	* threshold value to digitize the ADC
 * @var direction
 * 	* Direction of the cart
 * @var tx_buf
 * 	* buffer for CDC_Send_DATA, to send data thru USB
 * @var tx_len
 * 	* length for C_Send_DATA
 * @var A
 * 	* Dummy variable
 **/
uint16_t  ADC1ConvertedValue = 0;	/// ADC output Not used
uint16_t ADC_input[3];	///	DMA's magic which inserts the ADC into an array
uint16_t QuadA;	/// QuadA
uint16_t QuadB;	/// QuadB
uint16_t QuadA_p;	/// QuadA_p
uint16_t QuadB_p;	/// QuadB_p
uint8_t i = 1;	/// index
int16_t counter = 0;	/// counter for height
uint8_t sentinel = 0;
uint16_t threshold = 2500;
int8_t direction;
int16_t A = 0;
float Fcounter = 0;
float Lengthp = 0;
uint32_t timer;
uint8_t alpha = 0.5;
uint16_t Bias_Sonar = 0;
/// Those are inported from MPL311
/** MPL311 Variables
 * @var ret:
 * 	@brief: Dummy variable
 * @var n:
 * 	@brief Dummy variable
 * @var Bias:
 * 	@brief Bias value used to remove the Sea level components
 * @var data[]:
 * 	@brief The array is used for the busrt mode of the I2C (SIZE 5)
 * @var Intdata[]
 * 	@brief The array is used for the interrupt mode of the MPL311
 * @var t, t0
 * 	@brief Those to variables are used to obtain the time difference in the program
 * @var ADC1ConvertedVoltage
 * 	@brief This variable is not used
 * @var Len_Vol[]
 * 	@brief This array is the converted values of the IR
 * @var Altitude
 * 	@brief Struct that contains the Altitude values of the barometer
 * @var Temp
 * 	@brief Struct that contains the Temperature values of the barometer
 **/
uint16_t ret = 0;	/// Dummy variable
uint8_t n = 0;		/// Dummy loop variable
float Bias = 0;		/// Bias to calibrate the altitude with reference to initial position
uint8_t data[5];	/// Data array
uint8_t Intdata[6];	/// Interupt data array
uint32_t t, t0 = 0;
float  ADC1ConvertedVoltage = 0;
float Len_Vol[2];
float Lengthp_prev;
uint8_t starter = 0;
uint8_t DeviceReady = 0;
struct ALTITUDE Altitude;
struct TEMP Temp;
/// main function
int main(void){
  InitPeripherals();  /// Configure GPIO, USB, I2C, ect based on peripherals.h
  InitUsbInterface();
  InitSystemClock(); /// Setup "stopwatch"
  __asm__(".global _printf_float");
  __asm__(".global _scanf_float");
  
  ret = mBusPeriphReady(MPL,1);
   while(!ret){				// Check if the MPL is ready
     ret = mBusPeriphReady(MPL,1);
     mRedON;
   }
   ret = mBusWrite(MPL, 0x26, 0xB8, 1);
  while(!ret){				// Check if the writing was successful
    ret = mBusWrite(MPL, 0x26, 0xB8, 1);
    mRedON;
  }
  ret = mBusWrite(MPL, 0x13, 0x07, 1);
  while(!ret){
    ret = mBusWrite(MPL, 0x13, 0x07, 1);
    mRedON;
  }
  MPLINT(4, Intdata, 0);
  mRedOFF;
  /// Initialize code here
  /// Setup the DMA
  DMAinit();
  /// Setup the ADC
  ADCinit();
  /// Setup PWM
  TIM_Config();
  /// Continuous loop
  uint32_t timer = TIM_GetCapture1(TIM2);
  for(n = 0; n < 255; n++){
    uint32_t timer = TIM_GetCapture1(TIM2);
    Bias_Sonar = 0.0002456*timer + 8.985 + Bias_Sonar;
  }
  Bias_Sonar = Bias_Sonar / n;
  n = 0;
  ChkI2C(MPL, 1);
  CalAlt(1);
  while(1){
    t0 = GetMicroseconds();
    
    /// Wait for the DMA to read
     while((DMA_GetFlagStatus(DMA1_FLAG_TC1)) == RESET){
     }
    DMA_ClearFlag(DMA1_FLAG_TC1);
    
    /// Read the ADC values
    QuadA = ADC_input[0];
    QuadB = ADC_input[1];
    ret = ADC_input[2];
    
    /// Digitize the incoming values
    if(ADC_input[0] >= threshold){
      //mGreenON;
      QuadA = 1;
    }else{
      //mGreenOFF;
      QuadA = 0;
    }
    if(ADC_input[1] >= threshold){
      QuadB = 1;
    }else{
      QuadB = 0;
    }
     direction = 0;

     /// Decode the quadrature looking at Quad A
    if(QuadA == 1 && QuadA_p == 0){ /// Rising edge of A
      if(QuadB == 0){	/// B is leading
	direction = 1;
      }else{
	direction = -1;
      }
    }else if(QuadA == 0 && QuadA_p == 1){ 		/// Falling edge of A
      if(QuadB == 0){
	direction = -1;		/// A is leading
      }else{
	direction = 1;
      }
    }
    
    /// Decode quadratue looking at QuadB, 
    ///not doing anything for now
    if(QuadB == 1 && QuadB_p == 0){	/// Rising edge of B
      if(QuadA == 0){
	// direction = 1;	/// B is leading
      }else{
	// direction = 1;	/// A is leading
      }
    }else if(QuadB == 0 && QuadB_p == 1 ){
      if(QuadA == 0){
	//direction = 1;	/// B is leading
      }else{
	// direction = -1;	/// A is leading
      }
    }
     
     /// Add the direction to the counter to get the displacement
     counter = counter + direction;
    if(direction == 1){
      mBlueOFF;
      mWhiteON;
    }else if(direction == -1){
      mWhiteOFF;
      mBlueON;
    }
     
    /// QuadA and QUadB are stored to the previous Quads  
    QuadA_p = QuadA;
    QuadB_p = QuadB;
        
    /**
     * To slow the transmission down
     * only send data to usb when there is a transistion
     **/
    
    t = t + GetMicroseconds() - t0;
    if(direction != 0){
      SkperI2C(MPL, 1);
      IR_range(Len_Vol);
      uint32_t timer = TIM_GetCapture1(TIM2);
      Lengthp =( 0.0002456*timer + 8.985 - Bias_Sonar)/2.54 - Bias_Sonar;
      ///TODO: more comments

/// here
	 Fcounter = (float)counter/2;
	 #ifdef Using_IR /// Is the IR define
	      uint8_t msg_type = 10;
	      MsgMyData_IR data_out_IR;
	      data_out_IR.Displacement_IR = Fcounter;
	      data_out_IR.Timing_IR = t;
	      data_out_IR.ADC_IR = Len_Vol[0];
	      SendUsbPacket(msg_type, (uint8_t*)&data_out_IR, sizeof(MsgMyData_IR));
	      SendUsbNow();
	 #endif
	 #ifdef Using_Sonar
	      uint8_t msg_type = 11;
	      MsgMyData_Sonar data_out_Sonar;
	      data_out_Sonar.Displacement_Sonar = Fcounter;
	      data_out_Sonar.Timing_Sonar = t;
	      data_out_Sonar.PWM_Sonar = Lengthp;
	      SendUsbPacket(msg_type, (uint8_t*)&data_out_Sonar, sizeof(MsgMyData_Sonar));
	      SendUsbNow();
	 #endif
	 #ifdef Using_Baro
	      uint8_t msg_type = 9;
	      DeviceReady = 1;
	      if(DeviceReady == 1){
		MsgMyData_baro data_out_baro;
		data_out_baro.Displacement_Baro = Fcounter;
		data_out_baro.I2C_Alt = Altitude.valWord;
		data_out_baro.Timing_Baro = t;
		SendUsbPacket(msg_type, (uint8_t*)&data_out_baro, sizeof(MsgMyData_baro));
		SendUsbNow();
		DeviceReady = 0;
	      }
	 #endif
	mYellowTOGGLE;
    }
    mWaitms(1);
  }
  return(0);
}
void DMAinit(void){
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
  
  DMA_DeInit(DMA1_Channel1);
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ADC_input;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = 3;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);
  
  DMA_Cmd(DMA1_Channel1, ENABLE);
}
void ADCinit(void){
  RCC_ADCCLKConfig(RCC_PCLK2_Div4);	/// Configures the ADC clock
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE); /// Setup clock for GPIOB
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  /// PB.0 and PB.1 PA.6 are set for AN
  
//   RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE); /// Enable the clock for DMA
//   
//   DMA_DeInit(DMA2_Channel1);
//   DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
//   DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)Quad;
//   DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
//   DMA_InitStructure.DMA_BufferSize = 2;
//   DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
//   DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
//   DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
//   DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
//   DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
//   DMA_InitStructure.DMA_Priority = DMA_Priority_High;
//   DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
//   DMA_Init(DMA2_Channel1, &DMA_InitStructure);
//   
//   DMA_Cmd(DMA2_Channel1, ENABLE);
  /**	DMA setup
   * Denitialize channel2
   * setup the Peripheral base address
   * 	Specify the register address DMA_PeripheralBaseAddr
   * 	Specify the return value address DMA_MemoryBaseAddr
   * 		This is specific to our case look at Direction
   * 	Specify if the Peripheral register is the source or receiver
   * 	Specify the size of the array
   * 	Specifies if we want to increment in the array DMA_PeripheralInc
   * 	Specify the size of the data sent and received: DMA_PeripheralDataSize, DMA_MemoryDataSize
   * 	DMA_Mode, Turn around to first value of the array
   * 	
   */
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); /// Enable clock for ADC1
  
  ADC_DeInit(ADC1);
  
  ADC_DMACmd(ADC1, ENABLE);  	/// Enables the request from DMA to ADC
  
  ADC_StructInit(&ADC_InitStructure);
  
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;	/// Scan mode... This is to read from multiple ADC channels
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;	/// Continuous mode never stop
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfChannel = 3;
  ADC_Init(ADC1, &ADC_InitStructure);
  
  ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_55Cycles5); /// 72*10^6/55
  ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 2, ADC_SampleTime_55Cycles5);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 3, ADC_SampleTime_55Cycles5);
  // 55 = 45kHz
  ADC_Cmd(ADC1, ENABLE);
  
  ADC_ResetCalibration(ADC1);
  while(ADC_GetResetCalibrationStatus(ADC1)){
  }
  
  ADC_StartCalibration(ADC1);
  while(ADC_GetCalibrationStatus(ADC1)){
  }
  
  ADC_Cmd(ADC1, ENABLE);
}
void TIMinit(void){
//   /* TIM3 clock enable */
//   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
//   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
// 
//   /* GPIOA and GPIOB clock enable */
//   RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
//    
//   /* GPIOA Configuration: TIM2 CH1 (PA5) and TIM3 CH2 (PA6) */
//   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6;
//   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
//   GPIO_Init(GPIOA, &GPIO_InitStructure); 
//     
//   /* Connect TIM Channels to AF1 */
//   GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_1);
//   GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_2);
//   
//   //--------------------------------------------------
// 
//   /* Time base configuration */
//   TIM_TimeBaseStructure.TIM_Period = 65535;
//   TIM_TimeBaseStructure.TIM_Prescaler = 36;
//   TIM_TimeBaseStructure.TIM_ClockDivision = 0;
//   TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
// 
//   TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
// 
//   /* Output Compare Toggle Mode configuration: Channel1 */
//   TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Toggle;
//   TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
//   //TIM_OCInitStructure.TIM_Pulse = 100;
//   TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
//   TIM_OC1Init(TIM3, &TIM_OCInitStructure);
// 
//   TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Disable);
// 
//   /* Output Compare Toggle Mode configuration: Channel2 */
//   TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
// //  TIM_OCInitStructure.TIM_Pulse = 1000;
// 
//   TIM_OC2Init(TIM3, &TIM_OCInitStructure);
// 
//   TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Disable);
// 
//   /* TIM enable counter */
//   TIM_Cmd(TIM3, ENABLE);
//   
//   //-----------------------------------------------------
//   
//   
//   TIM_TimeBaseStructure.TIM_Period = 65535;
//   TIM_TimeBaseStructure.TIM_Prescaler = 72;
//   TIM_TimeBaseStructure.TIM_ClockDivision = 0;
//   TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
//   
//   TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
//   
// 
//   /* Output Compare Toggle Mode configuration: Channel1 */
//   TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Toggle;
//   TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
//   //TIM_OCInitStructure.TIM_Pulse = 100;
//   TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
//   TIM_OC1Init(TIM2, &TIM_OCInitStructure);
// 
//   TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Disable);
// 
//   /* Output Compare Toggle Mode configuration: Channel2 */
//   TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
// //  TIM_OCInitStructure.TIM_Pulse = 1000;
// 
//   TIM_OC2Init(TIM2, &TIM_OCInitStructure);
// 
//   TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Disable);
// 
//   /* TIM enable counter */
//   TIM_Cmd(TIM2, ENABLE);
//   
//   //TIM_ITConfig(TIM3, TIM_IT_CC1, ENABLE);
}
void timerCallback1(void){
    if(sentinel % 2 == 0){ // Quad A is switched QuadB = QuadB
    QuadA = ~QuadA;
    QuadB = QuadB;
    //GPIO_WriteBit(GPIOA, 0, ret); // QuadA
    GPIOA->ODR ^= (1<<0);
    //mWhiteTOGGLE;
    //mYellowTOGGLE;
    //GPIO_WriteBit(GPIOA, GPIO_Pin_1, ~ret); // QuadB
  }else{	// QuadB = is switched QuadA = Quad A
    QuadB = ~QuadB;
    QuadA = QuadA;
    //mBlueTOGGLE;
    GPIOA->ODR ^= (1<<1);
    //GPIO_WriteBit(GPIOA, GPIO_Pin_1, ret); // QuadB
    //GPIO_WriteBit(GPIOA, GPIO_Pin_0, ~ret); // QuadA
  }
  sentinel = sentinel + 1;
}
void Start_Baro(uint8_t Starter){
  if(Starter == 1){
    GetDataBurst(3, 1, data, 1);
  }
}
void ChkI2C(uint8_t Adrs, uint8_t I2Cx){	/// How long is this function
  mRedON;
  ret = mBusRead(Adrs, 0x00, I2Cx);
    while((ret & 0x08) == 0){
      /// Checks if the MPL is ready to read i am not sure why it is anded
      ret = mBusRead(Adrs, 0x00, I2Cx);
    }
    mRedOFF;
}
void SkperI2C(uint8_t Adrs, uint8_t I2Cx){	/// How long is this function
  mRedON;
  ret = mBusRead(Adrs, 0x00, I2Cx);
    if((ret & 0x08) != 0){
      /// Checks if the MPL is ready to read i am not sure why it is anded
       DeviceReady = 1;
      GetDataBurst(3,1,data, 1);
    }else{
       DeviceReady = 0;
    }
    mRedOFF;
}
void CalAlt(uint8_t I2Cx){
  while(n < 5){
    ChkI2C(MPL, 1);
    mWhiteTOGGLE;
    Altitude.hiByte = mBusRead(MPL, 0x01, I2Cx);
    Altitude.loByte = mBusRead(MPL, 0x02, I2Cx);
    Altitude.fracNib = mBusRead(MPL, 0x03, I2Cx);
    if(Altitude.hiByte > 0x7F){
      //printf("\rneg \t %d\n", Altitude.hiByte);
      Altitude.valWord = ~Altitude.hiByte + 1;
    }
    Altitude.valWord = (float)(Altitude.hiByte * 256  + Altitude.loByte);
    float fret = 0;
    if(Altitude.fracNib & 0b10000000){
      fret += 0.5;
    }
    if(Altitude.fracNib & 0b01000000){
      fret += 0.25;
    }
    if(Altitude.fracNib & 0b00100000){
      fret += 0.125;
    }
    if(Altitude.fracNib & 0b00010000){
      fret += 0.0625;
    }
    Altitude.valWord = Altitude.valWord + fret;
    printf("\rAltitude = %f\n", Altitude.valWord);
    Bias = Altitude.valWord + Bias;
    //printf("\rBias = %f\n", Bias);
    n = n + 1;
  }
  Bias = Bias / n;
  printf("\r%f\n", Bias);
  mWhiteOFF;
}
void GetAlt(uint8_t I2Cx){
  Altitude.hiByte = mBusRead(MPL, 0x01, I2Cx);
  Altitude.loByte = mBusRead(MPL, 0x02, I2Cx);
  Altitude.fracNib = mBusRead(MPL, 0x03, I2Cx);
  if(Altitude.hiByte > 0x7F){
    //printf("\rneg \t %d\n", Altitude.hiByte);
    Altitude.valWord = ~Altitude.hiByte + 1;
  }else{
    //printf("\rpos\t %d\n", Altitude.hiByte);
  }
  Altitude.valWord = (float)(Altitude.hiByte * 256  + Altitude.loByte);
  float fret = 0;
  if(Altitude.fracNib & 0b10000000){
    fret += 0.5;
  }
  if(Altitude.fracNib & 0b01000000){
    fret += 0.25;
  }
  if(Altitude.fracNib & 0b00100000){
    fret += 0.125;
  }
  if(Altitude.fracNib & 0b00010000){
    fret += 0.0625;
  }
  Altitude.valWord = Altitude.valWord + fret - Bias;
  //printf("\rBias = %f\n", Bias);
  //printf("\rAltitude = %f\n", Altitude.valWord);
}
void GetTemp(uint8_t I2Cx){
  ChkI2C(MPL, I2Cx);
  Temp.TByte = mBusRead(MPL, 0x04, I2Cx);
  Temp.fracTNib = mBusRead(MPL, 0x05, I2Cx);
  Temp.valTWord = (float)(Temp.TByte);
  float fret = 0;
  if(Temp.fracTNib & 0b10000000){
    fret += 0.5;
  }
  if(Temp.fracTNib & 0b01000000){
    fret += 0.25;
  }
  if(Temp.fracTNib & 0b00100000){
    fret += 0.125;
  }
  if(Temp.fracTNib & 0b00010000){
    fret += 0.0625;
  }
}
void GetDataBurst(uint8_t length, uint8_t I2Cx, uint8_t* data, uint8_t Regaddr){
 ChkI2C(MPL, I2Cx);
 mGreenTOGGLE;
 ret = mBusReadBurstStartNB(MPL, Regaddr, length, I2Cx);
  while(!ret){
    ret = mBusReadBurstStartNB(MPL, Regaddr, length, I2Cx);
    mRedON;
  } 
  mRedOFF;
 //_delay_ms(500);
  ret = mBusReadBurstDataNB(length, data, I2Cx);
  while(!ret){
    ret = mBusReadBurstDataNB(length, data, I2Cx);
    //mWaitms(100);
    mRedON;
  }
  Altitude.hiByte = data[0];
  Altitude.loByte = data[1];
  Altitude.fracNib = data[2];
  Temp.TByte = data[3];
  if(data[0] > 0x7F){
    data[0] = ~data[0] + 1;
  }
  Altitude.valWord = (float)(Altitude.hiByte * 256 + Altitude.loByte);
  float fret = 0;
  if(Altitude.fracNib & 0b10000000){
    fret += 0.5;
  }
  if(Altitude.fracNib & 0b01000000){
    fret += 0.25;
  }
  if(Altitude.fracNib & 0b00100000){
    fret += 0.125;
  }
  if(Altitude.fracNib & 0b00010000){
    fret += 0.0625;
  }
  Altitude.valWord = (Altitude.valWord + fret - Bias) / 0.254; 
  printf("\rAltitude = %f\n", Altitude.valWord);
}
void MPLINT(uint8_t mode, uint8_t* Intdata, uint8_t length){
  switch(mode){
    case 0:		/// Data ready interrupt
      mGreenON;
      ret = mBusWrite(MPL, 0x28, 0x11, 1);
      while(!ret){				/// Check if the writing was successful
	ret = mBusWrite(MPL, 0x28, 0x11, 1);
      }
      ret = mBusWrite(MPL, 0x29, 0x80, 1);
      while(!ret){				/// Check if the writing was successful
	ret = mBusWrite(MPL, 0x29, 0x80, 1);
      }
      ret = mBusWrite(MPL, 0x26, 0xB9, 1);
      while(!ret){				/// Check if the writing was successful
	ret = mBusWrite(MPL, 0x26, 0xB9, 1);
      }
      break;
    case 1:		/// Threshold for altitude
      //mYellowON;
      ret = mBusWrite(MPL, 0x28, 0x11, 1);
      while(!ret){				/// Check if the writing was successful
	ret = mBusWrite(MPL, 0x28, 0x11, 1);
      }
      ret = mBusWrite(MPL, 0x29, 0x08, 1);
      while(!ret){				/// Check if the writing was successful
	ret = mBusWrite(MPL, 0x29, 0x08, 1);
      }
      ret = mBusWriteBurstNB(MPL, 0x1A, length, Intdata, 1); /// length = 6
      ret = mBusWrite(MPL, 0x2A, 0x08, 1);
      while(!ret){				/// Check if the writing was successful
	ret = mBusWrite(MPL, 0x2A, 0x08, 1);
      }
      ret = mBusWrite(MPL, 0x26, 0xB9, 1);
      while(!ret){				/// Check if the writing was successful
	ret = mBusWrite(MPL, 0x26, 0xB9, 1);
      }
      break;
    case 2:	/// Window threshold
      mRedON;
      ret = mBusWrite(MPL, 0x28, 0x11, 1);
      while(!ret){				/// Check if the writing was successful
	ret = mBusWrite(MPL, 0x28, 0x11, 1);
      }
      ret = mBusWrite(MPL, 0x29, 0x08, 1);
      while(!ret){				/// Check if the writing was successful
	ret = mBusWrite(MPL, 0x29, 0x08, 1);
      }
      ret = mBusWriteBurstNB(MPL, 0x1A, length, Intdata, 1); /// length = 6
      ret = mBusWrite(MPL, 0x2A, 0x08, 1);
      while(!ret){				/// Check if the writing was successful
	ret = mBusWrite(MPL, 0x2A, 0x08, 1);
      }
      ret = mBusWrite(MPL, 0x26, 0xB9, 1);
      while(!ret){				/// Check if the writing was successful
	ret = mBusWrite(MPL, 0x26, 0xB9, 1);
      }
      break;
    case 3:	/// Window (outside)
      mWhiteON;
      ret = mBusWrite(MPL, 0x28, 0x11, 1);
      while(!ret){				/// Check if the writing was successful
	ret = mBusWrite(MPL, 0x28, 0x11, 1);
      }
      ret = mBusWrite(MPL, 0x29, 0x08, 1);
      while(!ret){				/// Check if the writing was successful
	ret = mBusWrite(MPL, 0x29, 0x08, 1);
      }
      mBusWriteBurstNB(MPL, 0x1A, length, Intdata, 1); /// length = 6
      ret = mBusWrite(MPL, 0x2A, 0x08, 1);
      while(!ret){				/// Check if the writing was successful
	ret = mBusWrite(MPL, 0x2A, 0x08, 1);
      }
      ret = mBusWrite(MPL, 0x26, 0xB9, 1);
      while(!ret){				/// Check if the writing was successful
	ret = mBusWrite(MPL, 0x26, 0xB9, 1);
      }
      break;
    default:	/// No interrupt
      mBusWrite(MPL, 0x26, 0xB9, 1);
      ChkI2C(MPL, 1);
      break;    
  }
}
void MPL311_init(void){
  /// Initialize code here
  /** Set PA.2 for input interrupt
   * That is easy Cedric
   **/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; // Set pin 2 as input to read interupts
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  /** Check if MPL is ready
   * This implies talking to the device through I2C 
   **/
   ret = mBusPeriphReady(MPL,1);
   while(!ret){				// Check if the MPL is ready
     ret = mBusPeriphReady(MPL,1);
     mRedON;
   }
  /** Control register 1
   * Set the Barometer as (B8)
   * 	* Altimeter mode 0
   *	* RAW 0
   * 	* OS[2:0] 111
   **/
   mRedOFF;
  ret = mBusWrite(MPL, 0x26, 0xB8, 1);
  while(!ret){				// Check if the writing was successful
    ret = mBusWrite(MPL, 0x26, 0xB8, 1);
    mRedON;
  }
  mRedOFF;
  /** Interupt register (PT_DATA_CFG)
   * Setup an interrupt
   * This is kind of complicated but not really, it is related to the control register #
   **/
  ret = mBusWrite(MPL, 0x13, 0x07, 1);
  while(!ret){
    ret = mBusWrite(MPL, 0x13, 0x07, 1);
    mRedON;
  }
  mRedOFF;
  /** Calibrate the altitude
   * 	* make sure you bias the output so that they give true values related to the altitude
   * 	* also make sure you take care of negative stuff
   **/
  
  /** Checklist
   * 1) Does the I2C work
   * 	* Yes
   * 2) Can you read and bias altitude measurements that make sense
   * 	* Yes, the data is noisy tho TODO get the frequency
   * 3) Can you send your data thru USB using Jimmy's
   * 	* Yes that is done
   * 4) Can you plot them on python
   * 	* Yes, but I must detail. Title, x label...
   * 5) Apply Fan
   * 	* yep, must save data
   * 6) Find out how fast is can you read data
   **/
  /// Setup to no interupts
  MPLINT(4, Intdata, 0);
  /// Calibrate the altitude 
  CalAlt(1);  
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
    /* Get ADC1 converted data */
    uint16_t ADC1ConvertedValue = ADC_input[2];
    
    /* Compute the voltage */
    float ADC1Voltage = (float)(ADC1ConvertedValue);
    
    float ADC1ConvertedVoltage = (ADC1Voltage - 1196) / 967;
    
    float Length = (123.4 / (ADC1ConvertedVoltage * ADC1ConvertedVoltage + 3.543 * ADC1ConvertedVoltage + 4.712)) * 2.54;
    
    float Voltage = 3.3 * ADC1Voltage /4096;
    
    Len_Vol[0] = Length;
    
    Len_Vol[1] = Voltage;
}