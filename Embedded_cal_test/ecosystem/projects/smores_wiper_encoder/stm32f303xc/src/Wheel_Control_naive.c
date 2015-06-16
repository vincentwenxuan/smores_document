#include "Wheel_Control_naive.h"

void init_motor(void);

Motor left_wheel_motor;
Motor right_wheel_motor;
Motor right_pan_tilt_motor;
Motor left_pan_tilt_motor;

void init_Dof(void)
{
    init_motor();
}

void left_control(uint8_t direction, uint8_t speed)
{
    if (direction) {
        set_motor_direction(left_wheel_motor, Bit_SET);
    } else {
        set_motor_direction(left_wheel_motor, Bit_RESET);
    }
    set_motor_speed(left_wheel_motor, speed);
}

void right_control(uint8_t direction, uint8_t speed)
{
    if (direction) {
        set_motor_direction(right_wheel_motor, Bit_RESET);
    } else {
        set_motor_direction(right_wheel_motor, Bit_SET);
    }
    set_motor_speed(right_wheel_motor, speed);
}

void tilt_control(uint8_t direction, uint8_t speed)
{
    if (direction)
    {
        set_motor_direction(left_pan_tilt_motor, Bit_SET);
        set_motor_direction(right_pan_tilt_motor, Bit_RESET);
    }
    else
    {
        set_motor_direction(left_pan_tilt_motor, Bit_RESET);
        set_motor_direction(right_pan_tilt_motor, Bit_SET);
    }
    set_motor_speed(left_pan_tilt_motor, speed);
    set_motor_speed(right_pan_tilt_motor, speed);
}

void pan_control(uint8_t direction, uint8_t speed)
{
    if (direction)
    {
        set_motor_direction(left_pan_tilt_motor, Bit_RESET);
        set_motor_direction(right_pan_tilt_motor, Bit_RESET);
    }
    else
    {
        set_motor_direction(left_pan_tilt_motor, Bit_SET);
        set_motor_direction(right_pan_tilt_motor, Bit_SET);
    }
    set_motor_speed(left_pan_tilt_motor, speed);
    set_motor_speed(right_pan_tilt_motor, speed);
}

void init_motor(void)
{
    right_pan_tilt_motor.GPIO_Direction_GPIOx = GPIOB;
    right_pan_tilt_motor.GPIO_Direction_Pin = GPIO_Pin_2;
    right_pan_tilt_motor.GPIO_Direction_RCC = RCC_AHBPeriph_GPIOB;
    right_pan_tilt_motor.GPIO_PWM_GPIOx = GPIOA;
    right_pan_tilt_motor.GPIO_PWM_Pin = GPIO_Pin_8;
    right_pan_tilt_motor.GPIO_PWM_PinSource = GPIO_PinSource8;
    right_pan_tilt_motor.GPIO_PWM_AF = GPIO_AF_6;
    right_pan_tilt_motor.GPIO_PWM_RCC = RCC_AHBPeriph_GPIOA;
    right_pan_tilt_motor.TIMx = TIM1;
    right_pan_tilt_motor.TIMx_Channel = TIM_Channel_1;
    right_pan_tilt_motor.TIMx_RCC = RCC_APB2Periph_TIM1;
    right_pan_tilt_motor.TIMx_Mode = TIM_OCMode_PWM1;
    right_pan_tilt_motor.direction = Bit_RESET;
    right_pan_tilt_motor.speed = 0;

    Motor_Init(right_pan_tilt_motor);

    right_wheel_motor.GPIO_Direction_GPIOx = GPIOA;
    right_wheel_motor.GPIO_Direction_Pin = GPIO_Pin_10;
    right_wheel_motor.GPIO_Direction_RCC = RCC_AHBPeriph_GPIOA;
    right_wheel_motor.GPIO_PWM_GPIOx = GPIOA;
    right_wheel_motor.GPIO_PWM_Pin = GPIO_Pin_9;
    right_wheel_motor.GPIO_PWM_PinSource = GPIO_PinSource9;
    right_wheel_motor.GPIO_PWM_AF = GPIO_AF_6;
    right_wheel_motor.GPIO_PWM_RCC = RCC_AHBPeriph_GPIOA;
    right_wheel_motor.TIMx = TIM1;
    right_wheel_motor.TIMx_Channel = TIM_Channel_2;
    right_wheel_motor.TIMx_RCC = RCC_APB2Periph_TIM1;
    right_wheel_motor.TIMx_Mode = TIM_OCMode_PWM1;
    right_wheel_motor.direction = Bit_RESET;
    right_wheel_motor.speed = 0;

    Motor_Init(right_wheel_motor);

    left_pan_tilt_motor.GPIO_Direction_GPIOx = GPIOB;
    left_pan_tilt_motor.GPIO_Direction_Pin = GPIO_Pin_15;
    left_pan_tilt_motor.GPIO_Direction_RCC = RCC_AHBPeriph_GPIOB;
    left_pan_tilt_motor.GPIO_PWM_GPIOx = GPIOB;
    left_pan_tilt_motor.GPIO_PWM_Pin = GPIO_Pin_14;
    left_pan_tilt_motor.GPIO_PWM_PinSource = GPIO_PinSource14;
    left_pan_tilt_motor.GPIO_PWM_AF = GPIO_AF_1;
    left_pan_tilt_motor.GPIO_PWM_RCC = RCC_AHBPeriph_GPIOB;
    left_pan_tilt_motor.TIMx = TIM15;
    left_pan_tilt_motor.TIMx_Channel = TIM_Channel_1;
    left_pan_tilt_motor.TIMx_RCC = RCC_APB2Periph_TIM15;
    left_pan_tilt_motor.TIMx_Mode = TIM_OCMode_PWM1;
    left_pan_tilt_motor.direction = Bit_RESET;
    left_pan_tilt_motor.speed = 0;

    Motor_Init(left_pan_tilt_motor);

    left_wheel_motor.GPIO_Direction_GPIOx = GPIOA;
    left_wheel_motor.GPIO_Direction_Pin = GPIO_Pin_0;
    left_wheel_motor.GPIO_Direction_RCC = RCC_AHBPeriph_GPIOA;
    left_wheel_motor.GPIO_PWM_GPIOx = GPIOA;
    left_wheel_motor.GPIO_PWM_Pin = GPIO_Pin_1;
    left_wheel_motor.GPIO_PWM_PinSource = GPIO_PinSource1;
    left_wheel_motor.GPIO_PWM_AF = GPIO_AF_1;
    left_wheel_motor.GPIO_PWM_RCC = RCC_AHBPeriph_GPIOA;
    left_wheel_motor.TIMx = TIM2;
    left_wheel_motor.TIMx_Channel = TIM_Channel_2;
    left_wheel_motor.TIMx_RCC = RCC_APB1Periph_TIM2;
    left_wheel_motor.TIMx_Mode = TIM_OCMode_PWM1;
    left_wheel_motor.direction = Bit_RESET;
    left_wheel_motor.speed = 0;

    Motor_Init(left_wheel_motor);
}
