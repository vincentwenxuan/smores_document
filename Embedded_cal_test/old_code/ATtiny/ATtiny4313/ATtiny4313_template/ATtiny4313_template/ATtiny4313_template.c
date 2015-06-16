/*
 * ATtiny4313_template.c
 *
 * Created: 7/4/2014 10:37:17 AM
 *  Author: wuliuxiansheng
 */ 


# define F_CPU 8000000UL	// define the clock to be 8 MHz in order to use m_wait accurately
#include "m_general.h"

int main(void)
{
	m_clockdivide(0);

	while(1)
	{
		//TODO:: Please write your application code
		m_green(TOGGLE);
		m_yellow(TOGGLE);
		m_white(TOGGLE);
		m_blue(TOGGLE);
		m_wait(1000);
		
	}
}