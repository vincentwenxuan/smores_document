/*
2015.6.3
Smores mother board main code
Location: Embedded/ecosystem/projects/smores_stm32



Overview:
This code is for uploading to the STM32F303XC in the motherboard of SMORES

(---what this code mainly do---)


receive command and execute
generate trajectories
PID control 
position control
torque control
speed control


Questions: 


using so much flag maybe not a good idea for readability(especially they are not commented)?? Is there other way?
Can the declaration of variables put outside as a file?
Can the different part be seperated into different files so that the main.cpp will not be so long. So that it can show the high level construction of this code 









*/




#include "common_peripherals.h"   // where ? 
#include "system_clock.h"         // ?? 
#include "mBus.h"             // TWX: location: ecosystem/stm32/c/user/inc

// common components 
#include "common_message_types.h"   // TWX: location: ecosystem/common/c/user/inc
// common messages
#include "usb_interface.h"    // TWX: location: ecosystem/common/cpp/user/inc
// usb interface
//#include "mBus.h"

// common libraries
#include "project_message_types.h"  // ??
#include "error_reporting.h"
#include <string.h> 
//needed for memset
//#define   mBUS 1

// user libraries
#include "Wheel_Control.h"   // TWX: ecosystem/smores_common/cpp/inc 
#include "Trajectory.h"      // TWX: ecosystem/smores_common/cpp/inc 

//#define TELECONTROL
//#define POSITION_CONTROL
//#define SPEED_CONTROL


//TWX: what's this for (in which part it was used)
// face ping request messages:
#define LEFT_FACE_PING   121
#define RIGHT_FACE_PING  122
#define TOP_FACE_PING    123
#define BOTTOM_FACE_PING 124

//TWX: what's this for (in which part it was used)
/*   face macros:   */
#define LEFT_FACE 21  // left
#define RIGHT_FACE 22  // right
#define TOP_FACE 23  // top
#define BOTTOM_FACE 24  // bottom

//TWX: address in I2C communication
/*   face i2c address macros:  */
#define BOTTOM_FACE_I2C_ADDRESS 0x0B
#define LEFT_FACE_I2C_ADDRESS   0x0A
#define RIGHT_FACE_I2C_ADDRESS  0x0C
#define TOP_FACE_I2C_ADDRESS    0x0D

//TWX: what's this for (in which part it was used)
/*   face board macros:   */
#define MAGNETS_ON  0xA2
#define MAGNETS_OFF 0xA3
#define FACE_PING   0xA5

//TWX: what's this for (in which part it was used)
// feecback cmd
#define GET_STATES 0xA4

#define UPDATE_GOAL 10
#define SEND_DATA   20


//TWX: these declaration of variables needs to be reorganized(which belongs to which part) and put outside

float path_left[501];
float velocity_left[501];    // max is 100s

float path_speed_left[165];
float velocity_speed_left;    // max is 40000


PID_State left_wheel_stateGoal; //TWX: Class definition in ecosystem/smores_common/cpp/inc/PID_control.h . this is for setting goal position and speed of a wheel
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

float path_right[501];
float velocity_right[501];    // min duration is 1.5

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

float path_pan[501];
float velocity_pan[501];    // min duration is 1.5

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

float path_tilt[501];
float velocity_tilt[501];    // min duration is 1.5

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

uint16_t handleFacePing(uint16_t cmd) {
/*  Handles a face name request by asking the face to return its name, and then 
    sending that information back to matlab over USB.
*/
    // Send a ping to a face, and receive data back:
    // Note: A left-shift of 1 (<< 1) appends a zero in the least significant bit
    //       of the address.  This indicates I2C read (as opposed to write).
    uint8_t data = 0xff;
    switch(cmd) {
    case LEFT_FACE_PING:
        //mBusReadBurst(0b01, FACE_PING, 1, &data, 1);
        data = mBusRead(LEFT_FACE_I2C_ADDRESS<<1, FACE_PING, 1);   // TWX: what <<1 means here?
        break;
    case RIGHT_FACE_PING:
        data = mBusRead(RIGHT_FACE_I2C_ADDRESS<<1, FACE_PING, 1);
        break;
    case TOP_FACE_PING:
        data = mBusRead(TOP_FACE_I2C_ADDRESS<<1, FACE_PING, 1);
        break;
    case BOTTOM_FACE_PING:
        data = mBusRead(BOTTOM_FACE_I2C_ADDRESS<<1, FACE_PING, 1);
        break;
    }
    return (uint16_t)data;
    // Send the face response back to Matlab:
    //MsgModule tx_msg;
    //tx_msg.data = (uint16_t)data;  //tx_msg.data is a uint16
    //usb.SendPacket(MsgModuleType, (uint8_t*)&tx_msg, sizeof(tx_msg));
    //mBlueTOGGLE;
}

