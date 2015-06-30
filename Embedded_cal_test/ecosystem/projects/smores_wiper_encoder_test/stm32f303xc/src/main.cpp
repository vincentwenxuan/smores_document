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
#include "Wheel_Control.h"
#include "Trajectory.h"

#include "stm_cali.h"


#define M_BLUE_TOGGLE 0xA0
#define GET_ADC_DATA 0xA6
#define M_YELLOW_TOGGLE 0xA2

Motor left_wheel_motor;
Motor right_wheel_motor;
Motor right_pan_tilt_motor;
Motor left_pan_tilt_motor;

int main (void){
    InitPeripherals();
    
    UsbInterface usb = UsbInterface();
    usb.Init();

    // right_pan_tilt_motor.GPIO_Direction_GPIOx = GPIOB;
    // right_pan_tilt_motor.GPIO_Direction_Pin = GPIO_Pin_2;
    // right_pan_tilt_motor.GPIO_Direction_RCC = RCC_AHBPeriph_GPIOB;
    // right_pan_tilt_motor.GPIO_PWM_GPIOx = GPIOA;
    // right_pan_tilt_motor.GPIO_PWM_Pin = GPIO_Pin_8;
    // right_pan_tilt_motor.GPIO_PWM_PinSource = GPIO_PinSource8;
    // right_pan_tilt_motor.GPIO_PWM_AF = GPIO_AF_6;
    // right_pan_tilt_motor.GPIO_PWM_RCC = RCC_AHBPeriph_GPIOA;
    // right_pan_tilt_motor.TIMx = TIM1;
    // right_pan_tilt_motor.TIMx_Channel = TIM_Channel_1;
    // right_pan_tilt_motor.TIMx_RCC = RCC_APB2Periph_TIM1;
    // right_pan_tilt_motor.TIMx_Mode = TIM_OCMode_PWM1;
    // right_pan_tilt_motor.direction = Bit_RESET;
    // right_pan_tilt_motor.speed = 0;
    
    // Motor_Init(right_pan_tilt_motor);
    
    // left_pan_tilt_motor.GPIO_Direction_GPIOx = GPIOB;
    // left_pan_tilt_motor.GPIO_Direction_Pin = GPIO_Pin_15;
    // left_pan_tilt_motor.GPIO_Direction_RCC = RCC_AHBPeriph_GPIOB;
    // left_pan_tilt_motor.GPIO_PWM_GPIOx = GPIOB;
    // left_pan_tilt_motor.GPIO_PWM_Pin = GPIO_Pin_14;
    // left_pan_tilt_motor.GPIO_PWM_PinSource = GPIO_PinSource14;
    // left_pan_tilt_motor.GPIO_PWM_AF = GPIO_AF_1;
    // left_pan_tilt_motor.GPIO_PWM_RCC = RCC_AHBPeriph_GPIOB;
    // left_pan_tilt_motor.TIMx = TIM15;
    // left_pan_tilt_motor.TIMx_Channel = TIM_Channel_1;
    // left_pan_tilt_motor.TIMx_RCC = RCC_APB2Periph_TIM15;
    // left_pan_tilt_motor.TIMx_Mode = TIM_OCMode_PWM1;
    // left_pan_tilt_motor.direction = Bit_RESET;
    // left_pan_tilt_motor.speed = 0;
    
    // Motor_Init(left_pan_tilt_motor);

    // set_motor_direction(right_pan_tilt_motor, Bit_RESET);
    // set_motor_speed(right_pan_tilt_motor, 50);

    // set_motor_direction(left_pan_tilt_motor, Bit_RESET);
    // set_motor_speed(left_pan_tilt_motor, 50);

    right_wheel_motor.GPIO_Direction_GPIOx = GPIOA;
    right_wheel_motor.GPIO_Direction_Pin = GPIO_Pin_10;
    right_wheel_motor.GPIO_Direction_RCC = RCC_AHBPeriph_GPIOA;
    right_wheel_motor.GPIO_PWM_GPIOx = GPIOB;
    right_wheel_motor.GPIO_PWM_Pin = GPIO_Pin_5;
    right_wheel_motor.GPIO_PWM_PinSource = GPIO_PinSource5;
    right_wheel_motor.GPIO_PWM_AF = GPIO_AF_10;
    right_wheel_motor.GPIO_PWM_RCC = RCC_AHBPeriph_GPIOB;
    right_wheel_motor.TIMx = TIM17;
    right_wheel_motor.TIMx_Channel = TIM_Channel_1;
    right_wheel_motor.TIMx_RCC = RCC_APB2Periph_TIM17;
    right_wheel_motor.TIMx_Mode = TIM_OCMode_PWM1;
    right_wheel_motor.direction = Bit_RESET;
    right_wheel_motor.speed = 0;

    Motor_Init(right_wheel_motor);

    set_motor_direction(right_wheel_motor, Bit_SET);
    set_motor_speed(right_wheel_motor, 50);

    // left_wheel_motor.GPIO_Direction_GPIOx = GPIOA;
    // left_wheel_motor.GPIO_Direction_Pin = GPIO_Pin_0;
    // left_wheel_motor.GPIO_Direction_RCC = RCC_AHBPeriph_GPIOA;
    // left_wheel_motor.GPIO_PWM_GPIOx = GPIOA;
    // left_wheel_motor.GPIO_PWM_Pin = GPIO_Pin_1;
    // left_wheel_motor.GPIO_PWM_PinSource = GPIO_PinSource1;
    // left_wheel_motor.GPIO_PWM_AF = GPIO_AF_1;
    // left_wheel_motor.GPIO_PWM_RCC = RCC_AHBPeriph_GPIOA;
    // left_wheel_motor.TIMx = TIM2;
    // left_wheel_motor.TIMx_Channel = TIM_Channel_2;
    // left_wheel_motor.TIMx_RCC = RCC_APB1Periph_TIM2;
    // left_wheel_motor.TIMx_Mode = TIM_OCMode_PWM1;
    // left_wheel_motor.direction = Bit_RESET;
    // left_wheel_motor.speed = 0;

    // Motor_Init(left_wheel_motor);

    // set_motor_direction(left_wheel_motor, Bit_SET);
    // set_motor_speed(left_wheel_motor, 50);

    char slave_addr = 0x14; // bottom 0x16, right 0x18, left 0x14, top 0x1A
    char cmd;
    uint8_t data[11] = {0,0,0,0,0,0,0,0,0,0,0};
    // uint8_t data[9] = {0,0,0,0,0,0,0,0,0};

    struct Time t;

    mWhiteON;mRedON;mBlueON;mYellowON;
    DelayMilliseconds(1000);
    mWhiteOFF;mRedOFF;mBlueOFF;mYellowOFF;

//    PlayTimebase();
    mBlueON;
    DelayMilliseconds(1000);
    mBlueTOGGLE;
    DelayMilliseconds(1000);
    mBlueTOGGLE;
    DelayMilliseconds(1000);
    mBlueTOGGLE;
    DelayMilliseconds(1000);
    mBlueTOGGLE;
    DelayMilliseconds(1000);
    cali_cmd(left_wheel_motor, right_wheel_motor, right_pan_tilt_motor, left_pan_tilt_motor);
    mBlueOFF;
    DelayMilliseconds(300);


    while(1)
    {
        mGreenTOGGLE;
        cmd = GET_ADC_DATA;

        mBusReadBurst(slave_addr, cmd, 11, data, 1);
        
        // t = GetTime();
        // MsgStates tx_state_msg;
        // tx_state_msg.time_sec = 5;
        // tx_state_msg.time_nsec = 5;
        // tx_state_msg.encoder_counter = 0;
        // tx_state_msg.velocity =5;
        // tx_state_msg.position = 5;
        // tx_state_msg.adc0 = 5;
        // tx_state_msg.adc1 = 5;
        // tx_state_msg.motor_cmd = 100;
        // usb.SendPacket(MsgStatesType, (uint8_t*)&tx_state_msg, sizeof(tx_state_msg));
        // usb.SendNow();
        
        if (data[10]==0xAA)
        {
            mYellowTOGGLE;
            
            // pan_wheel_currentState.Position = data[4] + data[5] * 256;
            // pan_wheel_currentState.Speed = data[2] + data[3] * 256;
            // wheel_control.updateState(pan_wheel_currentState);
            // int8_t pan_wheel_cmd = wheel_control.updateStateCmd(&usb);

            t = GetTime();
            MsgStates tx_state_msg;
            tx_state_msg.time_sec = t.sec;
            tx_state_msg.time_nsec = t.nsec;
            tx_state_msg.encoder_counter = 0;
            tx_state_msg.velocity = data[2] + (data[3] << 8);
            tx_state_msg.position = data[4] + (data[5] << 8);
            tx_state_msg.adc0 = data[6] + (data[7] << 8);
            tx_state_msg.adc1 = data[8] + (data[9] << 8);
            tx_state_msg.motor_cmd = 100;
            usb.SendPacket(MsgStatesType, (uint8_t*)&tx_state_msg, sizeof(tx_state_msg));
            usb.SendNow();

        }
        // mBusReadBurst(slave_addr, cmd, 9, data, 1);
        // if (data[8]==0xAA)
        // {
        //     mYellowTOGGLE;

        //     t = GetTime();
        //     MsgStates tx_state_msg;
        //     tx_state_msg.time_sec = t.sec;
        //     tx_state_msg.time_nsec = t.nsec;
        //     tx_state_msg.encoder_counter = 0;
        //     tx_state_msg.velocity = data[0] + data[1] * 256 + data[2] * 65536 + data[3] * 16777216;
        //     tx_state_msg.position = data[4] + data[5] * 256;
        //     tx_state_msg.adc0 = data[6] + data[7] * 256;
        //     tx_state_msg.adc1 = tx_state_msg.adc0;
        //     tx_state_msg.motor_cmd = 100;
        //     usb.SendPacket(MsgStatesType, (uint8_t*)&tx_state_msg, sizeof(tx_state_msg));
        //     usb.SendNow();
        // }

        // for matlab communication
        // usb.printf("%d ", t.sec);
        // usb.printf("%d ", t.nsec);
        // usb.printf("%d ", data[2] + data[3]*256);
        // usb.printf("%d\n\r", data[4] + data[5]*256);

        DelayMilliseconds(10);
    }
    return(0);
}

