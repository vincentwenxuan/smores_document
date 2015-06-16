#ifndef ENCODER_H
#define ENCODER_H

#define MAX_ENCODER_VALUE 4096
#include "common_peripherals.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void initEncoder(void);//Initial configuration for timers and GPIOs. Timer2 and pin PA0 and PA1 for quadrature reading, timer1 and PA8 for PWM reading. 

#ifdef ENCODER_PWM_TIMER
void enable_PWM_timer(void);
void disable_PWM_timer(void);//Note it will also clear the configurations of timer
uint32_t get_initial_position(void);
#endif

#ifdef ENCODER_QEP_TIMER
void start_encoder(uint32_t position_offset);
uint32_t encoderReadTicks(void);    // return current position (ticks) [0, MAX_ENCODER_VALUE)
float encoderReadRad(void);         // return current position (rad) [0, 2*PI]
void enable_QEP_timer(void);
void disable_QEP_timer(void);       // Note it will also clear the configurations of timer
int32_t get_QEP_direction(void);    // Returns the direction of the encoder
float compute_speed_fr(float frequency, float motor_speed_smoothing_factor); // Returns the speed in rad/s given the frequency that the function is called and the smoothing factor    // USED IN BRUSHLESS_CONTROLLER
#endif

#ifdef ENCODER_INTERRUPT_FCN
//void ENCODER_INTERRUPT_FCN(void);
void EXTI1_IRQHandler(void);
#endif

#ifdef __cplusplus
}
#endif // __cplusplus

#endif
