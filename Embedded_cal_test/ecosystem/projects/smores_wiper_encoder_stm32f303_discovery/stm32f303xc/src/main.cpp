#include "common_peripherals.h"
#include "system_clock.h"
#include "mBus.h"
// common components 
#include "common_message_types.h" 
// common messages
#include "usb_interface.h"        
// usb interface
//#include "mBus.h"

// common libraries
#include "project_message_types.h"
#include "error_reporting.h"
#include <string.h> 
//needed for memset
//#define   mBUS 1

// user libraries
#include "stm32f3_discovery.h"
#include "DC_Motor.h"

#define M_BLUE_TOGGLE 0xA0
#define GET_ADC_DATA 0xA1
#define M_YELLOW_TOGGLE 0xA2

void TIM2_Encoder_Init();
void TIM8_Encoder_Init();

Motor motor_1;
static char duty_cycle = 0;

int main (void){
    InitPeripherals();
    
    PlayTimebase();
    
    UsbInterface usb = UsbInterface();
    usb.Init();
    
    STM_EVAL_LEDInit(LED3);
    STM_EVAL_LEDInit(LED4);
    STM_EVAL_LEDInit(LED5);
    STM_EVAL_LEDInit(LED6);
    STM_EVAL_LEDInit(LED7);
    STM_EVAL_LEDInit(LED8);
    STM_EVAL_LEDInit(LED9);
    STM_EVAL_LEDInit(LED10);

    motor_1.GPIO_Direction_GPIOx = GPIOB;
    motor_1.GPIO_Direction_Pin = GPIO_Pin_2;
    motor_1.GPIO_Direction_RCC = RCC_AHBPeriph_GPIOB;
    motor_1.GPIO_PWM_GPIOx = GPIOA;
    motor_1.GPIO_PWM_Pin = GPIO_Pin_9;
    motor_1.GPIO_PWM_PinSource = GPIO_PinSource9;
    motor_1.GPIO_PWM_AF = GPIO_AF_6;
    motor_1.GPIO_PWM_RCC = RCC_AHBPeriph_GPIOA;
    motor_1.TIMx = TIM1;
    motor_1.TIMx_Channel = TIM_Channel_2;
    motor_1.TIMx_RCC = RCC_APB2Periph_TIM1;
    motor_1.TIMx_Mode = TIM_OCMode_PWM1;
    motor_1.direction = Bit_RESET;
    motor_1.speed = 0;
  
    Motor_Init(motor_1);

    set_motor_direction(motor_1, Bit_SET);
    set_motor_speed(motor_1, 0);

    //	TIM2_Encoder_Init();
    TIM8_Encoder_Init();

    //	u16 counter1 = 0;
    u16 counter2 = 0;
	
    char slave_addr = 0x14;
    char cmd;
    uint8_t data[7] = {0,0,0,0,0,0,0};

    struct Time t;

//    int flag = 100;
    while(1)
    {
        STM_EVAL_LEDToggle(LED10);
        cmd = GET_ADC_DATA;
        // while( mBusReadBurstStartNB(slave_addr, cmd, 6, 1) == ERROR );
        // mBusReadBurstDataNB(6, data, 1);
        mBusReadBurst(slave_addr, cmd, 7, data, 1);
        //		counter1 = TIM_GetCounter(TIM2);
        if (data[6] == 0xAA) {
            counter2 = TIM_GetCounter(TIM8);
            t = GetTime();
            // for matlab communication
            usb.printf("%d ", t.sec);
            usb.printf("%d ", t.nsec);
            usb.printf("%d ", data[2] + data[3]*256);
            usb.printf("%d ", data[4] + data[5]*256);
            usb.printf("%d ", data[6]);
//        int16_t velocity = data[2] + data[3]*256;
//        int16_t position = data[4] + data[5]*256;
//        usb.printf("%d ", velocity);
//        usb.printf("%d\n\r", position);
            //		usb.printf("encoder1 = %d\n\r", counter1);
            usb.printf("%d ", counter2);
            usb.printf("%d\n\r", duty_cycle);
        }
        
        // for terminal debug
        // usb.printf("tsec: %d\n\r", t.sec);
        // usb.printf("tnsec: %d\n\r", t.nsec);
        // usb.printf("adc0: %d\n\r", data[2] + data[3] * 255);
        // usb.printf("adc1: %d\n\r", data[4] + data[5] * 255);
        // //usb.printf("encoder1 = %d\n\r", counter1);
        // usb.printf("encoder2: %d\n\r", counter2);
	
        usb.SendNow();
        DelayMilliseconds(5);
    }
    return(0);
}

void timerCallback1(void)
{
    static char counter = 0;
    static bool flag_direction = 0;
    counter++;
    if (counter == 5) {
        counter = 0;
        if (flag_direction == 0) {
            STM_EVAL_LEDOn(LED4);
            STM_EVAL_LEDOff(LED5);
            duty_cycle += 20;
            set_motor_direction(motor_1, Bit_SET);
            set_motor_speed(motor_1, duty_cycle);
            if (duty_cycle == 100) {
                flag_direction = 1;
                duty_cycle = 120;
            }
        } else {
            STM_EVAL_LEDOn(LED5);
            STM_EVAL_LEDOff(LED4);
            duty_cycle -= 20;
            set_motor_direction(motor_1, Bit_RESET);
            set_motor_speed(motor_1, duty_cycle);
            if (duty_cycle == 0) {
                flag_direction = 0;
            }
        }
    }
    STM_EVAL_LEDToggle(LED3);
}

void TIM2_Encoder_Init()
{
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    TIM_DeInit(TIM2);
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_ICInitTypeDef TIM_ICInitStructure;
     
    TIM_TimeBaseStructure.TIM_Prescaler = 0;
    TIM_TimeBaseStructure.TIM_Period = 0xFFFF; // Max value for encoder pulse
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    TIM_EncoderInterfaceConfig(TIM2, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
    TIM_ICStructInit(&TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_ICFilter = 0;
    TIM_ICInit(TIM2, &TIM_ICInitStructure);
    //	TIM_SetAutoreload(TIM2, 0xFFFF);
    TIM_SetCounter(TIM2, 0);
    TIM_Cmd(TIM2, ENABLE);
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOD, &GPIO_InitStruct);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource3, GPIO_AF_2);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_2);
}

void TIM8_Encoder_Init()
{
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);
    TIM_DeInit(TIM8);
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_ICInitTypeDef TIM_ICInitStructure;
     
    TIM_TimeBaseStructure.TIM_Prescaler = 0;
    TIM_TimeBaseStructure.TIM_Period = 0xFFFF; // Max value for encoder pulse
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);
    TIM_EncoderInterfaceConfig(TIM8, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
    TIM_ICStructInit(&TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_ICFilter = 0;
    TIM_ICInit(TIM8, &TIM_ICInitStructure);
    //	TIM_SetAutoreload(TIM2, 0xFFFF);
    TIM_SetCounter(TIM8, 0);
    TIM_Cmd(TIM8, ENABLE);
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOC, &GPIO_InitStruct);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_4);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_4);
}