void timerCallback1(void)
{
    // static int16_t counter = 0;
    // static int16_t index = 1;
    // if(global_flag && start_flag)
    // {
    //     mBlueON;
    //     counter++;
    //     if(counter == timer_counter)
    //     {
    //         counter = 0;
    //         index++;
    //         if(index <= update_times)
    //         {
    //             pan_wheel_stateGoal.Position = path[index];
    //             pan_wheel_stateGoal.Speed = velocity[index];
    //         }
    //         else
    //         {
    //             index = 1;
    //             global_flag = 0;
    //             mBlueOFF;
    //         }
    //     }
    // }
    // mWhiteTOGGLE;
    
    static bool flag = 1;
    static u8 count;
    // if (flag)
    // {
    //     set_motor_direction(right_pan_tilt_motor, Bit_SET);
    //     set_motor_speed(right_pan_tilt_motor, 30);
        
    //     set_motor_direction(left_pan_tilt_motor, Bit_RESET);
    //     set_motor_speed(left_pan_tilt_motor, 30);
    //     flag = 0;
    // }
    // else
    // {
    //     count++;
    //     set_motor_direction(right_pan_tilt_motor, Bit_RESET);
    //     set_motor_speed(right_pan_tilt_motor, 0);
        
    //     set_motor_direction(left_pan_tilt_motor, Bit_SET);
    //     set_motor_speed(left_pan_tilt_motor, 0);
    //     if (count == 5)
    //     {
    //         flag = 1;
    //         count = 0;
    //     }
    // }

    if (flag)
    {
//        mYellowON;
        mBlueOFF;
        count++;
        set_motor_direction(right_pan_tilt_motor, Bit_SET);
        set_motor_speed(right_pan_tilt_motor, 30);
        set_motor_direction(left_pan_tilt_motor, Bit_RESET);
        set_motor_speed(left_pan_tilt_motor, 30);
        if (count == 2)
        {
            flag = 0;
            count = 0;
        }
    }
    else
    {
        //mBlueON;
//        mYellowOFF;
        count++;
        set_motor_direction(right_pan_tilt_motor, Bit_RESET);
        set_motor_speed(right_pan_tilt_motor, 30);
        set_motor_direction(left_pan_tilt_motor, Bit_SET);
        set_motor_speed(left_pan_tilt_motor, 30);
        if (count == 2)
        {
            flag = 1;
            count = 0;
        }
    }

    mWhiteTOGGLE;
}
