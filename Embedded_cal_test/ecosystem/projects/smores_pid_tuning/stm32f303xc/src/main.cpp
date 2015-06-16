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

#define POSITION_CONTROL
//#define SPEED_CONTROL

#define M_BLUE_TOGGLE 0xA0
#define GET_ADC_DATA 0xA1
#define M_YELLOW_TOGGLE 0xA2

#define UPDATE_GOAL 10
#define SEND_DATA   20

#ifdef POSITION_CONTROL
float path[301];
float velocity[301];
#endif

#ifdef SPEED_CONTROL
float path_speed[329];
float velocity_speed;
#endif

PID_State pan_wheel_stateGoal;
PID_State tilt_wheel_stateGoal;
bool global_flag;
bool start_flag;
int timer_counter;
int update_times;

int main (void){
    InitPeripherals();
    
    UsbInterface usb = UsbInterface();
    usb.Init();

    // Motor left_wheel_motor;
    // Motor right_wheel_motor;
    Motor right_pan_tilt_motor;
    Motor left_pan_tilt_motor;

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

    set_motor_direction(right_pan_tilt_motor, Bit_SET);
    set_motor_speed(right_pan_tilt_motor, 0);
    
    set_motor_direction(left_pan_tilt_motor, Bit_SET);
    set_motor_speed(left_pan_tilt_motor, 0);

    // right_wheel_motor.GPIO_Direction_GPIOx = GPIOA;
    // right_wheel_motor.GPIO_Direction_Pin = GPIO_Pin_10;
    // right_wheel_motor.GPIO_Direction_RCC = RCC_AHBPeriph_GPIOA;
    // right_wheel_motor.GPIO_PWM_GPIOx = GPIOA;
    // right_wheel_motor.GPIO_PWM_Pin = GPIO_Pin_9;
    // right_wheel_motor.GPIO_PWM_PinSource = GPIO_PinSource9;
    // right_wheel_motor.GPIO_PWM_AF = GPIO_AF_6;
    // right_wheel_motor.GPIO_PWM_RCC = RCC_AHBPeriph_GPIOA;
    // right_wheel_motor.TIMx = TIM1;
    // right_wheel_motor.TIMx_Channel = TIM_Channel_2;
    // right_wheel_motor.TIMx_RCC = RCC_APB2Periph_TIM1;
    // right_wheel_motor.TIMx_Mode = TIM_OCMode_PWM1;
    // right_wheel_motor.direction = Bit_RESET;
    // right_wheel_motor.speed = 0;

    // Motor_Init(right_wheel_motor);

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

    int8_t motor_cmd_min = -100;
    uint8_t motor_cmd_max = 100;

    float Kp = 0.018, Ki = 0, Kd = 0.00011;
    PID_Controller wheel_pid_controller_pan(Kp, Ki, Kd, motor_cmd_min, motor_cmd_max);
    PID_Controller wheel_pid_controller_tilt(Kp, Ki, Kd, motor_cmd_min, motor_cmd_max);
    Wheel_Controller_Pan_Tilt wheel_control(&wheel_pid_controller_pan, &wheel_pid_controller_tilt, left_pan_tilt_motor, right_pan_tilt_motor);

#ifdef POSITION_CONTROL
    // for position control  //
//    float coeff[6] = {-493.8272, 3703.7037, -7407.4074, 0, 0, 20000};
    float coeff[6] = {617.2840, -4629.6296, 9259.2593, 0, 0, -15000};
//    float coeff[6] = {493.8272, -3703.7037, 7407.4074, 0, 0, 0};
//    float coeff[6] = {246.9136, -1851.8519, 3703.7037, 0, 0, 10000};
    Polynomial p(coeff, 6);
    float startTime = 0;
    float endTime = 3;
    uint16_t sectionNum = 300;
    Trajectory trajectory(startTime, endTime, sectionNum, &p);
    trajectory.GeneratePositionTrajectory(path);
    trajectory.GenerateVelocityTrajectory(velocity);

    timer_counter = (endTime - startTime) / sectionNum / 0.01;
    update_times = 300;

    pan_wheel_stateGoal.Position = 0;
    pan_wheel_stateGoal.Speed = 0;
    tilt_wheel_stateGoal.Position = path[0];
    tilt_wheel_stateGoal.Speed = velocity[0];
    wheel_control.setStateGoal(pan_wheel_stateGoal, tilt_wheel_stateGoal);
#endif

#ifdef SPEED_CONTROL
    // for velocity control  //

    timer_counter = 1;
    float coeff_speed[2] = {20000, 0};
    Polynomial p_speed(coeff_speed, 2);
    uint16_t positionNum = (32767*2)/(20000*0.01*timer_counter) + 0.5;
    float min_pos = -32767;
    float max_pos = 32767;
    Trajectory trajectory_speed(positionNum, min_pos, max_pos, &p_speed);
    trajectory_speed.GeneratePositionVelocityTrajectory(path_speed, &velocity_speed);
    update_times = positionNum;
    
    pan_wheel_stateGoal.Position = path_speed[0];
    pan_wheel_stateGoal.Speed = velocity_speed;
    wheel_control.setStateGoal(pan_wheel_stateGoal);
#endif
    
    // pan_wheel_stateGoal.Position = 0;
    // pan_wheel_stateGoal.Speed = 0;
    // wheel_control.setStateGoal(pan_wheel_stateGoal);

    PID_State tilt_wheel_currentState;

    char slave_addr = 0x16;
    char cmd;
    uint8_t data[11] = {0,0,0,0,0,0,0,0,0,0,0};

    // struct Time t;

    int16_t current_position = 0;
    int32_t current_velocity = 0;
    int8_t left_motor_cmd = 0;
    int8_t right_motor_cmd = 0;

    mWhiteON;mRedON;mBlueON;mYellowON;
    DelayMilliseconds(1000);
    mWhiteOFF;mRedOFF;mBlueOFF;mYellowOFF;

//    int index = 1;
    start_flag = 0;
    global_flag = 1;
    PlayTimebase();

    while(1)
    {

        // for(int i = 0; i<301; i++)
        // {
        //     usb.printf("%d: %d\n\r", i, (int16_t)path_speed[i]);
        //     usb.printf("v: %d\n\r", (int32_t)velocity_speed);
        //     usb.SendNow();
        // }
        mGreenTOGGLE;

        uint8_t *rx_data;
        uint8_t rx_length;
        uint8_t is_data = 0;

        usb.GetBytes();
        while(usb.PeekPacket(&rx_data, &rx_length)) {
            uint8_t type = rx_data[0];
            if(type == MsgControlType) {
                MsgControl *rx_msg = ((MsgControl*) (rx_data+1));
                switch (rx_msg->cmd) {
                case UPDATE_GOAL:
                    mRedTOGGLE;
                    pan_wheel_stateGoal.Position = rx_msg->goal_position;
                    pan_wheel_stateGoal.Speed = rx_msg->goal_velocity;
                    break;
                case SEND_DATA:
                    mWhiteTOGGLE;
                    MsgCurrentState tx_msg;
                    tx_msg.goal_position = tilt_wheel_stateGoal.Position;
                    tx_msg.goal_velocity = tilt_wheel_stateGoal.Speed;
                    tx_msg.current_position = current_position;
                    tx_msg.current_velocity = current_velocity;
                    tx_msg.motor_cmd = left_motor_cmd;
                    usb.SendPacket(MsgCurrentStateType, (uint8_t*)&tx_msg, sizeof(tx_msg));
                    break;
                }
            }
            usb.DropPacket();
            is_data = 1;
        }
        if(is_data) {
            is_data = 0;
            usb.SendNow();
        }
        wheel_control.setStateGoal(pan_wheel_stateGoal, tilt_wheel_stateGoal);
        
        cmd = GET_ADC_DATA;
        // mBusReadBurst(slave_addr, cmd, 11, data, 1);
        mBusReadBurst(slave_addr, cmd, 9, data, 1);

        // usb.printf("i %d ", index);
        // usb.printf("P %d ", pan_wheel_stateGoal.Position);
        // usb.printf("V %d\n\r", pan_wheel_stateGoal.Speed);
        // usb.SendNow();

        // if (data[10]==0xAA)
        if (data[8]==0xAA)
        {
            start_flag = 1;
            mYellowTOGGLE;
            // if(index < 301)
            // {
            //     // pan_wheel_stateGoal.Position = path[index];
            //     // pan_wheel_stateGoal.Speed = velocity[index];
            //     // wheel_control.setStateGoal(pan_wheel_stateGoal);
            //     pan_wheel_stateGoal.Position = (int16_t)path_speed[index];
            //     pan_wheel_stateGoal.Speed = (int32_t)velocity_speed;
            //     wheel_control.setStateGoal(pan_wheel_stateGoal);
            //     index++;
            //     if(index > 300)
            //     {
            //         index = 1;
            //     }
            // }
            

            current_velocity = data[0] + data[1] * 256 + data[2] * 65536 + data[3] * 16777216;
            current_position = data[4] + data[5] * 256;

            tilt_wheel_currentState.Position = current_position;
            tilt_wheel_currentState.Speed = current_velocity;
            wheel_control.updateState(pan_wheel_stateGoal, tilt_wheel_currentState);
            wheel_control.updateStateCmd(&left_motor_cmd, &right_motor_cmd, &usb);

            // t = GetTime();
            // MsgStates tx_state_msg;
            // tx_state_msg.time_sec = t.sec;
            // tx_state_msg.time_nsec = t.nsec;
            // tx_state_msg.encoder_counter = 0;
            // tx_state_msg.velocity = data[0] + data[1] * 256 + data[2] * 65536 + data[3] * 16777216;
            // tx_state_msg.position = data[4] + data[5] * 256;
            // tx_state_msg.adc0 = data[6] + data[7] * 256;
            // tx_state_msg.adc1 = data[8] + data[9] * 256;
            // tx_state_msg.motor_cmd = pan_wheel_cmd;
            // usb.SendPacket(MsgStatesType, (uint8_t*)&tx_state_msg, sizeof(tx_state_msg));
            // usb.SendNow();
        }

        // for matlab communication
        // usb.printf("%d ", t.sec);
        // usb.printf("%d ", t.nsec);
        // usb.printf("%d ", data[2] + data[3]*256);
        // usb.printf("%d\n\r", data[4] + data[5]*256);


//        DelayMilliseconds(5);
    }
    return(0);
}

