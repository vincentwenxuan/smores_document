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

//#define TELECONTROL
//#define POSITION_CONTROL
//#define SPEED_CONTROL

#define M_BLUE_TOGGLE 0xA0
#define GET_ADC_DATA 0xA1
#define M_YELLOW_TOGGLE 0xA2
#define GET_STATES 0xA4

#define UPDATE_GOAL 10
#define SEND_DATA   20

float path_left[151];
float velocity_left[151];    // min duration is 1.5

float path_speed_left[165];
float velocity_speed_left;    // max is 40000

PID_State left_wheel_stateGoal;
bool init_flag_left;
bool global_flag_left;
bool start_flag_left;
int timer_counter_left;
int update_times_left;
int16_t counter_left;
int16_t index_position_left;
bool start_control_flag_left;


bool global_flag_speed_left;
int timer_counter_speed_left;
int update_times_speed_left;
int16_t counter_speed_left;
int16_t index_speed_left;
bool start_control_speed_flag_left;

float path_right[151];
float velocity_right[151];    // min duration is 1.5

float path_speed_right[165];
float velocity_speed_right;    // max is 40000

PID_State right_wheel_stateGoal;
bool init_flag_right;
bool global_flag_right;
bool start_flag_right;
int timer_counter_right;
int update_times_right;
int16_t counter_right;
int16_t index_position_right;
bool start_control_flag_right;


bool global_flag_speed_right;
int timer_counter_speed_right;
int update_times_speed_right;
int16_t counter_speed_right;
int16_t index_speed_right;
bool start_control_speed_flag_right;

float path_pan[151];
float velocity_pan[151];    // min duration is 1.5

float path_speed_pan[165];
float velocity_speed_pan;    // max is 40000

PID_State pan_wheel_stateGoal;
bool init_flag_pan;
bool global_flag_pan;
bool start_flag_pan;
int timer_counter_pan;
int update_times_pan;
int16_t counter_pan;
int16_t index_position_pan;
bool start_control_flag_pan;


bool global_flag_speed_pan;
int timer_counter_speed_pan;
int update_times_speed_pan;
int16_t counter_speed_pan;
int16_t index_speed_pan;
bool start_control_speed_flag_pan;

float path_tilt[151];
float velocity_tilt[151];    // min duration is 1.5

PID_State tilt_wheel_stateGoal;
bool init_flag_tilt;
bool global_flag_tilt;
bool start_flag_tilt;
int timer_counter_tilt;
int update_times_tilt;
int16_t counter_tilt;
int16_t index_position_tilt;
bool start_control_flag_tilt;

bool pan_ready_flag;
bool tilt_ready_flag;

