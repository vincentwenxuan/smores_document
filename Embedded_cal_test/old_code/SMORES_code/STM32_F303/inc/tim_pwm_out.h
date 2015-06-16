/*
Author Chaitanya Bhargava (cbhar@seas.upenn.edu)
Settings for TIMER PWM OUT usage on the STM32F303
mGeneral includes all the ST library files and the wait functions
For simple control of motors
*/

#ifndef TIM_PWM_OUT_H
#define	TIM_PWM_OUT_H

#include <stdint.h>

#ifdef	__cplusplus
extern "C" {
#endif

void TIMA_config(uint16_t presc_value, uint32_t period_val, uint32_t pulse_val);
void TIMB_config(uint16_t presc_value, uint32_t period_val, uint32_t pulse_val);
void TIMC_config(uint16_t presc_value, uint32_t period_val, uint32_t pulse_val);
void TIMD_config(uint16_t presc_value, uint32_t period_val, uint32_t pulse_val);
void motor_dir_init();
#ifdef	__cplusplus
}
#endif

#endif	/* TIM_PWM_OUT_H */