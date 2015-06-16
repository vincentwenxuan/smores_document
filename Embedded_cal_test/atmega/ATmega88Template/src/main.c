/* Name: main.c
 * Author: <insert your name here>
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

#include "m_general.h"

int main(void)
{
	set(DDRB, 1);
	/* insert your hardware initialization here */
	for(;;){
		/* insert your main loop code here */
		toggle(PORTB, 1);
		m_green(TOGGLE);
		m_blue(ON);
		m_yellow(TOGGLE);
		m_wait(1000);
	}
	return 0;   /* never reached */
}
