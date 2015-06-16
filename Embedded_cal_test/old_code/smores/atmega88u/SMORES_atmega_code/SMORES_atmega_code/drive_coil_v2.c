/**
  *****************************************************************************************
  * @file	 drive_coil_V2.c 
  * @author  Chaitanya Bhargava (cbhar)
  * @version V1.1.0
  * @date    10-September-2014
  * @brief   Different version of drive coil to avoid TIMER interrupt and simply toggle 
  *			 GPIOs to turn coils on and off
  *****************************************************************************************
*/
//*****************************************************************************************
//
//! \addtogroup SMORES_peripherals_apis
//! @{
//
//*****************************************************************************************
#include "drive_coil_v2.h"

uint32_t nos_pulses;
volatile unsigned int count1 = 0,count2=0;

/*-------------------User Functions-------------------------*/
//**************************************************************************************
//
//!  init_all_coils
//!
//!  @param    none
//!
//!  @return   none
//!
//!  @brief    This function will initialize all the pins for coil driving/communication
//!
//
//**************************************************************************************
void init_all_coils()
{
	 
	 set(DDRB, 5);
	 set(DDRC, 3);   // digital output control for coil 1
	 
	 set(DDRD, 7);
	 set(DDRD, 6);   // digital output control for coil 2
	 
	 set(DDRD, 5);
	 set(DDRD, 4);   // digital output control for coil 3

	 set(DDRB, 4);
	 set(DDRC, 2);   // digital output control for coil 4

	 set(DDRB, 2);
	 set(DDRB, 1);   // digital output control for coil common
	 
	 clear(PORTB, 5);
	 clear(PORTC, 3);    // shut down coil 1

	 clear(PORTD, 6);
	 clear(PORTD, 7);    // shut down coil 2

	 clear(PORTD, 5);
	 clear(PORTD, 4);    // shut down coil 3

	 clear(PORTB, 4);
	 clear(PORTC, 2);    // shut down coil 4
	 
	 clear(PORTB, 2);
	 set(PORTB, 1);    // shut down coils common //Set since connected to NOR
	 
	 set(DDRD, 1);		//Set USART TX pin to low 
	 clear(PORTD, 1);	//Since this and coil common low side (PB1) connected to NOR
}

//**************************************************************************************
//
//!  coils
//!		
//!  @param    coil_num			CO1-4 to select the coil
//!  @param    TURN_ON			turn current on through coil
//!  @param    TURN_OFF			turn current on opposite direction through coil
//!  @param    STOP1			turn everything off through coil	
//!  @param    USART_SENDER		set coil ready to send over usart
//!  @param    USART_LISTENER	set coil ready to listen for usart packets
//!
//!  @return   none
//!
//!  @brief    This function will control the behavior of coil 1. Extra unnecessary pins
//!			   set to avoid shoot-through
//!	 @NOTE	   The behavior of the common side is now handled separately
//!
//
//**************************************************************************************
void coil_setting(COIL_NUM coil_num, DIR direction)
{
	switch(coil_num)
	{
		case CO1:
			switch (direction)
			{
				case STOP_ALL:
				COIL1_HIGH_OFF;
				COIL1_LOW_OFF;
				break;
				case TURN_ON:
				COIL1_LOW_OFF;
				COIL1_HIGH_ON;
				break;
				case TURN_OFF:
				COIL1_HIGH_OFF;
				COIL1_LOW_ON;
				break;
				case USART_SENDER:
				COIL1_HIGH_ON;
				COIL1_LOW_OFF;
				break;
				case USART_LISTENER:
				COIL1_HIGH_OFF;
				COIL1_LOW_ON;	
				break;
			}
		break;
		
		case CO2:
			switch (direction)
			{
				case STOP_ALL:
				COIL2_HIGH_OFF;
				COIL2_LOW_OFF;
				break;
				case TURN_ON:
				COIL2_LOW_OFF;
				COIL2_HIGH_ON;
				break;
				case TURN_OFF:
				COIL2_HIGH_OFF;
				COIL2_LOW_ON;
				break;
				case USART_SENDER:
				COIL2_HIGH_ON;
				COIL2_LOW_OFF;
				break;
				case USART_LISTENER:
				COIL2_HIGH_OFF;
				COIL2_LOW_ON;
				break;
			}
		break;
		case CO3:
			switch (direction)
			{
				case STOP_ALL:
				COIL3_HIGH_OFF;
				COIL3_LOW_OFF;
				break;
				case TURN_ON:
				COIL3_LOW_OFF;
				COIL3_HIGH_ON;
				break;
				case TURN_OFF:
				COIL3_HIGH_OFF;
				COIL3_LOW_ON;
				break;
				case USART_SENDER:
				COIL3_HIGH_ON;
				COIL3_LOW_OFF;
				break;
				case USART_LISTENER:
				COIL3_HIGH_OFF;
				COIL3_LOW_ON;
				break;
			}
		break;
		case CO4:
			switch (direction)
			{
				case STOP_ALL:
				COIL4_HIGH_OFF;
				COIL4_LOW_OFF;
				break;
				case TURN_ON:
				COIL4_LOW_OFF;
				COIL4_HIGH_ON;
				break;
				case TURN_OFF:
				COIL4_HIGH_OFF;
				COIL4_LOW_ON;
				break;
				case USART_SENDER:
				COIL4_HIGH_ON;
				COIL4_LOW_OFF;
				break;
				case USART_LISTENER:
				COIL4_HIGH_OFF;
				COIL4_LOW_ON;
				break;
			}
		break;
	}
}

