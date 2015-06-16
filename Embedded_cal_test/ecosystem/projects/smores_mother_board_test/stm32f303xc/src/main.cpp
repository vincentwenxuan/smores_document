#include "common_peripherals.h"
#include "system_clock.h"
// common components 
#include "common_message_types.h" 
// common messages
#include "usb_interface.h"        
// usb interface
#include "mBus.h"

// common libraries
#include "project_message_types.h"
#include "error_reporting.h"
#include <string.h> 
//needed for memset

#include "DC_Motor.h"

//#define USB_TEST
//#define LED_TEST
#define CONNECTOR_TEST

int main (void){
    InitPeripherals();
#ifdef USB_TEST
    UsbInterface usb = UsbInterface();
    usb.Init();
#endif

    
#ifdef LED_TEST    
    mYellowON; mRedON; mGreenON; mWhiteON; mBlueON;
    DelayMilliseconds(1000);
    mYellowOFF;mRedOFF;mGreenOFF; mWhiteOFF; mBlueOFF;
#endif

#ifdef CONNECTOR_TEST

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_2;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOB, &GPIO_InitStruct);
    
    Motor motor_1;
    motor_1.GPIO_Direction_GPIOx = GPIOB;
    motor_1.GPIO_Direction_Pin = GPIO_Pin_2;
    motor_1.GPIO_Direction_RCC = RCC_AHBPeriph_GPIOB;
    motor_1.GPIO_PWM_GPIOx = GPIOA;
    motor_1.GPIO_PWM_Pin = GPIO_Pin_8;
    motor_1.GPIO_PWM_PinSource = GPIO_PinSource8;
    motor_1.GPIO_PWM_AF = GPIO_AF_6;
    motor_1.GPIO_PWM_RCC = RCC_AHBPeriph_GPIOA;
    motor_1.TIMx = TIM1;
    motor_1.TIMx_Channel = TIM_Channel_1;
    motor_1.TIMx_RCC = RCC_APB2Periph_TIM1;
    motor_1.TIMx_Mode = TIM_OCMode_PWM1;
    motor_1.direction = Bit_RESET;
    motor_1.speed = 0;
  
    Motor_Init(motor_1);

    Motor motor_2;
    motor_2.GPIO_Direction_GPIOx = GPIOA;
    motor_2.GPIO_Direction_Pin = GPIO_Pin_10;
    motor_2.GPIO_Direction_RCC = RCC_AHBPeriph_GPIOA;
    motor_2.GPIO_PWM_GPIOx = GPIOB;
    motor_2.GPIO_PWM_Pin = GPIO_Pin_5;
    motor_2.GPIO_PWM_PinSource = GPIO_PinSource5;
    motor_2.GPIO_PWM_AF = GPIO_AF_10;
    motor_2.GPIO_PWM_RCC = RCC_AHBPeriph_GPIOB;
    motor_2.TIMx = TIM17;
    motor_2.TIMx_Channel = TIM_Channel_1;
    motor_2.TIMx_RCC = RCC_APB2Periph_TIM17;
    motor_2.TIMx_Mode = TIM_OCMode_PWM1;
    motor_2.direction = Bit_RESET;
    motor_2.speed = 0;

    Motor_Init(motor_2);
  
    Motor motor_3;
    motor_3.GPIO_Direction_GPIOx = GPIOB;
    motor_3.GPIO_Direction_Pin = GPIO_Pin_15;
    motor_3.GPIO_Direction_RCC = RCC_AHBPeriph_GPIOB;
    motor_3.GPIO_PWM_GPIOx = GPIOB;
    motor_3.GPIO_PWM_Pin = GPIO_Pin_14;
    motor_3.GPIO_PWM_PinSource = GPIO_PinSource14;
    motor_3.GPIO_PWM_AF = GPIO_AF_1;
    motor_3.GPIO_PWM_RCC = RCC_AHBPeriph_GPIOB;
    motor_3.TIMx = TIM15;
    motor_3.TIMx_Channel = TIM_Channel_1;
    motor_3.TIMx_RCC = RCC_APB2Periph_TIM15;
    motor_3.TIMx_Mode = TIM_OCMode_PWM1;
    motor_3.direction = Bit_RESET;
    motor_3.speed = 0;

    Motor_Init(motor_3);

    Motor motor_4;
    motor_4.GPIO_Direction_GPIOx = GPIOA;
    motor_4.GPIO_Direction_Pin = GPIO_Pin_0;
    motor_4.GPIO_Direction_RCC = RCC_AHBPeriph_GPIOA;
    motor_4.GPIO_PWM_GPIOx = GPIOA;
    motor_4.GPIO_PWM_Pin = GPIO_Pin_1;
    motor_4.GPIO_PWM_PinSource = GPIO_PinSource1;
    motor_4.GPIO_PWM_AF = GPIO_AF_1;
    motor_4.GPIO_PWM_RCC = RCC_AHBPeriph_GPIOA;
    motor_4.TIMx = TIM2;
    motor_4.TIMx_Channel = TIM_Channel_2;
    motor_4.TIMx_RCC = RCC_APB1Periph_TIM2;
    motor_4.TIMx_Mode = TIM_OCMode_PWM1;
    motor_4.direction = Bit_RESET;
    motor_4.speed = 0;

    Motor_Init(motor_4);
#endif

    //////////////////////////////////////////////////////////////////////////////
    // Main Loop
    while(1) {

#ifdef USB_TEST
        usb.printf("Congratulations, USB is working!\n\r");
#endif

#ifdef LED_TEST
        mRedTOGGLE;
        mGreenTOGGLE;
        mBlueTOGGLE;
        mWhiteTOGGLE;
        mYellowTOGGLE;
        DelayMilliseconds(500);
#endif

#ifdef CONNECTOR_TEST
        DelayMilliseconds(500);
        GPIO_WriteBit(GPIOB, GPIO_Pin_8, Bit_SET);
        GPIO_WriteBit(GPIOB, GPIO_Pin_9, Bit_SET);
        set_motor_direction(motor_1, Bit_SET);
        set_motor_direction(motor_2, Bit_RESET);
        set_motor_direction(motor_3, Bit_SET);
        set_motor_direction(motor_4, Bit_RESET);
        set_motor_speed(motor_1, 20);
        set_motor_speed(motor_2, 40);
        set_motor_speed(motor_3, 60);
        set_motor_speed(motor_4, 80);
        DelayMilliseconds(500);
        GPIO_WriteBit(GPIOB, GPIO_Pin_8, Bit_RESET);
        GPIO_WriteBit(GPIOB, GPIO_Pin_9, Bit_RESET);
        set_motor_direction(motor_1, Bit_RESET);
        set_motor_direction(motor_2, Bit_SET);
        set_motor_direction(motor_3, Bit_RESET);
        set_motor_direction(motor_4, Bit_SET);
#endif
        DelayMilliseconds(10);
 
    } // while(1)
    return(0);
}