int main (void){
    InitPeripherals();
    
    UsbInterface usb = UsbInterface();
    usb.Init();

    Motor left_wheel_motor;
    Motor right_wheel_motor;
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

    int8_t motor_cmd_min = -100;
    uint8_t motor_cmd_max = 100;

    float min_pos = -32767;
    float max_pos = 32767;

    float Kp = 0.018, Ki = 0, Kd = 0.0002;
    PID_Controller left_wheel_pid_controller(Kp, Ki, Kd, motor_cmd_min, motor_cmd_max);
    Wheel_Controller_OneMotor left_wheel_control(&left_wheel_pid_controller, left_wheel_motor);

    float coeff_left[6] = {0,0,0,0,0,0};
    float duration_left = 0;
    Polynomial p_left(coeff_left, 6);
    float startTime_left = 0;
    float endTime_left = 0;
    uint16_t sectionNum_left = 150;
    Trajectory trajectory_left(startTime_left, endTime_left, sectionNum_left, &p_left);

    float coeff_speed_left[2] = {0,0};
    Polynomial p_speed_left(coeff_speed_left, 2);
    uint16_t positionNum_left = 164;
    Trajectory trajectory_speed_left(positionNum_left, min_pos, max_pos, &p_speed_left);
    trajectory_speed_left.GeneratePositionVelocityTrajectory(path_speed_left, &velocity_speed_left);
    update_times_speed_left = positionNum_left;
    
    left_wheel_stateGoal.Position = 0;
    left_wheel_stateGoal.Speed = 0;
    left_wheel_control.setStateGoal(left_wheel_stateGoal);

    PID_State left_wheel_currentState;

    char slave_addr_left = 0x14;
    uint8_t data_left[7] = {0,0,0,0,0,0,0};


    int16_t current_position_left = 0;
    int32_t current_velocity_left = 0;
    int8_t left_wheel_cmd = 0;

    // right wheel control
    PID_Controller right_wheel_pid_controller(Kp, Ki, Kd, motor_cmd_min, motor_cmd_max);
    Wheel_Controller_OneMotor right_wheel_control(&right_wheel_pid_controller, right_wheel_motor);

    float coeff_right[6] = {0,0,0,0,0,0};
    float duration_right = 0;
    Polynomial p_right(coeff_right, 6);
    float startTime_right = 0;
    float endTime_right = 0;
    uint16_t sectionNum_right = 150;
    Trajectory trajectory_right(startTime_right, endTime_right, sectionNum_right, &p_right);

    float coeff_speed_right[2] = {0,0};
    Polynomial p_speed_right(coeff_speed_right, 2);
    uint16_t positionNum_right = 164;
    Trajectory trajectory_speed_right(positionNum_right, min_pos, max_pos, &p_speed_right);
    trajectory_speed_right.GeneratePositionVelocityTrajectory(path_speed_right, &velocity_speed_right);
    update_times_speed_right = positionNum_right;
    
    right_wheel_stateGoal.Position = 0;
    right_wheel_stateGoal.Speed = 0;
    right_wheel_control.setStateGoal(right_wheel_stateGoal);

    PID_State right_wheel_currentState;

    char slave_addr_right = 0x18;
    uint8_t data_right[7] = {0,0,0,0,0,0,0};


    int16_t current_position_right = 0;
    int32_t current_velocity_right = 0;
    int8_t right_wheel_cmd = 0;

    // pan and tilt DoF control
    float Kd_pan = 0.00011;
    float Kd_tilt = 0.00011;
    PID_Controller wheel_pid_controller_pan(Kp, Ki, Kd_pan, motor_cmd_min, motor_cmd_max);
    PID_Controller wheel_pid_controller_tilt(Kp, Ki, Kd_tilt, motor_cmd_min, motor_cmd_max);
    Wheel_Controller_Pan_Tilt wheel_control_pan_tilt(&wheel_pid_controller_pan, &wheel_pid_controller_tilt, left_pan_tilt_motor, right_pan_tilt_motor);

    float coeff_pan[6] = {0,0,0,0,0,0};
    float duration_pan = 0;
    Polynomial p_pan(coeff_pan, 6);
    float startTime_pan = 0;
    float endTime_pan = 0;
    uint16_t sectionNum_pan = 150;
    Trajectory trajectory_pan(startTime_pan, endTime_pan, sectionNum_pan, &p_pan);

    float coeff_speed_pan[2] = {0,0};
    Polynomial p_speed_pan(coeff_speed_pan, 2);
    uint16_t positionNum_pan = 164;
    Trajectory trajectory_speed_pan(positionNum_pan, min_pos, max_pos, &p_speed_pan);
    trajectory_speed_pan.GeneratePositionVelocityTrajectory(path_speed_pan, &velocity_speed_pan);
    update_times_speed_pan = positionNum_pan;
    
    pan_wheel_stateGoal.Position = 0;
    pan_wheel_stateGoal.Speed = 0;

    PID_State pan_wheel_currentState;

    char slave_addr_pan = 0x1A;
    uint8_t data_pan[7] = {0,0,0,0,0,0,0};

    int16_t current_position_pan = 0;
    int32_t current_velocity_pan = 0;
    int8_t left_motor_cmd = 0;
    int8_t right_motor_cmd = 0;

    // tilt DoF control

    float coeff_tilt[6] = {0,0,0,0,0,0};
    float duration_tilt = 0;
    Polynomial p_tilt(coeff_tilt, 6);
    float startTime_tilt = 0;
    float endTime_tilt = 0;
    uint16_t sectionNum_tilt = 150;
    Trajectory trajectory_tilt(startTime_tilt, endTime_tilt, sectionNum_tilt, &p_tilt);
    
    tilt_wheel_stateGoal.Position = 0;
    tilt_wheel_stateGoal.Speed = 0;

    wheel_control_pan_tilt.setStateGoal(pan_wheel_stateGoal, tilt_wheel_stateGoal);

    PID_State tilt_wheel_currentState;

    char slave_addr_tilt = 0x16;
    uint8_t data_tilt[7] = {0,0,0,0,0,0,0};

    int16_t current_position_tilt = 0;
    int32_t current_velocity_tilt = 0;

    mWhiteON;mRedON;mBlueON;mYellowON;
    DelayMilliseconds(1000);
    mWhiteOFF;mRedOFF;mBlueOFF;mYellowOFF;

    start_flag_left = 0;
    start_flag_right = 0;
    start_flag_pan = 0;
    start_flag_tilt = 0;
    pan_ready_flag = 0;
    tilt_ready_flag = 0;
    char cmd;
    PlayTimebase();

    while(1)
    {
        mGreenTOGGLE;

        uint8_t *rx_data;
        uint8_t rx_length;
        uint8_t is_data = 0;

        usb.GetBytes();
        while(usb.PeekPacket(&rx_data, &rx_length)) {
            uint8_t type = rx_data[0];
            if(type == MsgControlType) {
                MsgControl *rx_msg = ((MsgControl*) (rx_data+1));
                switch (rx_msg->face_id) {
                case LEFT_DOF:
                    switch (rx_msg->cmd) {
                    case UPDATE_GOAL:
//                        mRedTOGGLE;
                        left_wheel_stateGoal.Position = rx_msg->goal_position;
                        left_wheel_stateGoal.Speed = rx_msg->goal_velocity;
                        break;
                    case SEND_DATA:
                        mWhiteTOGGLE;
                        MsgCurrentState tx_msg;
                        tx_msg.goal_position = left_wheel_stateGoal.Position;
                        tx_msg.goal_velocity = left_wheel_stateGoal.Speed;
                        tx_msg.current_position = current_position_left;
                        tx_msg.current_velocity = current_velocity_left;
                        tx_msg.motor_cmd = left_wheel_cmd;
                        usb.SendPacket(MsgCurrentStateType, (uint8_t*)&tx_msg, sizeof(tx_msg));
                        is_data = 1;
                        break;
                    }
                    break;
                case RIGHT_DOF:
                    switch (rx_msg->cmd) {
                    case UPDATE_GOAL:
//                        mRedTOGGLE;
                        right_wheel_stateGoal.Position = rx_msg->goal_position;
                        right_wheel_stateGoal.Speed = rx_msg->goal_velocity;
                        break;
                    case SEND_DATA:
                        mWhiteTOGGLE;
                        MsgCurrentState tx_msg;
                        tx_msg.goal_position = right_wheel_stateGoal.Position;
                        tx_msg.goal_velocity = right_wheel_stateGoal.Speed;
                        tx_msg.current_position = current_position_right;
                        tx_msg.current_velocity = current_velocity_right;
                        tx_msg.motor_cmd = right_wheel_cmd;
                        usb.SendPacket(MsgCurrentStateType, (uint8_t*)&tx_msg, sizeof(tx_msg));
                        is_data = 1;
                        break;
                    }
                    break;
                case PAN_DOF:
                    switch (rx_msg->cmd) {
                    case UPDATE_GOAL:
//                        mRedTOGGLE;
                        pan_wheel_stateGoal.Position = rx_msg->goal_position;
                        pan_wheel_stateGoal.Speed = rx_msg->goal_velocity;
                        break;
                    case SEND_DATA:
                        mWhiteTOGGLE;
                        MsgCurrentState tx_msg;
                        tx_msg.goal_position = pan_wheel_stateGoal.Position;
                        tx_msg.goal_velocity = pan_wheel_stateGoal.Speed;
                        tx_msg.current_position = current_position_pan;
                        tx_msg.current_velocity = current_velocity_pan;
                        tx_msg.motor_cmd = left_motor_cmd;
                        usb.SendPacket(MsgCurrentStateType, (uint8_t*)&tx_msg, sizeof(tx_msg));
                        is_data = 1;
                        break;
                    }
                    break;
                case TILT_DOF:
                    switch (rx_msg->cmd) {
                    case UPDATE_GOAL:
//                        mRedTOGGLE;
                        tilt_wheel_stateGoal.Position = rx_msg->goal_position;
                        tilt_wheel_stateGoal.Speed = rx_msg->goal_velocity;
                        break;
                    case SEND_DATA:
                        mWhiteTOGGLE;
                        MsgCurrentState tx_msg;
                        tx_msg.goal_position = tilt_wheel_stateGoal.Position;
                        tx_msg.goal_velocity = tilt_wheel_stateGoal.Speed;
                        tx_msg.current_position = current_position_tilt;
                        tx_msg.current_velocity = current_velocity_tilt;
                        tx_msg.motor_cmd = right_motor_cmd;
                        usb.SendPacket(MsgCurrentStateType, (uint8_t*)&tx_msg, sizeof(tx_msg));
                        is_data = 1;
                        break;
                    }
                    break;
                }
            }
            else if(type == MsgTrajectoryPositionType) {
                MsgTrajectoryPosition *rx_msg = ((MsgTrajectoryPosition*) (rx_data+1));
                switch (rx_msg->face_id) {
                case LEFT_DOF:
                    switch (rx_msg->cmd) {
                    case INIT:
                        init_flag_left = 1;
                        break;
                    case UPDATE_PARAMETER:
//                        mRedTOGGLE;
                        coeff_left[0] = rx_msg->coeff_1;
                        coeff_left[1] = rx_msg->coeff_2;
                        coeff_left[2] = rx_msg->coeff_3;
                        coeff_left[3] = rx_msg->coeff_4;
                        coeff_left[4] = rx_msg->coeff_5;
                        coeff_left[5] = rx_msg->coeff_6;
                        duration_left = rx_msg->duration;
                        p_left.updateCofficient(coeff_left);
                        startTime_left = 0;
                        endTime_left = startTime_left + duration_left;
                        trajectory_left.updateTrajectory(startTime_left, endTime_left, sectionNum_left, &p_left);
                        trajectory_left.GeneratePositionTrajectory(path_left);
                        trajectory_left.GenerateVelocityTrajectory(velocity_left);
                        timer_counter_left = (endTime_left - startTime_left) / sectionNum_left / 0.01;
                        update_times_left = sectionNum_left;
                        left_wheel_stateGoal.Position = path_left[0];
                        left_wheel_stateGoal.Speed = velocity_left[0];
                        left_wheel_control.setStateGoal(left_wheel_stateGoal);
                        counter_left = 0;
                        index_position_left = 0;
                        init_flag_left = 0;
//                        start_control_speed_flag_left = 0;
//                        start_control_flag_left = 1;
                        global_flag_left = 1;
                        break;
                    case SEND_PARAMETER:
                        MsgTrajectoryPositionTX tx_msg;
                        tx_msg.coeff_1 = coeff_left[0];
                        tx_msg.coeff_2 = coeff_left[1];
                        tx_msg.coeff_3 = coeff_left[2];
                        tx_msg.coeff_4 = coeff_left[3];
                        tx_msg.coeff_5 = coeff_left[4];
                        tx_msg.coeff_6 = coeff_left[5];
                        tx_msg.duration = duration_left;
                        usb.SendPacket(MsgTrajectoryPositionTXType, (uint8_t*)&tx_msg, sizeof(tx_msg));
                        is_data = 1;
                        break;
                    }
                    break;
                case RIGHT_DOF:
                    switch (rx_msg->cmd) {
                    case INIT:
                        init_flag_right = 1;
                        break;
                    case UPDATE_PARAMETER:
//                        mRedTOGGLE;
                        coeff_right[0] = rx_msg->coeff_1;
                        coeff_right[1] = rx_msg->coeff_2;
                        coeff_right[2] = rx_msg->coeff_3;
                        coeff_right[3] = rx_msg->coeff_4;
                        coeff_right[4] = rx_msg->coeff_5;
                        coeff_right[5] = rx_msg->coeff_6;
                        duration_right = rx_msg->duration;
                        p_right.updateCofficient(coeff_right);
                        startTime_right = 0;
                        endTime_right = startTime_right + duration_right;
                        trajectory_right.updateTrajectory(startTime_right, endTime_right, sectionNum_right, &p_right);
                        trajectory_right.GeneratePositionTrajectory(path_right);
                        trajectory_right.GenerateVelocityTrajectory(velocity_right);
                        timer_counter_right = (endTime_right - startTime_right) / sectionNum_right / 0.01;
                        update_times_right = sectionNum_right;
                        right_wheel_stateGoal.Position = path_right[0];
                        right_wheel_stateGoal.Speed = velocity_right[0];
                        right_wheel_control.setStateGoal(right_wheel_stateGoal);
                        counter_right = 0;
                        index_position_right = 0;
                        init_flag_right = 0;
//                        start_control_speed_flag_right = 0;
//                        start_control_flag_right = 1;
                        global_flag_right = 1;
                        break;
                    case SEND_PARAMETER:
                        MsgTrajectoryPositionTX tx_msg;
                        tx_msg.coeff_1 = coeff_right[0];
                        tx_msg.coeff_2 = coeff_right[1];
                        tx_msg.coeff_3 = coeff_right[2];
                        tx_msg.coeff_4 = coeff_right[3];
                        tx_msg.coeff_5 = coeff_right[4];
                        tx_msg.coeff_6 = coeff_right[5];
                        tx_msg.duration = duration_right;
                        usb.SendPacket(MsgTrajectoryPositionTXType, (uint8_t*)&tx_msg, sizeof(tx_msg));
                        is_data = 1;
                        break;
                    }
                    break;
                case PAN_DOF:
                    switch (rx_msg->cmd) {
                    case INIT:
                        init_flag_pan = 1;
                        tilt_wheel_stateGoal.Position = tilt_wheel_currentState.Position;
                        tilt_wheel_stateGoal.Speed = tilt_wheel_currentState.Speed;
                        break;
                    case UPDATE_PARAMETER:
//                        mRedTOGGLE;
                        coeff_pan[0] = rx_msg->coeff_1;
                        coeff_pan[1] = rx_msg->coeff_2;
                        coeff_pan[2] = rx_msg->coeff_3;
                        coeff_pan[3] = rx_msg->coeff_4;
                        coeff_pan[4] = rx_msg->coeff_5;
                        coeff_pan[5] = rx_msg->coeff_6;
                        duration_pan = rx_msg->duration;
                        p_pan.updateCofficient(coeff_pan);
                        startTime_pan = 0;
                        endTime_pan = startTime_pan + duration_pan;
                        trajectory_pan.updateTrajectory(startTime_pan, endTime_pan, sectionNum_pan, &p_pan);
                        trajectory_pan.GeneratePositionTrajectory(path_pan);
                        trajectory_pan.GenerateVelocityTrajectory(velocity_pan);
                        timer_counter_pan = (endTime_pan - startTime_pan) / sectionNum_pan / 0.01;
                        update_times_pan = sectionNum_pan;
                        pan_wheel_stateGoal.Position = path_pan[0];
                        pan_wheel_stateGoal.Speed = velocity_pan[0];
                        tilt_wheel_stateGoal.Position = tilt_wheel_currentState.Position;
                        tilt_wheel_stateGoal.Speed = 0;
                        wheel_control_pan_tilt.setStateGoal(pan_wheel_stateGoal, tilt_wheel_stateGoal);
                        counter_pan = 0;
                        index_position_pan = 0;
                        init_flag_pan = 0;
//                        start_control_speed_flag_pan = 0;
//                        start_control_flag_pan = 1;
                        global_flag_pan = 1;
                        break;
                    case SEND_PARAMETER:
                        MsgTrajectoryPositionTX tx_msg;
                        tx_msg.coeff_1 = coeff_pan[0];
                        tx_msg.coeff_2 = coeff_pan[1];
                        tx_msg.coeff_3 = coeff_pan[2];
                        tx_msg.coeff_4 = coeff_pan[3];
                        tx_msg.coeff_5 = coeff_pan[4];
                        tx_msg.coeff_6 = coeff_pan[5];
                        tx_msg.duration = duration_pan;
                        usb.SendPacket(MsgTrajectoryPositionTXType, (uint8_t*)&tx_msg, sizeof(tx_msg));
                        is_data = 1;
                        break;
                    }
                    break;
                case TILT_DOF:
                    switch (rx_msg->cmd) {
                    case INIT:
                        init_flag_tilt = 1;
                        pan_wheel_stateGoal.Position = pan_wheel_currentState.Position;
                        pan_wheel_stateGoal.Speed = pan_wheel_currentState.Speed;
                        break;
                    case UPDATE_PARAMETER:
//                        mRedTOGGLE;
                        coeff_tilt[0] = rx_msg->coeff_1;
                        coeff_tilt[1] = rx_msg->coeff_2;
                        coeff_tilt[2] = rx_msg->coeff_3;
                        coeff_tilt[3] = rx_msg->coeff_4;
                        coeff_tilt[4] = rx_msg->coeff_5;
                        coeff_tilt[5] = rx_msg->coeff_6;
                        duration_tilt = rx_msg->duration;
                        p_tilt.updateCofficient(coeff_tilt);
                        startTime_tilt = 0;
                        endTime_tilt = startTime_tilt + duration_tilt;
                        trajectory_tilt.updateTrajectory(startTime_tilt, endTime_tilt, sectionNum_tilt, &p_tilt);
                        trajectory_tilt.GeneratePositionTrajectory(path_tilt);
                        trajectory_tilt.GenerateVelocityTrajectory(velocity_tilt);
                        timer_counter_tilt = (endTime_tilt - startTime_tilt) / sectionNum_tilt / 0.01;
                        update_times_tilt = sectionNum_tilt;
                        tilt_wheel_stateGoal.Position = path_tilt[0];
                        tilt_wheel_stateGoal.Speed = velocity_tilt[0];
                        pan_wheel_stateGoal.Position = pan_wheel_currentState.Position;
                        pan_wheel_stateGoal.Speed = 0;
                        wheel_control_pan_tilt.setStateGoal(pan_wheel_stateGoal, tilt_wheel_stateGoal);
                        counter_tilt = 0;
                        index_position_tilt = 0;
                        init_flag_tilt = 0;
//                        start_control_speed_flag_tilt = 0;
//                        start_control_flag_tilt = 1;
                        global_flag_tilt = 1;
                        break;
                    case SEND_PARAMETER:
                        MsgTrajectoryPositionTX tx_msg;
                        tx_msg.coeff_1 = coeff_tilt[0];
                        tx_msg.coeff_2 = coeff_tilt[1];
                        tx_msg.coeff_3 = coeff_tilt[2];
                        tx_msg.coeff_4 = coeff_tilt[3];
                        tx_msg.coeff_5 = coeff_tilt[4];
                        tx_msg.coeff_6 = coeff_tilt[5];
                        tx_msg.duration = duration_tilt;
                        usb.SendPacket(MsgTrajectoryPositionTXType, (uint8_t*)&tx_msg, sizeof(tx_msg));
                        is_data = 1;
                        break;
                    }
                    break;
                }
            }
            else if(type == MsgTrajectoryVelocityType) {
                MsgTrajectoryVelocity *rx_msg = ((MsgTrajectoryVelocity*) (rx_data+1));
                switch (rx_msg->face_id) {
                case LEFT_DOF:
                    switch (rx_msg->cmd) {
                    case UPDATE_PARAMETER:
//                        mRedTOGGLE;
                        coeff_speed_left[0] = rx_msg->coeff_1;
                        coeff_speed_left[1] = rx_msg->coeff_2;
                        p_speed_left.updateCofficient(coeff_speed_left);
                        trajectory_speed_left.updateTrajectory(positionNum_left, min_pos, max_pos, &p_speed_left);
                        trajectory_speed_left.GeneratePositionVelocityTrajectory(path_speed_left, &velocity_speed_left);
                        timer_counter_speed_left = (max_pos - min_pos)/(abs(velocity_speed_left)*0.01*positionNum_left) + 0.5;
                        left_wheel_stateGoal.Position = path_speed_left[0];
                        left_wheel_stateGoal.Speed = velocity_speed_left;
                        left_wheel_control.setStateGoal(left_wheel_stateGoal);
                        counter_speed_left = 0;
                        index_speed_left = 0;
                        break;
                    case BEGIN:
                        init_flag_left = 0;
//                        start_control_flag_left = 0;
//                        start_control_speed_flag_left = 1;
                        global_flag_speed_left = 1;
                        break;
                    case STOP:
//                        start_control_speed_flag_left = 0;
                        global_flag_speed_left = 0;
                        break;
                    case SEND_PARAMETER:
                        MsgTrajectoryVelocityTX tx_msg;
                        tx_msg.coeff_1 = coeff_speed_left[0];
                        tx_msg.coeff_2 = coeff_speed_left[1];
                        usb.SendPacket(MsgTrajectoryVelocityTXType, (uint8_t*)&tx_msg, sizeof(tx_msg));
                        is_data = 1;
                        break;
                    }
                    break;
                case RIGHT_DOF:
                    switch (rx_msg->cmd) {
                    case UPDATE_PARAMETER:
//                        mRedTOGGLE;
                        coeff_speed_right[0] = rx_msg->coeff_1;
                        coeff_speed_right[1] = rx_msg->coeff_2;
                        p_speed_right.updateCofficient(coeff_speed_right);
                        trajectory_speed_right.updateTrajectory(positionNum_right, min_pos, max_pos, &p_speed_right);
                        trajectory_speed_right.GeneratePositionVelocityTrajectory(path_speed_right, &velocity_speed_right);
                        timer_counter_speed_right = (max_pos - min_pos)/(abs(velocity_speed_right)*0.01*positionNum_right) + 0.5;
                        right_wheel_stateGoal.Position = path_speed_right[0];
                        right_wheel_stateGoal.Speed = velocity_speed_right;
                        right_wheel_control.setStateGoal(right_wheel_stateGoal);
                        counter_speed_right = 0;
                        index_speed_right = 0;
                        break;
                    case BEGIN:
                        init_flag_right = 0;
//                        start_control_flag_right = 0;
//                        start_control_speed_flag_right = 1;
                        global_flag_speed_right = 1;
                        break;
                    case STOP:
//                        start_control_speed_flag_right = 0;
                        global_flag_speed_right = 0;
                        break;
                    case SEND_PARAMETER:
                        MsgTrajectoryVelocityTX tx_msg;
                        tx_msg.coeff_1 = coeff_speed_right[0];
                        tx_msg.coeff_2 = coeff_speed_right[1];
                        usb.SendPacket(MsgTrajectoryVelocityTXType, (uint8_t*)&tx_msg, sizeof(tx_msg));
                        is_data = 1;
                        break;
                    }
                    break;
                case PAN_DOF:
                    switch (rx_msg->cmd) {
                    case UPDATE_PARAMETER:
//                        mRedTOGGLE;
                        coeff_speed_pan[0] = rx_msg->coeff_1;
                        coeff_speed_pan[1] = rx_msg->coeff_2;
                        p_speed_pan.updateCofficient(coeff_speed_pan);
                        trajectory_speed_pan.updateTrajectory(positionNum_pan, min_pos, max_pos, &p_speed_pan);
                        trajectory_speed_pan.GeneratePositionVelocityTrajectory(path_speed_pan, &velocity_speed_pan);
                        timer_counter_speed_pan = (max_pos - min_pos)/(abs(velocity_speed_pan)*0.01*positionNum_pan) + 0.5;
                        pan_wheel_stateGoal.Position = path_speed_pan[0];
                        pan_wheel_stateGoal.Speed = velocity_speed_pan;
                        tilt_wheel_stateGoal.Position = tilt_wheel_currentState.Position;
                        tilt_wheel_stateGoal.Speed = 0;
                        wheel_control_pan_tilt.setStateGoal(pan_wheel_stateGoal, tilt_wheel_stateGoal);
                        counter_speed_pan = 0;
                        index_speed_pan = 0;
                        break;
                    case BEGIN:
                        init_flag_pan = 0;
//                        start_control_flag_pan = 0;
//                        start_control_speed_flag_pan = 1;
                        global_flag_speed_pan = 1;
                        break;
                    case STOP:
//                        start_control_speed_flag_pan = 0;
                        global_flag_speed_pan = 0;
                        break;
                    case SEND_PARAMETER:
                        MsgTrajectoryVelocityTX tx_msg;
                        tx_msg.coeff_1 = coeff_speed_pan[0];
                        tx_msg.coeff_2 = coeff_speed_pan[1];
                        usb.SendPacket(MsgTrajectoryVelocityTXType, (uint8_t*)&tx_msg, sizeof(tx_msg));
                        is_data = 1;
                        break;
                    }
                    break;
                }
            }
            usb.DropPacket();
        }
        if(is_data) {
            is_data = 0;
            usb.SendNow();
        }

        if (init_flag_left)
        {
            left_wheel_stateGoal.Position = 0;
            left_wheel_stateGoal.Speed = 0;
            left_wheel_control.setStateGoal(left_wheel_stateGoal);
            cmd = GET_STATES;
            mBusReadBurst(slave_addr_left, cmd, 7, data_left, 1);

            if (data_left[6]==0xAA)
            {
                start_flag_left = 1;
                mYellowTOGGLE;
                current_velocity_left = data_left[0] + data_left[1] * 256 + data_left[2] * 65536 + data_left[3] * 16777216;
                current_position_left = data_left[4] + data_left[5] * 256;
                left_wheel_currentState.Position = current_position_left;
                left_wheel_currentState.Speed = current_velocity_left;
                left_wheel_control.updateState(left_wheel_currentState);
                left_wheel_cmd = left_wheel_control.updateStateCmd(&usb);
            }
        }
        else
        // if (start_control_flag_left || start_control_speed_flag_left)
        {
//            mRedTOGGLE;
            left_wheel_control.setStateGoal(left_wheel_stateGoal);
            cmd = GET_STATES;
            mBusReadBurst(slave_addr_left, cmd, 7, data_left, 1);

            if (data_left[6]==0xAA)
            {
                start_flag_left = 1;
                mYellowTOGGLE;
                current_velocity_left = data_left[0] + data_left[1] * 256 + data_left[2] * 65536 + data_left[3] * 16777216;
                current_position_left = data_left[4] + data_left[5] * 256;
                left_wheel_currentState.Position = current_position_left;
                left_wheel_currentState.Speed = current_velocity_left;
                left_wheel_control.updateState(left_wheel_currentState);
                left_wheel_cmd = left_wheel_control.updateStateCmd(&usb);
            }
        }
        
        // if ((start_control_speed_flag_left == 0) && (init_flag_left == 0))
        // {
        //     left_wheel_currentState.Position = left_wheel_stateGoal.Position;
        //     left_wheel_currentState.Speed = left_wheel_stateGoal.Speed;
        //     left_wheel_control.updateState(left_wheel_currentState);
        //     left_wheel_cmd = left_wheel_control.updateStateCmd(&usb);
        // }
        if (init_flag_right)
        {
            right_wheel_stateGoal.Position = 0;
            right_wheel_stateGoal.Speed = 0;
            right_wheel_control.setStateGoal(right_wheel_stateGoal);
            cmd = GET_STATES;
            mBusReadBurst(slave_addr_right, cmd, 7, data_right, 1);

            if (data_right[6]==0xAA)
            {
                start_flag_right = 1;
                mYellowTOGGLE;
                current_velocity_right = data_right[0] + data_right[1] * 256 + data_right[2] * 65536 + data_right[3] * 16777216;
                current_position_right = data_right[4] + data_right[5] * 256;
                right_wheel_currentState.Position = current_position_right;
                right_wheel_currentState.Speed = current_velocity_right;
                right_wheel_control.updateState(right_wheel_currentState);
                right_wheel_cmd = right_wheel_control.updateStateCmd(&usb);
            }
        }
        else
        {
//            mRedTOGGLE;
            right_wheel_control.setStateGoal(right_wheel_stateGoal);
            cmd = GET_STATES;
            mBusReadBurst(slave_addr_right, cmd, 7, data_right, 1);

            if (data_right[6]==0xAA)
            {
                start_flag_right = 1;
                mYellowTOGGLE;
                current_velocity_right = data_right[0] + data_right[1] * 256 + data_right[2] * 65536 + data_right[3] * 16777216;
                current_position_right = data_right[4] + data_right[5] * 256;
                right_wheel_currentState.Position = current_position_right;
                right_wheel_currentState.Speed = current_velocity_right;
                right_wheel_control.updateState(right_wheel_currentState);
                right_wheel_cmd = right_wheel_control.updateStateCmd(&usb);
            }
        }
        if (init_flag_pan)
        {
            pan_wheel_stateGoal.Position = 0;
            pan_wheel_stateGoal.Speed = 0;
            wheel_control_pan_tilt.setStateGoal(pan_wheel_stateGoal, tilt_wheel_stateGoal);
            cmd = GET_STATES;
            mBusReadBurst(slave_addr_pan, cmd, 7, data_pan, 1);

            if (data_pan[6]==0xAA)
            {
                pan_ready_flag = 1;
            }

            if (pan_ready_flag && tilt_ready_flag && (data_pan[6]==0xAA))
            {
//                pan_ready_flag = 0;
                start_flag_pan = 1;
                mYellowTOGGLE;
                current_velocity_pan = data_pan[0] + data_pan[1] * 256 + data_pan[2] * 65536 + data_pan[3] * 16777216;
                current_position_pan = data_pan[4] + data_pan[5] * 256;
                pan_wheel_currentState.Position = current_position_pan;
                pan_wheel_currentState.Speed = current_velocity_pan;
                wheel_control_pan_tilt.updateState(pan_wheel_currentState, tilt_wheel_currentState);
                wheel_control_pan_tilt.updateStateCmd(&left_motor_cmd, &right_motor_cmd, &usb);
            }
        }
        else
        {
//            mRedTOGGLE;
            wheel_control_pan_tilt.setStateGoal(pan_wheel_stateGoal, tilt_wheel_stateGoal);
            cmd = GET_STATES;
            mBusReadBurst(slave_addr_pan, cmd, 7, data_pan, 1);

            if (data_pan[6]==0xAA)
            {
                pan_ready_flag = 1;
            }

            if (pan_ready_flag && tilt_ready_flag && (data_pan[6]==0xAA))
            {
//                pan_ready_flag = 0;
                start_flag_pan = 1;
                mYellowTOGGLE;
                current_velocity_pan = data_pan[0] + data_pan[1] * 256 + data_pan[2] * 65536 + data_pan[3] * 16777216;
                current_position_pan = data_pan[4] + data_pan[5] * 256;
                pan_wheel_currentState.Position = current_position_pan;
                pan_wheel_currentState.Speed = current_velocity_pan;
                wheel_control_pan_tilt.updateState(pan_wheel_currentState, tilt_wheel_currentState);
                wheel_control_pan_tilt.updateStateCmd(&left_motor_cmd, &right_motor_cmd, &usb);
            }
        }
        if (init_flag_tilt)
        {
            tilt_wheel_stateGoal.Position = 0;
            tilt_wheel_stateGoal.Speed = 0;
            wheel_control_pan_tilt.setStateGoal(pan_wheel_stateGoal, tilt_wheel_stateGoal);
            cmd = GET_STATES;
            mBusReadBurst(slave_addr_tilt, cmd, 7, data_tilt, 1);

            if (data_tilt[6]==0xAA)
            {
                tilt_ready_flag = 1;
            }

            if (tilt_ready_flag && pan_ready_flag && (data_tilt[6]==0xAA))
            {
//                tilt_ready_flag = 0;
                start_flag_tilt = 1;
                mYellowTOGGLE;
                current_velocity_tilt = data_tilt[0] + data_tilt[1] * 256 + data_tilt[2] * 65536 + data_tilt[3] * 16777216;
                current_position_tilt = data_tilt[4] + data_tilt[5] * 256;
                tilt_wheel_currentState.Position = current_position_tilt;
                tilt_wheel_currentState.Speed = current_velocity_tilt;
                wheel_control_pan_tilt.updateState(pan_wheel_currentState, tilt_wheel_currentState);
                wheel_control_pan_tilt.updateStateCmd(&left_motor_cmd, &right_motor_cmd, &usb);
            }
        }
        else
        {
//            mRedTOGGLE;
            wheel_control_pan_tilt.setStateGoal(pan_wheel_stateGoal, tilt_wheel_stateGoal);
            cmd = GET_STATES;
            mBusReadBurst(slave_addr_tilt, cmd, 7, data_tilt, 1);

            if (data_tilt[6]==0xAA)
            {
                tilt_ready_flag = 1;
            }

            if (tilt_ready_flag && pan_ready_flag && (data_tilt[6]==0xAA))
            {
//                tilt_ready_flag = 0;
                start_flag_tilt = 1;
                mYellowTOGGLE;
                current_velocity_tilt = data_tilt[0] + data_tilt[1] * 256 + data_tilt[2] * 65536 + data_tilt[3] * 16777216;
                current_position_tilt = data_tilt[4] + data_tilt[5] * 256;
                tilt_wheel_currentState.Position = current_position_tilt;
                tilt_wheel_currentState.Speed = current_velocity_tilt;
                wheel_control_pan_tilt.updateState(pan_wheel_currentState, tilt_wheel_currentState);
                wheel_control_pan_tilt.updateStateCmd(&left_motor_cmd, &right_motor_cmd, &usb);
            }
        }
    }
    return(0);
}

