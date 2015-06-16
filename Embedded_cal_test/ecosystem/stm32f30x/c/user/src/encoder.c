#include "common_peripherals.h"
#include "encoder.h"
#include STM32FXXX_GPIO_H
#include STM32FXXX_MISC_H
#include STM32FXXX_RCC_H
#include STM32FXXX_TIM_H
#include STM32FXXX_SYSCFG_H
#include STM32FXXX_EXTI_H
#include "system_clock.h"
#include "arm_math.h"

void initEncoder(){
  #if defined(ENCODER_QEP_TIMER) || defined(ENCODER_PWM_TIMER)
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  #endif
  
  #ifdef ENCODER_QEP_TIMER
  // QEP Timer Enable
  #if ENCODER_QEP_TIMER == 19
    RCC_APB2PeriphClockCmd(MAKENAME(RCC_APB2Periph_TIM,ENCODER_QEP_TIMER), ENABLE);
  #else
    RCC_APB1PeriphClockCmd(MAKENAME(RCC_APB1Periph_TIM,ENCODER_QEP_TIMER), ENABLE);
  #endif
   
  /* QEPA GPIO clock enable */
  RCC_AHBPeriphClockCmd(MAKENAME(RCC_AHBPeriph_GPIO,ENCODER_QEPA_PORT), ENABLE);
  
  /* QEPA GPIO config */
  GPIO_InitStructure.GPIO_Pin   = MAKENAME(GPIO_Pin_,ENCODER_QEPA_PIN);
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(MAKENAME(GPIO,ENCODER_QEPA_PORT), &GPIO_InitStructure);
  
  /* Connect TIM pin QEPA to AF */
  GPIO_PinAFConfig(MAKENAME(GPIO,ENCODER_QEPA_PORT), MAKENAME(GPIO_PinSource,ENCODER_QEPA_PIN), MAKENAME(GPIO_AF_,ENCODER_QEPA_AF));
  
  /* QEPB GPIO clock enable */
  RCC_AHBPeriphClockCmd(MAKENAME(RCC_AHBPeriph_GPIO,ENCODER_QEPB_PORT), ENABLE);
  
  /* QEPB GPIO config */
  GPIO_InitStructure.GPIO_Pin   = MAKENAME(GPIO_Pin_,ENCODER_QEPB_PIN);
  GPIO_Init(MAKENAME(GPIO,ENCODER_QEPB_PORT), &GPIO_InitStructure);
  
  /* Connect TIM pin QEPB to AF */
  GPIO_PinAFConfig(MAKENAME(GPIO,ENCODER_QEPB_PORT), MAKENAME(GPIO_PinSource,ENCODER_QEPB_PIN), MAKENAME(GPIO_AF_,ENCODER_QEPB_AF));
  
  // QEP Timer Setup
  //TODO:: Do I need to do TIM_ICInitStructure here?
  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = ENCODER_COUNTS;
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(MAKENAME(TIM,ENCODER_QEP_TIMER), &TIM_TimeBaseStructure);
  
  TIM_EncoderInterfaceConfig(MAKENAME(TIM,ENCODER_QEP_TIMER),TIM_EncoderMode_TI12,TIM_ICPolarity_Rising,TIM_ICPolarity_Rising);
  
  /* QEP timer enable counter */
  TIM_Cmd(MAKENAME(TIM,ENCODER_QEP_TIMER), ENABLE);

  #ifdef ENCODER_INDEX_PORT
  
  /* INDEX GPIO clock enable */
  RCC_AHBPeriphClockCmd(MAKENAME(RCC_AHBPeriph_GPIO,ENCODER_INDEX_PORT), ENABLE);
  
  /* INDEX GPIO config */
  GPIO_InitStructure.GPIO_Pin   = MAKENAME(GPIO_Pin_,ENCODER_INDEX_PIN);
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(MAKENAME(GPIO,ENCODER_INDEX_PORT), &GPIO_InitStructure);
  
  /* Enable SYSCFG clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  // Select the input source pin for the EXTI line using SYSCFG_EXTILineConfig().
  SYSCFG_EXTILineConfig(MAKENAME(EXTI_PortSourceGPIO,ENCODER_INDEX_PORT),MAKENAME(EXTI_PinSource,ENCODER_INDEX_PIN));
  // Select the mode(interrupt, event) and configure the trigger selection (Rising, falling or both) using EXTI_Init(). 
  EXTI_InitTypeDef   EXTI_InitStructure;
  /* Configure EXTI0 line */
  EXTI_InitStructure.EXTI_Line = MAKENAME(EXTI_Line,ENCODER_INDEX_PIN);
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  // Configure NVIC IRQ channel mapped to the EXTI line using NVIC_Init().
  NVIC_InitTypeDef   NVIC_InitStructure;
  /* Enable and set EXTI0 Interrupt to the highest priority */
  NVIC_InitStructure.NVIC_IRQChannel = ENCODER_INTERRUPT;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  // Optionally, you can generate a software interrupt using the function EXTI_GenerateSWInterrupt().
  
  #endif //ENCODER_INDEX_PORT
  
  #endif //ENCODER_QEP_TIMER
  
  #ifdef ENCODER_PWM_TIMER
  
  // PWM Timer Enable
  #if ENCODER_PWM_TIMER == 15 || ENCODER_PWM_TIMER == 19
    RCC_APB2PeriphClockCmd(MAKENAME(RCC_APB2Periph_TIM,ENCODER_PWM_TIMER), ENABLE);
  #else
    RCC_APB1PeriphClockCmd(MAKENAME(RCC_APB1Periph_TIM,ENCODER_PWM_TIMER), ENABLE);
  #endif
  
  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 65535;
  TIM_TimeBaseStructure.TIM_Prescaler = 5;
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(MAKENAME(TIM,ENCODER_PWM_TIMER), &TIM_TimeBaseStructure);
  
  /* PWM GPIO clock enable */
  RCC_AHBPeriphClockCmd(MAKENAME(RCC_AHBPeriph_GPIO,ENCODER_PWM_PORT), ENABLE);
  
  /* PWM GPIO config */
  GPIO_InitStructure.GPIO_Pin   = MAKENAME(GPIO_Pin_,ENCODER_PWM_PIN);
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(MAKENAME(GPIO,ENCODER_PWM_PORT), &GPIO_InitStructure);
  
  /* Connect TIM pin PWM to AF */
  GPIO_PinAFConfig(MAKENAME(GPIO,ENCODER_PWM_PORT), MAKENAME(GPIO_PinSource,ENCODER_PWM_PIN), MAKENAME(GPIO_AF_,ENCODER_PWM_AF));
  
  // PWM Timer Config
  TIM_ICInitTypeDef  TIM_ICInitStructure;
  
  TIM_ICInitStructure.TIM_Channel = MAKENAME(TIM_Channel_,ENCODER_PWM_CHANNEL);
  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
  TIM_ICInitStructure.TIM_ICFilter = 0x0;

  TIM_PWMIConfig(MAKENAME(TIM,ENCODER_PWM_TIMER), &TIM_ICInitStructure);
  
  /* Select the PWM TIM Input Trigger */
  TIM_SelectInputTrigger(MAKENAME(TIM,ENCODER_PWM_TIMER), MAKENAME(MAKENAME(TIM_TS_TI,ENCODER_PWM_CHANNEL),MAKENAME(FP,ENCODER_PWM_CHANNEL)));

  /* Select the slave Mode: Reset Mode */
  TIM_SelectSlaveMode(MAKENAME(TIM,ENCODER_PWM_TIMER), TIM_SlaveMode_Reset);
  TIM_SelectMasterSlaveMode(MAKENAME(TIM,ENCODER_PWM_TIMER),TIM_MasterSlaveMode_Enable);

  /* TIM enable counter */
  //TIM_Cmd(MAKENAME(TIM,ENCODER_PWM_TIMER), ENABLE);
  
  #endif //ENCODER_PWM_TIMER
}