//**************************************************************************************
//
//!  set_comm_channels
//!
//!  @param    none
//!
//!  @return   none
//!
//!  @brief    Sets TIMER1 channels ready to start PWM based on user set duty cycle and
//!			   frequency from the common side of the coils
//!
//
//**************************************************************************************
void set_comm_channels()
{	 
	clear(TCCR1B, CS12);
	clear(TCCR1B, CS11);
	clear(TCCR1B, CS10);  // TIMER is OFF at start
	
	set(TCCR1B, WGM13);
	set(TCCR1B, WGM12);
	set(TCCR1A, WGM11);
	clear(TCCR1A, WGM10);	//Mode will be up to ICR1
	
	set(TIMSK1, OCIE1A);
	set(TIMSK1, OCIE1B);	//Enable ch A and B interrupts
	
	//initialize with random frequency
	ICR1 = (int)(8000000/5000) - 1;
	
	//Set mode and duty cycle for channel A
	clear(TCCR1A, COM1A1);
	clear(TCCR1A, COM1A0);
	OCR1A = (int)((float)ICR1 * 0.5);
	
	//Set mode and duty cycle for channel B
	clear(TCCR1A, COM1B1);
	clear(TCCR1A, COM1B0);
	OCR1B = (int)((float)ICR1 * 0.5);
	//Dummy number of pulses at initialization
	nos_pulses = 5;
}

//**************************************************************************************
//
//!  set_comm_frequency
//!
//!  @param    freq		The frequency to be set
//!
//!  @return   none
//!
//!  @brief    Sets TIMER1 channels Frequency
//!
//
//**************************************************************************************
void set_comm_freq(uint32_t freq)
{
	ICR1 = (uint16_t)(31250/freq)-1;
}

//**************************************************************************************
//
//!  set_comm_duty_cycle
//!
//!  @param    duty_cycle	The duty cycle to be set
//!
//!  @return   none
//!
//!  @brief    Sets TIMER1 channels duty cycle
//!
//
//**************************************************************************************
void set_comm_duty_cycle(float duty_cycle)
{
	OCR1A = (uint16_t)((float)ICR1 * duty_cycle);
	OCR1B = OCR1A;
}

//**************************************************************************************
//
//!  set_comm_nos_pulses
//!
//!  @param    num		Number of pulses to send
//!
//!  @return   none
//!
//!  @brief    Sets TIMER1 channels ready to start PWM based on user set duty cycle and
//!			   frequency
//!
//
//**************************************************************************************
void set_comm_nos_pulses(uint16_t num)
{
	nos_pulses = num;
}

//**************************************************************************************
//
//!  Carrier_Wave
//!
//!  @param    onoff	input weather to turn the carrier wave on or off
//!
//!  @return   none
//!
//!  @brief    Since the carrier wave is at 500KHz this function will first disable the 
//!			   TIMER interrupt before starting the wave. Interrupt is re-enabled once the 
//!			   wave is stopped.
//!
//
//**************************************************************************************
void Carrier_Wave(int state)
{							
	switch (state)			
	{
		case ON:

		clear(TIMSK1, OCIE1A);
		clear(TIMSK1, OCIE1B);	//Disable ch A and B interrupts
		ICR1 = 15;	//500KHz PWM. Why do math
		OCR1A = 8;
		//start timer1 with timer clock = 8Mhz
		set(TCCR1B,CS10);set(TCCR1A, COM1A1);set(TCCR1A,COM1A0);clear(PORTD,1);
		break;
		case OFF:
		clear(TCCR1B,CS10);clear(TCCR1A, COM1A1);clear(TCCR1A, COM1A0);set(PORTB,1);
		set(TIMSK1, OCIE1A);
		set(TIMSK1, OCIE1B);	//Enable ch A and B interrupts
		//COMM_LOW_OFF;
		ICR1 = 300;		//Random values
		OCR1A = 70;
		set(PORTB,1);			//Ensure NOR output is zero
		reset_pulses_counter();
		break;
	}
}

//**************************************************************************************
//
//!  reset_pulses_counter
//!
//!  @param    none
//!
//!  @return   none
//!
//!  @brief    resets the timer channels pulses counter. seems to be required every time 
//!			   after the high frequency pulse. (Maybe figure out the error later??)
//!
//
//**************************************************************************************
void reset_pulses_counter()
{
	count1=0;
	count2=0;
}
/*--------------------------------Private Functions-------------------------*/
//**************************************************************************************
//
//!  Timer 1 Compare Match A Interrupt
//!
//!  @param    none
//!
//!  @return   none
//!
//!  @brief    This function is used to call Timer 1 Compare Match A interrupt.
//
//**************************************************************************************
ISR (TIMER1_COMPA_vect)
{
	count1++;
	if(count1 >=nos_pulses)
	{
		count1 = 0;
		COMM_LOW_OFF;
	}
}
	
//**************************************************************************************
//
//!  Timer 1 Compare Match B Interrupt
//!
//!  @param    none
//!
//!  @return   none
//!
//!  @brief    This function is used to call Timer 1 Compare Match B interrupt.
//
//**************************************************************************************
ISR (TIMER1_COMPB_vect)
{
	count2++;
	if(count2 >=nos_pulses)
	{
		count2 = 0;
		COMM_HIGH_OFF;
	}
}

//***************************************************************************************
//
// Close the Doxygen group.
//! @}
//
//
//***************************************************************************************