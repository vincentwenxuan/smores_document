/*
 * ATtiny1634_template.c
 *
 * Created: 7/24/2014 4:59:34 PM
 *  Author: Chao Liu(chao.liu0307@gmail.com)
 */ 

# define F_CPU 8000000UL
#include "m_general.h"

int main(void)
{
	m_clockdivide(0);
    while(1)
    {
        //TODO:: Please write your application code 
		m_white(TOGGLE);
		m_yellow(TOGGLE);
		m_blue(TOGGLE);
		m_wait(1000);
    }
}