int main (void){
    InitPeripherals();
    
    UsbInterface usb = UsbInterface();
    usb.Init();

    //XJ: magnet on-off command;
     
    char slave_cmd;

    // power on flag
    bool power_on_flag = 1;
    bool counter_left_power_on = 1;
    bool counter_right_power_on = 1;
    bool counter_pan_power_on = 1;
    bool counter_tilt_power_on = 1;
    
    
    // torque control flag
    bool torque_flag_left = 0;
    bool torque_flag_right = 0;
    bool torque_flag_pan_tilt = 0;
    
    // feedback control init part
    Motor left_wheel_motor;     //TWX: class definition in ecosystem/smores_common/cpp/inc/DC_Motor.h 
    Motor right_wheel_motor;
    Motor right_pan_tilt_motor;
    Motor left_pan_tilt_motor;

    //XJ: right inner motor init
    //TWX: definition in ecosystem/smores_common/cpp/inc/DC_Motor.h 
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
    
    //XJ: left inner motor init
   
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

    //XJ: right face motor init
    
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

    //XJ: left face motor init
    
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

    //XJ: motor min-max speed
    int8_t motor_cmd_min = -50;
    uint8_t motor_cmd_max = 50;

    //XJ: motor min-max position
    float min_pos = -32767;
    float max_pos = 32767;

    //XJ: left face motor PID control
    //TWX: PID controller definition in ecosystem/projects/smores_pid_control/stm32f303xc/inc/PID_Control.h

    float Kp = 0.018, Ki = 0, Kd = 0.0002;
    PID_Controller left_wheel_pid_controller(Kp, Ki, Kd, motor_cmd_min, motor_cmd_max);
    Wheel_Controller_OneMotor left_wheel_control(&left_wheel_pid_controller, left_wheel_motor);

    //XJ quintic trajectory init
    //TWX: Polynomial class definition Embedded/ecosystem/smores_common/cpp/inc/Polynomial.h
    float coeff_left[6] = {0,0,0,0,0,0};
    float duration_left = 0;
    Polynomial p_left(coeff_left, 6);
    float startTime_left = 0;
    float endTime_left = 0;
    uint16_t sectionNum_left = 500;
    Trajectory trajectory_left(startTime_left, endTime_left, sectionNum_left, &p_left);

    //XJ: quintic trajectory velocity init
    float coeff_speed_left[2] = {0,0};
    Polynomial p_speed_left(coeff_speed_left, 2);
    uint16_t positionNum_left = 164;
    Trajectory trajectory_speed_left(positionNum_left, min_pos, max_pos, &p_speed_left);
    trajectory_speed_left.GeneratePositionVelocityTrajectory(path_speed_left, &velocity_speed_left);
    update_times_speed_left = positionNum_left;
    
    //XJ: goal position velocity init
    left_wheel_stateGoal.Position = 0;
    left_wheel_stateGoal.Speed = 0;
    left_wheel_control.setStateGoal(left_wheel_stateGoal);

    PID_State left_wheel_currentState;

    char slave_addr_left = 0x14;
    uint8_t data_left[7] = {0,0,0,0,0,0,0};

    int16_t current_position_left = 0;
    int32_t current_velocity_left = 0;
    int8_t left_wheel_cmd = 0;

    //XJ: right face motor PID control
    PID_Controller right_wheel_pid_controller(Kp, Ki, Kd, motor_cmd_min, motor_cmd_max);
    Wheel_Controller_OneMotor right_wheel_control(&right_wheel_pid_controller, right_wheel_motor);

    //XJ: quintic trajectory init
    float coeff_right[6] = {0,0,0,0,0,0};
    float duration_right = 0;
    Polynomial p_right(coeff_right, 6);
    float startTime_right = 0;
    float endTime_right = 0;
    uint16_t sectionNum_right = 500;
    Trajectory trajectory_right(startTime_right, endTime_right, sectionNum_right, &p_right);

    //XJ: quintic trajectory velocity init
    float coeff_speed_right[2] = {0,0};
    Polynomial p_speed_right(coeff_speed_right, 2);
    uint16_t positionNum_right = 164;
    Trajectory trajectory_speed_right(positionNum_right, min_pos, max_pos, &p_speed_right);
    trajectory_speed_right.GeneratePositionVelocityTrajectory(path_speed_right, &velocity_speed_right);
    update_times_speed_right = positionNum_right;
    
    //XJ: goal position velocity init
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

    //XJ: pan quintic trajectory init
    float coeff_pan[6] = {0,0,0,0,0,0};
    float duration_pan = 0;
    Polynomial p_pan(coeff_pan, 6);
    float startTime_pan = 0;
    float endTime_pan = 0;
    uint16_t sectionNum_pan = 500;
    Trajectory trajectory_pan(startTime_pan, endTime_pan, sectionNum_pan, &p_pan);

    //XJ: pan quintic trajecoty velocity init
    float coeff_speed_pan[2] = {0,0};
    Polynomial p_speed_pan(coeff_speed_pan, 2);
    uint16_t positionNum_pan = 164;
    Trajectory trajectory_speed_pan(positionNum_pan, min_pos, max_pos, &p_speed_pan);
    trajectory_speed_pan.GeneratePositionVelocityTrajectory(path_speed_pan, &velocity_speed_pan);
    update_times_speed_pan = positionNum_pan;  //TWX:  ??
    
    //XJ: goal position velocity init
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
    uint16_t sectionNum_tilt = 500;
    Trajectory trajectory_tilt(startTime_tilt, endTime_tilt, sectionNum_tilt, &p_tilt);
    
    tilt_wheel_stateGoal.Position = 0;
    tilt_wheel_stateGoal.Speed = 0;

    wheel_control_pan_tilt.setStateGoal(pan_wheel_stateGoal, tilt_wheel_stateGoal);

    PID_State tilt_wheel_currentState;

    char slave_addr_tilt = 0x16;
    uint8_t data_tilt[7] = {0,0,0,0,0,0,0};

    int16_t current_position_tilt = 0;
    int32_t current_velocity_tilt = 0;




    //XJ: turn on the LEDS and turn off them when power on
    mWhiteON;mRedON;mBlueON;mYellowON;
    DelayMilliseconds(1000);
    mWhiteOFF;mRedOFF;mBlueOFF;mYellowOFF;

    //XJ: flag init
    start_flag_left = 0;
    start_flag_right = 0;
    start_flag_pan = 0;
    start_flag_tilt = 0;
    pan_ready_flag = 0;
    tilt_ready_flag = 0;
    char cmd;
    PlayTimebase();  //TWX: what is playtimebase??
    // feedback control part end //

    //XJ
    /**
    main loop. 
    Do the most part of the work
    Read the command and do the command
    **/
    while(1)
    {
//        mGreenTOGGLE;

        uint8_t *rx_data;
        uint8_t rx_length;
        uint8_t is_data = 0;

        //XJ: read the command from USB
        //TWX: to rx_data, the cmd type is defined by rx_data[0], which could be MsgCmdType MsgMagnetType MsgControlType MsgTrajectoryPositionType  MsgTrajectoryVelocityType MsgTorqueType indicating type of the message received
        usb.GetBytes();
        while(usb.PeekPacket(&rx_data, &rx_length)) {
            uint8_t type = rx_data[0];

            //XJ: send data to the buffer?
            //TWX: what is MsgCmdType? and where can we find it
            if(type == MsgCmdType) {
                mGreenTOGGLE;
                MsgCmd *rx_msg = ((MsgCmd*) (rx_data+1));
                if(rx_msg->cmd > 100) {
                    mYellowTOGGLE;
                    MsgModule tx_msg;
                    tx_msg.data = handleFacePing(rx_msg->cmd);
                    usb.SendPacket(MsgModuleType, (uint8_t*)&tx_msg, sizeof(tx_msg));
                    is_data = 1;
                }
            }

            //XJ: choose a face and set the magnet on or off
            else if (type == MsgMagnetType) {
                MsgMagnet *rx_msg = ((MsgMagnet*) (rx_data+1)); 
                switch (rx_msg->face) {
                case LEFT_FACE:
//                    mWhiteTOGGLE;
                    if (rx_msg->state)
                    {
                        slave_cmd = MAGNETS_ON;
                    }
                    else
                    {
                        slave_cmd = MAGNETS_OFF;
                    }
                    mBusWriteNoAdd(LEFT_FACE_I2C_ADDRESS << 1, slave_cmd, 1);
                    break;
                case RIGHT_FACE:
//                    mGreenTOGGLE;
                    if (rx_msg->state)
                    {
                        slave_cmd = MAGNETS_ON;
                    }
                    else
                    {
                        slave_cmd = MAGNETS_OFF;
                    }
                    mBusWriteNoAdd(RIGHT_FACE_I2C_ADDRESS << 1, slave_cmd, 1);
                    break;
                case TOP_FACE:
//                    mRedTOGGLE;
                    if (rx_msg->state)
                    {
                        slave_cmd = MAGNETS_ON;
                    }
                    else
                    {
                        slave_cmd = MAGNETS_OFF;
                    }
                    mBusWriteNoAdd(TOP_FACE_I2C_ADDRESS << 1, slave_cmd, 1);
                    break;
                case BOTTOM_FACE:
//                    mBlueTOGGLE;
                    if (rx_msg->state)
                    {
                        slave_cmd = MAGNETS_ON;
                    }
                    else
                    {
                        slave_cmd = MAGNETS_OFF;
                    }
                    mBusWriteNoAdd(BOTTOM_FACE_I2C_ADDRESS << 1, slave_cmd, 1);
                    break;
                }
            }

            //XJ: control mode
            // TWX: reads msgfrom 

            else if(type == MsgControlType) {
                MsgControl *rx_msg = ((MsgControl*) (rx_data+1));
                switch (rx_msg->face_id) {
                case LEFT_DOF:
                    switch (rx_msg->cmd) {
                    
                    //XJ: get the goal position and velocity.
                    case UPDATE_GOAL:
//                        mRedTOGGLE;
                        left_wheel_stateGoal.Position = rx_msg->goal_position;
                        left_wheel_stateGoal.Speed = rx_msg->goal_velocity;
                        break;
                    
                    //XJ: sent the goal postion velocity to the buffer.
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
                    
                    //XJ: get the goal position and velocity.
                    case UPDATE_GOAL:
//                        mRedTOGGLE;
                        right_wheel_stateGoal.Position = rx_msg->goal_position;
                        right_wheel_stateGoal.Speed = rx_msg->goal_velocity;
                        break;
                    
                    //XJ: sent the goal postion velocity to the buffer.
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
                    
                    //XJ: get the goal position and velocity.
                    case UPDATE_GOAL:
//                        mRedTOGGLE;
                        pan_wheel_stateGoal.Position = rx_msg->goal_position;
                        pan_wheel_stateGoal.Speed = rx_msg->goal_velocity;
                        break;
                    
                    //XJ: sent the goal postion velocity to the buffer.
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
                    
                    //XJ: get the goal position and velocity.
                    case UPDATE_GOAL:
//                        mRedTOGGLE;
                        tilt_wheel_stateGoal.Position = rx_msg->goal_position;
                        tilt_wheel_stateGoal.Speed = rx_msg->goal_velocity;
                        break;
                    
                    //XJ: sent the goal postion velocity to the buffer.    
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

            //XJ: do the trjectory, get the arrays of position of velocity.
            else if(type == MsgTrajectoryPositionType) {
                power_on_flag = 0;
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
                        timer_counter_left = 1;
                        sectionNum_left = (endTime_left - startTime_left) / (timer_counter_left * 0.01);
                        trajectory_left.updateTrajectory(startTime_left, endTime_left, sectionNum_left, &p_left);
                        trajectory_left.GeneratePositionTrajectory(path_left);
                        trajectory_left.GenerateVelocityTrajectory(velocity_left);
                        // timer_counter_left = (endTime_left - startTime_left) / sectionNum_left / 0.01;
                        update_times_left = sectionNum_left;
                        left_wheel_stateGoal.Position = path_left[0];
                        left_wheel_stateGoal.Speed = velocity_left[0];
                        left_wheel_control.setStateGoal(left_wheel_stateGoal);
                        counter_left = 0;
                        index_position_left = 0;
                        init_flag_left = 0;
//                        start_control_speed_flag_left = 0;
//                        start_control_flag_left = 1;
                        torque_flag_left = 0;
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
                        timer_counter_right = 1;
                        sectionNum_right = (endTime_right - startTime_right) / (timer_counter_right * 0.01);
                        trajectory_right.updateTrajectory(startTime_right, endTime_right, sectionNum_right, &p_right);
                        trajectory_right.GeneratePositionTrajectory(path_right);
                        trajectory_right.GenerateVelocityTrajectory(velocity_right);
                        // timer_counter_right = (endTime_right - startTime_right) / sectionNum_right / 0.01;
                        update_times_right = sectionNum_right;
                        right_wheel_stateGoal.Position = path_right[0];
                        right_wheel_stateGoal.Speed = velocity_right[0];
                        right_wheel_control.setStateGoal(right_wheel_stateGoal);
                        counter_right = 0;
                        index_position_right = 0;
                        init_flag_right = 0;
//                        start_control_speed_flag_right = 0;
//                        start_control_flag_right = 1;
                        torque_flag_right = 0;
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
                        timer_counter_pan = 1;
                        sectionNum_pan = (endTime_pan - startTime_pan) / (timer_counter_pan * 0.01);
                        trajectory_pan.updateTrajectory(startTime_pan, endTime_pan, sectionNum_pan, &p_pan);
                        trajectory_pan.GeneratePositionTrajectory(path_pan);
                        trajectory_pan.GenerateVelocityTrajectory(velocity_pan);
                        // timer_counter_pan = (endTime_pan - startTime_pan) / sectionNum_pan / 0.01;
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
                        torque_flag_pan_tilt = 0;
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
                        timer_counter_tilt = 1;
                        sectionNum_tilt = (endTime_tilt - startTime_tilt) / (timer_counter_tilt * 0.01);
                        trajectory_tilt.updateTrajectory(startTime_tilt, endTime_tilt, sectionNum_tilt, &p_tilt);
                        trajectory_tilt.GeneratePositionTrajectory(path_tilt);
                        trajectory_tilt.GenerateVelocityTrajectory(velocity_tilt);
                        // timer_counter_tilt = (endTime_tilt - startTime_tilt) / sectionNum_tilt / 0.01;
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
                        torque_flag_pan_tilt= 0;
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

            //XJ: do the trajectory velocity
            else if(type == MsgTrajectoryVelocityType) {
                power_on_flag = 0;
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
                        torque_flag_left = 0;
                        global_flag_speed_left = 1;
                        break;
                    case STOP:
//                        start_control_speed_flag_left = 0;
                        global_flag_speed_left = 0;
                        left_wheel_stateGoal.Speed = 0;
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
                        torque_flag_right = 0;
                        global_flag_speed_right = 1;
                        break;
                    case STOP:
//                        start_control_speed_flag_right = 0;
                        global_flag_speed_right = 0;
                        right_wheel_stateGoal.Speed = 0;
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
                        torque_flag_pan_tilt = 0;
                        global_flag_speed_pan = 1;
                        break;
                    case STOP:
//                        start_control_speed_flag_pan = 0;
                        global_flag_speed_pan = 0;
                        pan_wheel_stateGoal.Speed = 0;
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

            //XJ: set the direction and velocity of the motor.
            //follow the trajectory?
            else if (type == MsgTorqueType) {
                power_on_flag = 0;
                MsgTorque *rx_msg = ((MsgTorque*) (rx_data+1));
                switch (rx_msg->DoF) {
                    case LEFT_DOF:
                        torque_flag_left = 1;
                        init_flag_left = 0;
                        if (rx_msg->direction) {
                            set_motor_direction(left_wheel_motor, Bit_RESET);
                        } else {
                            set_motor_direction(left_wheel_motor, Bit_SET);
                        }
                        set_motor_speed(left_wheel_motor, rx_msg->speed);
                        break;
                    case RIGHT_DOF:
                        torque_flag_right = 1;
                        init_flag_right = 0;
                        if (rx_msg->direction) {
                            set_motor_direction(right_wheel_motor, Bit_SET);
                        } else {
                            set_motor_direction(right_wheel_motor, Bit_RESET);
                        }
                        set_motor_speed(right_wheel_motor, rx_msg->speed);
                        break;
                    case TILT_DOF:
                        torque_flag_pan_tilt = 1;
                        init_flag_tilt = 0;
                        if (rx_msg->direction)
                        {
                            set_motor_direction(left_pan_tilt_motor, Bit_SET);
                            set_motor_direction(right_pan_tilt_motor, Bit_RESET);
                        }
                        else
                        {
                            set_motor_direction(left_pan_tilt_motor, Bit_RESET);
                            set_motor_direction(right_pan_tilt_motor, Bit_SET);
                        }
                        set_motor_speed(left_pan_tilt_motor, rx_msg->speed);
                        set_motor_speed(right_pan_tilt_motor, rx_msg->speed);
                        break;
                    case PAN_DOF:
                        torque_flag_pan_tilt = 1;
                        init_flag_pan = 0;
                        if (rx_msg->direction)
                        {
                            set_motor_direction(left_pan_tilt_motor, Bit_SET);
                            set_motor_direction(right_pan_tilt_motor, Bit_SET);
                        }
                        else
                        {
                            set_motor_direction(left_pan_tilt_motor, Bit_RESET);
                            set_motor_direction(right_pan_tilt_motor, Bit_RESET);
                        }
                        set_motor_speed(left_pan_tilt_motor, rx_msg->speed);
                        set_motor_speed(right_pan_tilt_motor, rx_msg->speed);
                        break;
                }
            }
            usb.DropPacket();
        }

        //XJ: if the buffer is filled with data, send them.
        if(is_data) {
            is_data = 0;
            usb.SendNow();
        }

        //XJ: when the power is on, do the following things
        //This section is the first part the SMORES will do when power on.
        /**
        Get the current state when power on, then set the motor to the current state.
        The position is held everytime we power on the SMORES.
        **/
        if (power_on_flag)
        {
            cmd = GET_STATES;  
            mBusReadBurst(slave_addr_left, cmd, 7, data_left, 1); // TWX: communication with other faceboard, what specifically(check the header)??
            if (data_left[6]==0xAA)  //TWX: what does this do? means available?
            {
                //XJ: get the current state
                mYellowTOGGLE;
                current_velocity_left = data_left[0] + (data_left[1] << 8) + (data_left[2] << 16) + (data_left[3] << 24);
                current_position_left = data_left[4] + (data_left[5] << 8);
                left_wheel_currentState.Position = current_position_left;
                left_wheel_currentState.Speed = current_velocity_left;
                
                //XJ: set the goal state to the current state.
                if (counter_left_power_on)
                {
                    left_wheel_stateGoal.Position = current_position_left;
                    left_wheel_stateGoal.Speed = 0;
                    left_wheel_control.setStateGoal(left_wheel_stateGoal);
                    counter_left_power_on = 0;
                }
                left_wheel_control.updateState(left_wheel_currentState);
                left_wheel_cmd = left_wheel_control.updateStateCmd(&usb);
            }
            mBusReadBurst(slave_addr_right, cmd, 7, data_right, 1);
            if (data_right[6]==0xAA)
            {
                //XJ: get the current state
                mYellowTOGGLE;
                current_velocity_right = data_right[0] + (data_right[1] << 8) + (data_right[2] << 16)  + (data_right[3] << 24);
                current_position_right = data_right[4] + (data_right[5] << 8);
                right_wheel_currentState.Position = current_position_right;
                right_wheel_currentState.Speed = current_velocity_right;
                
                //XJ: set the goal state to the current state.
                if (counter_right_power_on)
                {
                    right_wheel_stateGoal.Position = current_position_right;
                    right_wheel_stateGoal.Speed = 0;
                    right_wheel_control.setStateGoal(right_wheel_stateGoal);
                    counter_right_power_on = 0;
                }
                right_wheel_control.updateState(right_wheel_currentState);
                right_wheel_cmd = right_wheel_control.updateStateCmd(&usb);
            }
            
            mBusReadBurst(slave_addr_pan, cmd, 7, data_pan, 1);
            if (data_pan[6]==0xAA)
            {
                pan_ready_flag = 1;
            }
            mBusReadBurst(slave_addr_tilt, cmd, 7, data_tilt, 1);
            if (data_tilt[6]==0xAA)
            {
                tilt_ready_flag = 1;
            }

            if (pan_ready_flag && tilt_ready_flag && (data_pan[6]==0xAA) && (data_tilt[6]==0xAA))  //TWX: when pan-tilt wheel are both ready
            {
                //XJ: get the current state
//                pan_ready_flag = 0;
                start_flag_pan = 1;
                start_flag_tilt = 1;
                mYellowTOGGLE;
                current_velocity_pan = data_pan[0] + (data_pan[1] << 8) + (data_pan[2] << 16) + (data_pan[3] << 24);
                current_position_pan = data_pan[4] + (data_pan[5] << 8);
                pan_wheel_currentState.Position = current_position_pan;
                pan_wheel_currentState.Speed = current_velocity_pan;
                current_velocity_tilt = data_tilt[0] + (data_tilt[1] << 8) + (data_tilt[2] << 16) + (data_tilt[3] << 24);
                current_position_tilt = data_tilt[4] + (data_tilt[5] << 8);
                tilt_wheel_currentState.Position = current_position_tilt;
                tilt_wheel_currentState.Speed = current_velocity_tilt;
                //XJ: set the goal state to the current state.
                if (counter_pan_power_on && counter_tilt_power_on)
                {
                    pan_wheel_stateGoal.Position = current_position_pan;
                    pan_wheel_stateGoal.Speed = 0;
                    wheel_control_pan_tilt.setStateGoal(pan_wheel_stateGoal, tilt_wheel_stateGoal);
                    counter_pan_power_on = 0;
                    tilt_wheel_stateGoal.Position = current_position_tilt;
                    tilt_wheel_stateGoal.Speed = 0;
                    wheel_control_pan_tilt.setStateGoal(pan_wheel_stateGoal, tilt_wheel_stateGoal);
                    counter_tilt_power_on = 0;
                    counter_pan_power_on = 0;
                }
                wheel_control_pan_tilt.updateState(pan_wheel_currentState, tilt_wheel_currentState);
                wheel_control_pan_tilt.updateStateCmd(&left_motor_cmd, &right_motor_cmd, &usb);
            }
            
//             if (tilt_ready_flag && pan_ready_flag && (data_tilt[6]==0xAA))
//             {
// //                tilt_ready_flag = 0;
//                 start_flag_tilt = 1;
//                 mYellowTOGGLE;
//                 current_velocity_tilt = data_tilt[0] + (data_tilt[1] << 8) + (data_tilt[2] << 16) + (data_tilt[3] << 24);
//                 current_position_tilt = data_tilt[4] + (data_tilt[5] << 8);
//                 tilt_wheel_currentState.Position = current_position_tilt;
//                 tilt_wheel_currentState.Speed = current_velocity_tilt;
//                 if (counter_tilt_power_on)
//                 {
//                     tilt_wheel_stateGoal.Position = current_position_tilt;
//                     tilt_wheel_stateGoal.Speed = current_velocity_tilt;
//                     wheel_control_pan_tilt.setStateGoal(pan_wheel_stateGoal, tilt_wheel_stateGoal);
//                     counter_tilt_power_on = 0;
//                 }
//                 wheel_control_pan_tilt.updateState(pan_wheel_currentState, tilt_wheel_currentState);
//                 wheel_control_pan_tilt.updateStateCmd(&left_motor_cmd, &right_motor_cmd, &usb);
//             }
        }

        //XJ: init the state when the flag is on.
        // Set the SMORES to the initial position.
        //TWX: ?? init_flag_left when commanded position, and commanded to init , this flag set to 1,otherwise 0. 
        //TWX: and when the flag is ON then that wheel will move to 0 position

        if (init_flag_left) 
        {
            left_wheel_stateGoal.Position = 0;
            left_wheel_stateGoal.Speed = 0;
            left_wheel_control.setStateGoal(left_wheel_stateGoal);
            cmd = GET_STATES;
            mBusReadBurst(slave_addr_left, cmd, 7, data_left, 1);

            //XJ: get the current state, so trajectory could be generated.
            if (data_left[6]==0xAA)
            {
                start_flag_left = 1;
                mYellowTOGGLE;
                current_velocity_left = data_left[0] + (data_left[1] << 8) + (data_left[2] << 16) + (data_left[3] << 24);
                current_position_left = data_left[4] + (data_left[5] << 8);
                left_wheel_currentState.Position = current_position_left;
                left_wheel_currentState.Speed = current_velocity_left;
                left_wheel_control.updateState(left_wheel_currentState);
                left_wheel_cmd = left_wheel_control.updateStateCmd(&usb);
            }
        }
        //TWX: power_on_flag: this is set to 1, when power on, and then all 0 at othertime.(to check if that is the "first run")
        //TWX: torque_flag_left: when receive torque control command, this is set to 1, otherwise 0

        else if ((torque_flag_left == 0) && (power_on_flag == 0)) //TWX: this means in torque control mode and not the "first run"(or power-on-run)
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
                current_velocity_left = data_left[0] + (data_left[1] << 8) + (data_left[2] << 16) + (data_left[3] << 24);
                current_position_left = data_left[4] + (data_left[5] << 8);
                left_wheel_currentState.Position = current_position_left;
                left_wheel_currentState.Speed = current_velocity_left;
                left_wheel_control.updateState(left_wheel_currentState);
                left_wheel_cmd = left_wheel_control.updateStateCmd(&usb);
            }
        }
        else   //TWX: this is when the left wheel is in mode that's NOT "init" or torque?? is it?
        {
            cmd = GET_STATES;
            mBusReadBurst(slave_addr_left, cmd, 7, data_left, 1);
            if (data_left[6]==0xAA)
            {
                mYellowTOGGLE;
                current_velocity_left = data_left[0] + (data_left[1] << 8) + (data_left[2] << 16) + (data_left[3] << 24);
                current_position_left = data_left[4] + (data_left[5] << 8);
            }
        }
        
        // if ((start_control_speed_flag_left == 0) && (init_flag_left == 0))
        // {
        //     left_wheel_currentState.Position = left_wheel_stateGoal.Position;
        //     left_wheel_currentState.Speed = left_wheel_stateGoal.Speed;
        //     left_wheel_control.updateState(left_wheel_currentState);
        //     left_wheel_cmd = left_wheel_control.updateStateCmd(&usb);
        // }
       
        //TWX: this part is the same as left-wheel control above. they are right-pan-tilt control
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
                current_velocity_right = data_right[0] + (data_right[1] << 8) + (data_right[2] << 16)  + (data_right[3] << 24);
                current_position_right = data_right[4] + (data_right[5] << 8);
                right_wheel_currentState.Position = current_position_right;
                right_wheel_currentState.Speed = current_velocity_right;
                right_wheel_control.updateState(right_wheel_currentState);
                right_wheel_cmd = right_wheel_control.updateStateCmd(&usb);
            }
        }
        else if ((torque_flag_right == 0) && (power_on_flag == 0))
        {
//            mRedTOGGLE;
            right_wheel_control.setStateGoal(right_wheel_stateGoal);
            cmd = GET_STATES;
            mBusReadBurst(slave_addr_right, cmd, 7, data_right, 1);

            if (data_right[6]==0xAA)
            {
                start_flag_right = 1;
                mYellowTOGGLE;
                current_velocity_right = data_right[0] + (data_right[1] << 8) + (data_right[2] << 16)  + (data_right[3] << 24);
                current_position_right = data_right[4] + (data_right[5] << 8);
                right_wheel_currentState.Position = current_position_right;
                right_wheel_currentState.Speed = current_velocity_right;
                right_wheel_control.updateState(right_wheel_currentState);
                right_wheel_cmd = right_wheel_control.updateStateCmd(&usb);
            }
        }
        else
        {
            cmd = GET_STATES;
            mBusReadBurst(slave_addr_right, cmd, 7, data_right, 1);
            
            if (data_right[6]==0xAA)
            {
                mYellowTOGGLE;
                current_velocity_right = data_right[0] + (data_right[1] << 8) + (data_right[2] << 16) + (data_right[3] << 24);
                current_position_right = data_right[4] + (data_right[5] << 8);
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
                current_velocity_pan = data_pan[0] + (data_pan[1] << 8) + (data_pan[2] << 16) + (data_pan[3] << 24);
                current_position_pan = data_pan[4] + (data_pan[5] << 8);
                pan_wheel_currentState.Position = current_position_pan;
                pan_wheel_currentState.Speed = current_velocity_pan;
                wheel_control_pan_tilt.updateState(pan_wheel_currentState, tilt_wheel_currentState);
                wheel_control_pan_tilt.updateStateCmd(&left_motor_cmd, &right_motor_cmd, &usb);
            }
        }
        else if ((torque_flag_pan_tilt == 0) && (power_on_flag == 0))
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
                current_velocity_pan = data_pan[0] + (data_pan[1] << 8) + (data_pan[2] << 16) + (data_pan[3] << 24);
                current_position_pan = data_pan[4] + (data_pan[5] << 8);
                pan_wheel_currentState.Position = current_position_pan;
                pan_wheel_currentState.Speed = current_velocity_pan;
                wheel_control_pan_tilt.updateState(pan_wheel_currentState, tilt_wheel_currentState);
                wheel_control_pan_tilt.updateStateCmd(&left_motor_cmd, &right_motor_cmd, &usb);
            }
        }
        else
        {
            cmd = GET_STATES;
            mBusReadBurst(slave_addr_pan, cmd, 7, data_pan, 1);
            
            if (data_pan[6]==0xAA)
            {
                mYellowTOGGLE;
                current_velocity_pan = data_pan[0] + (data_pan[1] << 8) + (data_pan[2] << 16) + (data_pan[3] << 24);
                current_position_pan = data_pan[4] + (data_pan[5] << 8);
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
                current_velocity_tilt = data_tilt[0] + (data_tilt[1] << 8) + (data_tilt[2] << 16) + (data_tilt[3] << 24);
                current_position_tilt = data_tilt[4] + (data_tilt[5] << 8);
                tilt_wheel_currentState.Position = current_position_tilt;
                tilt_wheel_currentState.Speed = current_velocity_tilt;
                // tilt_wheel_currentState.Position = 0;
                // tilt_wheel_currentState.Speed = 0;
                wheel_control_pan_tilt.updateState(pan_wheel_currentState, tilt_wheel_currentState);
                wheel_control_pan_tilt.updateStateCmd(&left_motor_cmd, &right_motor_cmd, &usb);
            }
        }
        else if ((torque_flag_pan_tilt == 0) && (power_on_flag == 0))
        {
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
                current_velocity_tilt = data_tilt[0] + (data_tilt[1] << 8) + (data_tilt[2] << 16) + (data_tilt[3] << 24);
                current_position_tilt = data_tilt[4] + (data_tilt[5] << 8);
                tilt_wheel_currentState.Position = current_position_tilt;
                tilt_wheel_currentState.Speed = current_velocity_tilt;
                // tilt_wheel_currentState.Position = 0;
                // tilt_wheel_currentState.Speed = 0;
                wheel_control_pan_tilt.updateState(pan_wheel_currentState, tilt_wheel_currentState);
                wheel_control_pan_tilt.updateStateCmd(&left_motor_cmd, &right_motor_cmd, &usb);
            }
        }
        else
        {
            cmd = GET_STATES;
            mBusReadBurst(slave_addr_tilt, cmd, 7, data_tilt, 1);
            if (data_tilt[6]==0xAA)
            {
                mYellowTOGGLE;
                current_velocity_tilt = data_tilt[0] + (data_tilt[1] << 8) + (data_tilt[2] << 16) + (data_tilt[3] << 24);
                current_position_tilt = data_tilt[4] + (data_tilt[5] << 8);
            }
        }
        
    }
    return(0);
}



// TWX: is timercallback triggered every ? seconds? what is the mechanism of updating time with the trajectory?
void timerCallback1(void)   //TWX: This is for ????
{
    //TWX: global_flag_left: set to 1 when received command of position control
    //TWX: start_flag_left: ?????

    if(global_flag_left && start_flag_left)  
    {
        mBlueON;
        counter_left++;
        if(counter_left == timer_counter_left)
        {
            counter_left = 0;
            index_position_left++;
            if(index_position_left <= update_times_left)  //TWX: if it is still running set the goal position(come from trajectory planner) 
            {
                left_wheel_stateGoal.Position = path_left[index_position_left];
                left_wheel_stateGoal.Speed = velocity_left[index_position_left];
            }
            else  //TWX: what does this if else mean? is it for run or stop??
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

void timerCallback2(void)  //TWX: what is this callback for? speed control??
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



