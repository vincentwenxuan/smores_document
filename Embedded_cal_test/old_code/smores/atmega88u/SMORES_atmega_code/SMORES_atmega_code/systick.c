/**
  *****************************************************************************************
  * @file   systick.c 
  * @author  Chaitanya Bhargava (cbhar)
  * @version V1.1.0
  * @date    11-September-2014
  * @brief   Enables TIM0 to keep track of the system time. 
  *****************************************************************************************
*/

//*****************************************************************************************
//
//! \addtogroup systick system apis
//! @{
//
//*****************************************************************************************
#include "systick.h"
volatile float system_time = 0;
//**************************************************************************************
//
//!  systick_init
//!
//!  @param    none
//!
//!  @return   none
//!
//!  @brief    Initializes TIM0 to 31250Hz,i.e. 1/31250 sec per tick and overflow at 0xFF
//!
//
//**************************************************************************************
void systick_init()
{		//Enable power to TIM0
		clear(PRR,PRTIM0);
		
		// system clock divide by 256
		set(TCCR0B,CS02);
		clear(TCCR0B,CS01);
		clear(TCCR0B,CS00);
		
		//Waveform generation: up to 0xFF
		clear(TCCR0B,WGM02);
		clear(TCCR0A,WGM01);
		clear(TCCR0A,WGM00);	
		
		//Enable TIM0 overflow interrupt
		set(TIMSK0,TOIE0);	
}

//**************************************************************************************
//
//!  systick_reset
//!
//!  @param    none
//!
//!  @return   none
//!
//!  @brief    sets the system time to 0
//!
//
//**************************************************************************************
void systick_reset()
{
	system_time = 0;
}

//**************************************************************************************
//
//!  systick_disable
//!
//!  @param    none
//!
//!  @return   none
//!
//!  @brief    disables power to the timer
//!
//
//**************************************************************************************
void systick_disable()
{
	set(PRR,PRTIM0);
}

//**************************************************************************************
//
//!  get_curr_time
//!
//!  @param    none
//!
//!  @return   system_time		The time since system startup
//!
//!  @brief    Returns the time passed since system startup.
//!
//
//**************************************************************************************
float get_curr_time()
{
	return system_time + TCNT0/31250;
}
/*-----------------------------private functions------------------------------*/

//**************************************************************************************
//
//!  ISR (interrupt handler)
//!
//!  @param    TIMER0_OVF_vect
//!
//!  @return   none
//!
//!  @brief    Interrupt handler. Called when TIM0 overflows i.e. 1/31250*255 seconds 
//!			   have passed.
//
//**************************************************************************************

ISR(TIMER0_OVF_vect)
{
	system_time = system_time+0.00816;
}