#ifdef ENCODER_INTERRUPT_FCN
void ENCODER_INTERRUPT_FCN(void)
{
  if(EXTI_GetITStatus(MAKENAME(EXTI_Line,ENCODER_INDEX_PIN)) != RESET)
  {
    // Reset timer
    MAKENAME(TIM,ENCODER_QEP_TIMER)->CNT = 0;

    /* Clear the EXTI line 0 pending bit */
    EXTI_ClearITPendingBit(MAKENAME(EXTI_Line,ENCODER_INDEX_PIN));
  }
}
#endif

#ifdef ENCODER_PWM_TIMER
void enable_PWM_timer(){
	//MAKENAME(TIM,ENCODER_PWM_TIMER)->CCER |= 0x11;
	//MAKENAME(TIM,ENCODER_PWM_TIMER)->CR1 |= 0x01;
  /* TIM enable counter */
  TIM_Cmd(MAKENAME(TIM,ENCODER_PWM_TIMER), ENABLE);
}

void disable_PWM_timer(){
  /* TIM enable counter */
  TIM_Cmd(MAKENAME(TIM,ENCODER_PWM_TIMER), DISABLE);
	//MAKENAME(TIM,ENCODER_PWM_TIMER)->CCER = 0;
	//MAKENAME(TIM,ENCODER_PWM_TIMER)->CR1 = 0;
	//MAKENAME(TIM,ENCODER_PWM_TIMER)->CCMR1 = 0;
	//MAKENAME(TIM,ENCODER_PWM_TIMER)->CCER = 0;
	//MAKENAME(TIM,ENCODER_PWM_TIMER)->SMCR = 0;
}

