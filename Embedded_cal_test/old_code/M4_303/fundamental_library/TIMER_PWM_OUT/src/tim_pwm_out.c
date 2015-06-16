/**
  *****************************************************************************************
  * @file    tim_pwm_out.c 
  * @author  Chaitanya Bhargava (cbhar)
  * @version V1.1.0
  * @date    06-June-2014
  * @brief   This file has all the fuctions to configure the timers to controls the 4 motors
  *          on SMORES. Since the motors and their directions may be getting filpped while 
  *          boards are conected from one to another, specific motor control functions have 
  *			 been made yet
  *****************************************************************************************
*/

//*****************************************************************************
//
//! \addtogroup SMORES_motor_api
//! @{
//
//*****************************************************************************


#include "mGeneral.h"
#include "tim_pwm_out.h"
#include "tim_pwm_outplatform_config.h"
//**************************************************************************************
//
//!  TIMA_config
//!
//!  @param    presc_val      The timer clock becomes, system_clock/(presc_val+1)
//!  @param    period_value   The value to which the timer counts
//!  @param    pulse_val      An event(in our case PWM) can occur at pulse value+1
//!            
//!            
//!  @return   none
//!
//!  @brief    Starts the timer A based on settings in tim_pwm_outplatform_config.h
//!            with the entered parameters. If at initialization pulse_val is not 0 
//!            the motor will be drawing current
//!        
//        
//**************************************************************************************
void TIMA_config(uint16_t presc_value, uint32_t period_val, uint32_t pulse_val)
{
	//Configure the GPIO piins for TIMER
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = TIMA_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(TIMA_PORT, &GPIO_InitStructure); 
	
	//Configure the pins to TIMER Alternate Function
	GPIO_PinAFConfig(TIMA_PORT,TIMA_PIN_SOURCE, TIMA_AF);
	//Enable the TIMER clock
	RCC_APB2PeriphClockCmd(TIMA_RCC,ENABLE);
	
	//TIMER base Initialization
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	
	//TIMER base configuration
	TIM_TimeBaseStructure.TIM_Prescaler =presc_value;//PrescalerValue;
	TIM_TimeBaseStructure.TIM_Period = period_val;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	
	TIM_TimeBaseInit(TIMA, &TIM_TimeBaseStructure);
	
	//Set the TIMER to Output Compare Mode
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure); 
	
	/* Output Compare Active Mode configuration: Channel1 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = pulse_val;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //Not really required. can always change CCRx value or mode to PWM2
	TIM_OC1Init(TIMA, &TIM_OCInitStructure);

	TIM_OC1PreloadConfig(TIMA, TIM_OCPreload_Enable); //Required for PWM mode
	TIM_GenerateEvent(TIMA, TIM_EventSource_Update);
	TIM_ARRPreloadConfig(TIMA, DISABLE); 
	TIM_CtrlPWMOutputs(TIMA, ENABLE);
	//TIMER Counter Enable
	TIM_Cmd(TIMA,ENABLE);
	
}
//**************************************************************************************
//
//!  TIMB_config
//!
//!  @param    presc_val      The timer clock becomes, system_clock/(presc_val+1)
//!  @param    period_value   The value to which the timer counts
//!  @param    pulse_val      An event(in our case PWM) can occur at pulse value+1
//!            
//!            
//!  @return   none
//!
//!  @brief    Starts the timer B based on settings in tim_pwm_outplatform_config.h
//!            with the entered parameters. If at initialization pulse_val is not 0 
//!            the motor will be drawing current
//!        
//        
//**************************************************************************************
void TIMB_config(uint16_t presc_value, uint32_t period_val, uint32_t pulse_val)
{
	//Configure the GPIO pins for TIMER
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = TIMB_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(TIMB_PORT, &GPIO_InitStructure); 
	
	//Configure the pins to TIMER Alternate Function
	GPIO_PinAFConfig(TIMB_PORT,TIMB_PIN_SOURCE, TIMB_AF);
	//Enable the TIMER clock
	RCC_APB1PeriphClockCmd(TIMB_RCC,ENABLE);
	
	//TIMER base Initialization
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	
	//TIMER base configuration
	TIM_TimeBaseStructure.TIM_Prescaler =presc_value;//PrescalerValue;
	TIM_TimeBaseStructure.TIM_Period = period_val;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	
	TIM_TimeBaseInit(TIMB, &TIM_TimeBaseStructure);
	
	//Set the TIMER to Output Compare Mode
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure); 
	
	/* Output Compare Active Mode configuration: Channel1 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = pulse_val;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //Not really required. can always change CCRx value or mode to PWM2
	TIM_OC3Init(TIMB, &TIM_OCInitStructure);
	
	//TIM_ARRPreloadConfig(TIMB, DISABLE); 
	TIM_OC3PreloadConfig(TIMB, TIM_OCPreload_Enable); //Required for PWM mode
	TIM_GenerateEvent(TIMB, TIM_EventSource_Update);
	//TIMER Counter Enable
	TIM_Cmd(TIMB,ENABLE);
}
//**************************************************************************************
//
//!  TIMC_config
//!
//!  @param    presc_val      The timer clock becomes, system_clock/(presc_val+1)
//!  @param    period_value   The value to which the timer counts
//!  @param    pulse_val      An event(in our case PWM) can occur at pulse value+1
//!            
//!            
//!  @return   none
//!
//!  @brief    Starts the timer C based on settings in tim_pwm_outplatform_config.h
//!            with the entered parameters. If at initialization pulse_val is not 0 
//!            the motor will be drawing current
//!        
//        
//**************************************************************************************
void TIMC_config(uint16_t presc_value, uint32_t period_val, uint32_t pulse_val)
{
	//Configure the GPIO pins for TIMER
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = TIMC_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(TIMC_PORT, &GPIO_InitStructure); 
	
	//Configure the pins to TIMER Alternate Function
	GPIO_PinAFConfig(TIMC_PORT,TIMC_PIN_SOURCE, TIMC_AF);
	//Enable the TIMER clock
	RCC_APB2PeriphClockCmd(TIMC_RCC,ENABLE);
	
	//TIMER base Initialization
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	
	//TIMER base configuration
	TIM_TimeBaseStructure.TIM_Prescaler =presc_value;//PrescalerValue;
	TIM_TimeBaseStructure.TIM_Period = period_val;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	
	TIM_TimeBaseInit(TIMC, &TIM_TimeBaseStructure);
	
	//Set the TIMER to Output Compare Mode
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure); 
	
	/* Output Compare Active Mode configuration: Channel1 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = pulse_val;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //Not really required. can always change CCRx value or mode to PWM2
	TIM_OC1Init(TIMC, &TIM_OCInitStructure);

	TIM_OC1PreloadConfig(TIMC, TIM_OCPreload_Enable); //Required for PWM mode
	TIM_GenerateEvent(TIMC, TIM_EventSource_Update);
	TIM_ARRPreloadConfig(TIMC, DISABLE); 
	TIM_CtrlPWMOutputs(TIMC, ENABLE);
	//TIMER Counter Enable
	TIM_Cmd(TIMC,ENABLE);
}
//**************************************************************************************
//
//!  TIMD_config
//!
//!  @param    presc_val      The timer clock becomes, system_clock/(presc_val+1)
//!  @param    period_value   The value to which the timer counts
//!  @param    pulse_val      An event(in our case PWM) can occur at pulse value+1
//!            
//!            
//!  @return   none
//!
//!  @brief    Starts the timer D based on settings in tim_pwm_outplatform_config.h
//!            with the entered parameters. If at initialization pulse_val is not 0 
//!            the motor will be drawing current.
//!
//!  @NOTE     For SMORES, TIMA and TIMD are the same. Hence entering different parameters
//!        	   for TIMD will not reinitialise it since it has already been set up in 
//!   		   TIMA_config
//        
//**************************************************************************************
void TIMD_config(uint16_t presc_value, uint32_t period_val, uint32_t pulse_val)
{
	//Configure the GPIO pins for TIMER
	GPIO_InitTypeDef GPIO_InitStructure;

	// RCC_HCLKConfig(RCC_SYSCLK_Div1);	//Similar to m_clocKdivide
	GPIO_InitStructure.GPIO_Pin = TIMD_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(TIMD_PORT, &GPIO_InitStructure); 
	
	//Configure the pins to TIMER Alternate Function
	GPIO_PinAFConfig(TIMD_PORT,TIMD_PIN_SOURCE, TIMD_AF);
	/* USING THE SAME TIMER AS TIMB. CANNOT REINITIALISE.
	PRESCALER AND PERIOD WILL HAVE T0 BE THE SAME*/

	//Enable the TIMER clock
	// RCC_APB2PeriphClockCmd(TIMD_RCC,ENABLE);
	
	// //TIMER D base Initialization
	// TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	// TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	
	// //TIMER base configuration
	// TIM_TimeBaseStructure.TIM_Prescaler =presc_value;//PrescalerValue;
	// TIM_TimeBaseStructure.TIM_Period = period_val;
	// TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	// TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	
	// TIM_TimeBaseInit(TIMD, &TIM_TimeBaseStructure);
	
