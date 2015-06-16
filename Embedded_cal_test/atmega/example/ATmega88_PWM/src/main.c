/*
 * Name: main.c
 * Created: 8/22/2014
 * Author: Chao Liu (chao.liu0307@gmail.com)
 */

#include "m_general.h"
#include "PWM.h"

int main(void)
{
    /* insert your hardware initialization here */
    
	PWM_Init PWM1;
	PWM1.PWM_CHANNEL = OC1A;
	PWM1.frequency = 166;
	PWM1.duty_cycle = 0.5;
	PWM1.pulse_number = 3;

	pwm_channel_init(PWM1);


	PWM_Init PWM2;
	PWM2.PWM_CHANNEL = OC1B;
	PWM2.frequency = 200;
	PWM2.duty_cycle = 0.3;
	PWM2.pulse_number = 200;

	pwm_channel_init(PWM2);

	sei();
	
    for(;;){
        /* insert your main loop code here */
		//		pwm_set_pulse_number(PWM2.PWM_CHANNEL, 5);
		STOP_PWM_OC1A;
		START_PWM_OC1B;
		/* pwm_set_pulse_number(PWM2.PWM_CHANNEL, 200); */
		/* START_PWM_OC1B; */
		m_wait(3000);
		
		/* pwm_set_pulse_number(PWM1.PWM_CHANNEL, 10); */
		/* START_PWM_OC1B; */
		/* pwm_set_pulse_number(PWM2.PWM_CHANNEL, 100); */
		/* START_PWM_OC1B; */
		m_wait(3000);
		
        
        
    }
    return 0;   /* never reached */
}
