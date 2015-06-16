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
#include "Wheel_Control.h"


#define M_BLUE_TOGGLE 0xA0
#define GET_ADC_DATA 0xA1
#define M_YELLOW_TOGGLE 0xA2

void TIM8_Encoder_Init(void);


int main (void){
    InitPeripherals();
    
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

    Motor wheel_motor;
    wheel_motor.GPIO_Direction_GPIOx = GPIOB;
    wheel_motor.GPIO_Direction_Pin = GPIO_Pin_2;
    wheel_motor.GPIO_Direction_RCC = RCC_AHBPeriph_GPIOB;
    wheel_motor.GPIO_PWM_GPIOx = GPIOA;
    wheel_motor.GPIO_PWM_Pin = GPIO_Pin_9;
    wheel_motor.GPIO_PWM_PinSource = GPIO_PinSource9;
    wheel_motor.GPIO_PWM_AF = GPIO_AF_6;
    wheel_motor.GPIO_PWM_RCC = RCC_AHBPeriph_GPIOA;
    wheel_motor.TIMx = TIM1;
    wheel_motor.TIMx_Channel = TIM_Channel_2;
    wheel_motor.TIMx_RCC = RCC_APB2Periph_TIM1;
    wheel_motor.TIMx_Mode = TIM_OCMode_PWM1;
    wheel_motor.direction = Bit_RESET;
    wheel_motor.speed = 0;

    Motor_Init(wheel_motor);

    set_motor_direction(wheel_motor, Bit_SET);
    set_motor_speed(wheel_motor, 100);

    int8_t motor_cmd_min = -100;
    uint8_t motor_cmd_max = 100;

    float Kp = 0.01, Ki = 0, Kd = 0;
    PID_Controller wheel_pid_controller(Kp, Ki, Kd, motor_cmd_min, motor_cmd_max);
    Wheel_Controller_OneMotor wheel_control(&wheel_pid_controller, wheel_motor);

    PID_State wheel_stateGoal;
    wheel_stateGoal.Position = 10000;
    wheel_stateGoal.Speed = 0;
    wheel_control.setStateGoal(wheel_stateGoal);

    PID_State wheel_currentState;

    TIM8_Encoder_Init();
	
    char slave_addr = 0x1A;
    char cmd;
    uint8_t data[11] = {0,0,0,0,0,0,0,0,0,0,0};

    struct Time t;
    
    while(1)
    {
        STM_EVAL_LEDToggle(LED10);
        cmd = GET_ADC_DATA;
        mBusReadBurst(slave_addr, cmd, 11, data, 1);
        if(data[10]==0xAA)
        {
            t = GetTime();
            MsgStates tx_state_msg;
            tx_state_msg.time_sec = t.sec;
            tx_state_msg.time_nsec = t.nsec;
            tx_state_msg.encoder_counter = TIM_GetCounter(TIM8);
            tx_state_msg.velocity = data[2] + data[3] * 256;
            tx_state_msg.position = data[4] + data[5] * 256;
            tx_state_msg.adc0 = data[6] + data[7] * 256;
            tx_state_msg.adc1 = data[8] + data[9] * 256;
            usb.SendPacket(MsgStatesType, (uint8_t*)&tx_state_msg, sizeof(tx_state_msg));

            // wheel_currentState.Position = data[4] + data[5] * 256;
            // wheel_currentState.Speed = data[2] + data[3] * 256;
            // wheel_control.updateState(wheel_currentState);
            // int8_t wheel_cmd = wheel_control.updateStateCmd(&usb);
            // usb.printf("p: %d\n\r", wheel_currentState.Position);
            // usb.printf("cmd: %d\n\r", wheel_cmd);
        }


        // for matlab communication
        // usb.printf("%d ", t.sec);
        // usb.printf("%d ", t.nsec);
        // usb.printf("%d ", data[2] + data[3]*256);
        // usb.printf("%d\n\r", data[4] + data[5]*256);
	
        usb.SendNow();
        DelayMilliseconds(10);
    }
    return(0);
}

void timerCallback1(void)
{
    
}

void TIM8_Encoder_Init()
{
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);
    TIM_DeInit(TIM8);
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_ICInitTypeDef TIM_ICInitStructure;
     
    TIM_TimeBaseStructure.TIM_Prescaler = 0;
    TIM_TimeBaseStructure.TIM_Period = 18774; // Max value for encoder pulse
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);
    TIM_EncoderInterfaceConfig(TIM8, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
    TIM_ICStructInit(&TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_ICFilter = 0;
    TIM_ICInit(TIM8, &TIM_ICInitStructure);
    //	TIM_SetAutoreload(TIM2, 0xFFFF);
    TIM_Cmd(TIM8, ENABLE);
    TIM_SetCounter(TIM8, 0);
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