uint32_t get_initial_position(){
	uint32_t duty, period;
	enable_PWM_timer();
  	DelaySeconds(0.010);
	duty = MAKENAME(TIM,ENCODER_PWM_TIMER)->CCR1;	
	period = MAKENAME(TIM,ENCODER_PWM_TIMER)->CCR2;	
	while(1){
		duty = MAKENAME(TIM,ENCODER_PWM_TIMER)->CCR1;
		period = MAKENAME(TIM,ENCODER_PWM_TIMER)->CCR2;
		if(period > duty){
			disable_PWM_timer();
			return ENCODER_COUNTS*duty/period;
		}
	}
}
#endif

#ifdef ENCODER_QEP_TIMER
void enable_QEP_timer(){
	MAKENAME(TIM,ENCODER_QEP_TIMER)->CR1 |= 0x0081;
}

void disable_QEP_timer(){
	MAKENAME(TIM,ENCODER_QEP_TIMER)->CR1 = 0;
	MAKENAME(TIM,ENCODER_QEP_TIMER)->SMCR = 0; 
	MAKENAME(TIM,ENCODER_QEP_TIMER)->CCMR1 = 0;
	MAKENAME(TIM,ENCODER_QEP_TIMER)->CCMR2 = 0;	
	MAKENAME(TIM,ENCODER_QEP_TIMER)->CCER = 0;
	MAKENAME(TIM,ENCODER_QEP_TIMER)->ARR = 0;
	MAKENAME(TIM,ENCODER_QEP_TIMER)->CNT = 0;
}

uint32_t encoderReadTicks(){
	return MAKENAME(TIM,ENCODER_QEP_TIMER)->CNT;
}

float encoderReadRad() {
  int32_t ticks = MAKENAME(TIM,ENCODER_QEP_TIMER)->CNT;
  float angle = ticks * ENCODER_REV_PER_TICK * 2.0f * PI;
  #ifdef ENCODER_REVERSE_DIRECTION
  angle = 2.0f*PI - angle;
  #endif
  return angle;
}

void start_encoder(uint32_t position_offset){
	uint32_t initial_position;
	initial_position = position_offset%ENCODER_COUNTS;
	MAKENAME(TIM,ENCODER_QEP_TIMER)->CNT = initial_position;
	enable_QEP_timer();
}

int32_t get_QEP_direction(void) // Returns the direction of the encoder
{
  if (MAKENAME(TIM,ENCODER_QEP_TIMER)->CR1 & TIM_CR1_DIR)
    return -1; //Downcounter
  return 1; //Upcounter
}

float compute_speed_fr(float frequency, float motor_speed_smoothing_factor)
{
  static int32_t old_encoder_position = 0.0f; // only happens first time
  static float current_motor_speed = 0.0f; // only happens first time
  int32_t encoder_position = encoderReadTicks();

  // calculate encoder_position_diff with wrapping protection
  int32_t encoder_position_diff;
  int32_t tick_direction = get_QEP_direction(); // 1 for counting up, -1 for counting down
  // If encoder is counting down
  if (tick_direction == -1) {
    if (encoder_position > old_encoder_position) // Position wrap
      encoder_position_diff = encoder_position - ENCODER_COUNTS - old_encoder_position;
    else
      encoder_position_diff = encoder_position - old_encoder_position;
  }
  // Else if encoder is counting up
  else if (tick_direction == 1) {
    if (encoder_position < old_encoder_position)
      encoder_position_diff = ENCODER_COUNTS + encoder_position - old_encoder_position;
    else
      encoder_position_diff = encoder_position - old_encoder_position;
  }
  // Else if encoder has not been setup
  else {
    mRedON; // Error, you didn't implement the direction function.
  }
  
  // calculate speed in radians/second
  float temp_motor_speed = ((float)encoder_position_diff)*frequency*2.0f*PI*ENCODER_REV_PER_TICK; // speed in rad/s
  #ifdef ENCODER_REVERSE_DIRECTION
  temp_motor_speed = -temp_motor_speed;
  #endif

  // If the speed has changed by less than 1/2 of the speed sensing range in one calculation time
  float lb = current_motor_speed - ENCODER_COUNTS*(PI*frequency);
  float ub = current_motor_speed + ENCODER_COUNTS*(PI*frequency);
  if (lb < temp_motor_speed  &&  temp_motor_speed < ub) {
    current_motor_speed = temp_motor_speed*motor_speed_smoothing_factor + current_motor_speed*(1.0f-motor_speed_smoothing_factor);
  }
  
  old_encoder_position = encoder_position;
  return current_motor_speed;
}
#endif