//Set the TIMER to Output Compare Mode
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure); 
	
	/* Output Compare Active Mode configuration: Channel1 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = pulse_val;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //Not really required. can always change CCRx value or mode to PWM2
	TIM_OC2Init(TIMD, &TIM_OCInitStructure);
	
	//TIM_ARRPreloadConfig(TIMD, DISABLE); 
	TIM_OC2PreloadConfig(TIMD, TIM_OCPreload_Enable); //Required for PWM mode
	TIM_GenerateEvent(TIMD, TIM_EventSource_Update);
	//TIMER Counter Enable
	TIM_Cmd(TIMD,ENABLE);
}
//**************************************************************************************
//
//!  motor_dir_init
//!
//!  @param    none
//!            
//!  @return   none
//!
//!  @brief    Configures motors direction pins to output. For now SMORES motors
//        
//**************************************************************************************
void motor_dir_init()
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = MA_DIR_PIN; //MA DIRECTION
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(MA_DIR_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = MB_DIR_PIN; //MB DIRECTION
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(MB_DIR_PORT, &GPIO_InitStructure);

  	GPIO_InitStructure.GPIO_Pin = MC_DIR_PIN; //MC DIRECTION
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  	GPIO_Init(MC_DIR_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = MD_DIR_PIN; //MD DIRECTION
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  	GPIO_Init(MD_DIR_PORT, &GPIO_InitStructure);

}
//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//
//*****************************************************************************