void timerCallback1(void)
{
    static int16_t counter = 0;
    static int16_t index = 0;
    if(global_flag && start_flag)
    {
        mBlueON;
        counter++;
        if(counter == timer_counter)
        {
            counter = 0;
            index++;
            if(index <= update_times)
            {
#ifdef POSITION_CONTROL
                tilt_wheel_stateGoal.Position = path[index];
                tilt_wheel_stateGoal.Speed = velocity[index];
#endif
#ifdef SPEED_CONTROL
                pan_wheel_stateGoal.Position = path_speed[index];
                pan_wheel_stateGoal.Speed = velocity_speed;
#endif
            }
            else
            {
                index = 0;
#ifdef POSITION_CONTROL
                global_flag = 0;
#endif
                
#ifdef SPEED_CONTROL
                global_flag = 1;
#endif
                mBlueOFF;
            }
        }
    }
    mWhiteTOGGLE;
    
//     static bool flag = 1;
//     static u8 count;
    // if (flag)
    // {
    //     set_motor_direction(right_pan_tilt_motor, Bit_SET);
    //     set_motor_speed(right_pan_tilt_motor, 30);
        
    //     set_motor_direction(left_pan_tilt_motor, Bit_SET);
    //     set_motor_speed(left_pan_tilt_motor, 30);
    //     flag = 0;
    // }
    // else
    // {
    //     count++;
    //     set_motor_direction(right_pan_tilt_motor, Bit_SET);
    //     set_motor_speed(right_pan_tilt_motor, 0);
        
    //     set_motor_direction(left_pan_tilt_motor, Bit_SET);
    //     set_motor_speed(left_pan_tilt_motor, 0);
    //     if (count == 5)
    //     {
    //         flag = 1;
    //         count = 0;
    //     }
    // }

    // if (flag)
    // {
    //     mYellowON;
    //     mBlueOFF;
    //     count++;
    //     set_motor_direction(right_pan_tilt_motor, Bit_SET);
    //     set_motor_speed(right_pan_tilt_motor, 80);
    //     set_motor_direction(left_pan_tilt_motor, Bit_SET);
    //     set_motor_speed(left_pan_tilt_motor, 80);
    //     if (count == 5)
    //     {
    //         flag = 0;
    //         count = 0;
    //     }
    // }
    // else
    // {
    //     mBlueON;
    //     mYellowOFF;
    //     count++;
    //     set_motor_direction(right_pan_tilt_motor, Bit_RESET);
    //     set_motor_speed(right_pan_tilt_motor, 80);
    //     set_motor_direction(left_pan_tilt_motor, Bit_RESET);
    //     set_motor_speed(left_pan_tilt_motor, 80);
    //     if (count == 5)
    //     {
    //         flag = 1;
    //         count = 0;
    //     }
    // }

//    mWhiteTOGGLE;
}