void timerCallback1(void)
{
    if(global_flag_left && start_flag_left)
    {
        mBlueON;
        counter_left++;
        if(counter_left == timer_counter_left)
        {
            counter_left = 0;
            index_position_left++;
            if(index_position_left <= update_times_left)
            {
                left_wheel_stateGoal.Position = path_left[index_position_left];
                left_wheel_stateGoal.Speed = velocity_left[index_position_left];
            }
            else
            {
                index_position_left = 0;
                global_flag_left = 0;
                start_flag_left = 0;
                start_control_flag_left = 0;
                mBlueOFF;
            }
        }
    }

    if(global_flag_right && start_flag_right)
    {
        mBlueON;
        counter_right++;
        if(counter_right == timer_counter_right)
        {
            counter_right = 0;
            index_position_right++;
            if(index_position_right <= update_times_right)
            {
                right_wheel_stateGoal.Position = path_right[index_position_right];
                right_wheel_stateGoal.Speed = velocity_right[index_position_right];
            }
            else
            {
                index_position_right = 0;
                global_flag_right = 0;
                start_flag_right = 0;
                start_control_flag_right = 0;
                mBlueOFF;
            }
        }
    }
    if(global_flag_pan && start_flag_pan)
    {
        mBlueON;
        counter_pan++;
        if(counter_pan == timer_counter_pan)
        {
            counter_pan = 0;
            index_position_pan++;
            if(index_position_pan <= update_times_pan)
            {
                pan_wheel_stateGoal.Position = path_pan[index_position_pan];
                pan_wheel_stateGoal.Speed = velocity_pan[index_position_pan];
            }
            else
            {
                index_position_pan = 0;
                global_flag_pan = 0;
                start_flag_pan = 0;
                start_control_flag_pan = 0;
                mBlueOFF;
            }
        }
    }
    if(global_flag_tilt && start_flag_tilt)
    {
        mBlueON;
        counter_tilt++;
        if(counter_tilt == timer_counter_tilt)
        {
            counter_tilt = 0;
            index_position_tilt++;
            if(index_position_tilt <= update_times_tilt)
            {
                tilt_wheel_stateGoal.Position = path_tilt[index_position_tilt];
                tilt_wheel_stateGoal.Speed = velocity_tilt[index_position_tilt];
            }
            else
            {
                index_position_tilt = 0;
                global_flag_tilt = 0;
                start_flag_tilt = 0;
                start_control_flag_tilt = 0;
                mBlueOFF;
            }
        }
    }
//    mWhiteTOGGLE;
}

void timerCallback2(void)
{
    if(global_flag_speed_left && start_flag_left)
    {
        mBlueON;
        counter_speed_left++;
        if(counter_speed_left == timer_counter_speed_left)
        {
            counter_speed_left = 0;
            index_speed_left++;
            if(index_speed_left <= update_times_speed_left)
            {
                left_wheel_stateGoal.Position = path_speed_left[index_speed_left];
                left_wheel_stateGoal.Speed = velocity_speed_left;
            }
            else
            {
                index_speed_left = 0;
                start_flag_left = 1;
                global_flag_speed_left = 1;
                start_control_speed_flag_left = 0;
                mBlueOFF;
            }
        }
    }
    if(global_flag_speed_right && start_flag_right)
    {
        mBlueON;
        counter_speed_right++;
        if(counter_speed_right == timer_counter_speed_right)
        {
            counter_speed_right = 0;
            index_speed_right++;
            if(index_speed_right <= update_times_speed_right)
            {
                right_wheel_stateGoal.Position = path_speed_right[index_speed_right];
                right_wheel_stateGoal.Speed = velocity_speed_right;
            }
            else
            {
                index_speed_right = 0;
                start_flag_right = 1;
                global_flag_speed_right = 1;
                start_control_speed_flag_right = 0;
                mBlueOFF;
            }
        }
    }
    if(global_flag_speed_pan && start_flag_pan)
    {
        mBlueON;
        counter_speed_pan++;
        if(counter_speed_pan == timer_counter_speed_pan)
        {
            counter_speed_pan = 0;
            index_speed_pan++;
            if(index_speed_pan <= update_times_speed_pan)
            {
                pan_wheel_stateGoal.Position = path_speed_pan[index_speed_pan];
                pan_wheel_stateGoal.Speed = velocity_speed_pan;
            }
            else
            {
                index_speed_pan = 0;
                start_flag_pan = 1;
                global_flag_speed_pan = 1;
                start_control_speed_flag_pan = 0;
                mBlueOFF;
            }
        }
    }
//    mBlueTOGGLE;
    
}


