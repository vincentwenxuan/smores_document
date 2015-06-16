//** Edited by Gabby Merritt 6/11/2015 **// 

//TODO: write/clean up Traj handlers // 


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

// #include "Wheel_Control.h"
// #include "Trajectory.h"

#include "stm32_cc3000.h"
#include "CC3000_socket.h"
#include "wifi.h"
#include "handlers.h"

//#define TELECONTROL
//#define POSITION_CONTROL
//#define SPEED_CONTROL



// variables for left wheel control
float path_left[501];
float velocity_left[501];    // max duration is 5s

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

// variables for right wheel control
float path_right[501];
float velocity_right[501];    // max duration is 5s

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

// variables for pan wheel control
float path_pan[501];
float velocity_pan[501];    // max duration is 5s

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

// variables for tilt control
float path_tilt[501];
float velocity_tilt[501];    // max duration is 5s

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



/*Declare global variables*/
char cc3000_device_name[] = "CC3000";

int main (void){
//****** Init Peripherals ******//
	InitPeripherals();
    
    debug_setup();
    cc3000_initialize_ip(100);
    cc3000_startup_check();
    
    udp_socket_communication(); //UDP socket communication
    
    mYellowON; mRedON; mGreenON; mWhiteON; mBlueON;
	DelayMilliseconds(100);
    mYellowOFF;mRedOFF;mGreenOFF; mWhiteOFF; mBlueOFF;

    
    UsbInterface usb = UsbInterface();
    usb.Init();

    char slave_cmd;

    //  ** power on flag ** //
    // initial values are 1 to indicate the module is on

    bool power_on_flag = 1;
    bool counter_left_power_on = 1;
    bool counter_right_power_on = 1;
    bool counter_pan_power_on = 1;
    bool counter_tilt_power_on = 1;
    
    
    // ** torque control flag ** //
    // initial values are 0 meaning OFF, 1 means turning on torque control mode

    bool torque_flag_left = 0;
    bool torque_flag_right = 0;
    bool torque_flag_pan_tilt = 0;
    
    // feecback control init part

// motor declaration and initialization

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

// controller initialization

    // min and max duty cycle of the pwm signal for motors

    int8_t motor_cmd_min = -50;
    uint8_t motor_cmd_max = 50;

    // min and max position value, -32767 is -180 degree and 32767 is 180 degree

    float min_pos = -32767;
    float max_pos = 32767;

    // controller for left wheel

    float Kp = 0.018, Ki = 0, Kd = 0.0002;
    PID_Controller left_wheel_pid_controller(Kp, Ki, Kd, motor_cmd_min, motor_cmd_max);
    Wheel_Controller_OneMotor left_wheel_control(&left_wheel_pid_controller, left_wheel_motor);

    // left wheel position control trajectory initialization

    float coeff_left[6] = {0,0,0,0,0,0};
    float duration_left = 0;
    Polynomial p_left(coeff_left, 6);
    float startTime_left = 0;
    float endTime_left = 0;
    uint16_t sectionNum_left = 500;
    Trajectory trajectory_left(startTime_left, endTime_left, sectionNum_left, &p_left);

    // left wheel speed control trajectory initialization

    float coeff_speed_left[2] = {0,0};
    Polynomial p_speed_left(coeff_speed_left, 2);
    uint16_t positionNum_left = 164;
    Trajectory trajectory_speed_left(positionNum_left, min_pos, max_pos, &p_speed_left);
    trajectory_speed_left.GeneratePositionVelocityTrajectory(path_speed_left, &velocity_speed_left);
    update_times_speed_left = positionNum_left;

    // state goal for left wheel controller initialization

    left_wheel_stateGoal.Position = 0;
    left_wheel_stateGoal.Speed = 0;
    left_wheel_control.setStateGoal(left_wheel_stateGoal);

    // current state for left wheel controller declaration
    PID_State left_wheel_currentState;
    // left face IIC address and state data feedback

    char slave_addr_left = 0x14;
    uint8_t data_left[7] = {0,0,0,0,0,0,0};

    // feedback data for left wheel
    int16_t current_position_left = 0;
    int32_t current_velocity_left = 0;
    int8_t left_wheel_cmd = 0;

    // controller for right wheel

    PID_Controller right_wheel_pid_controller(Kp, Ki, Kd, motor_cmd_min, motor_cmd_max);
    Wheel_Controller_OneMotor right_wheel_control(&right_wheel_pid_controller, right_wheel_motor);

    // right wheel position control trajectory initialization

    float coeff_right[6] = {0,0,0,0,0,0};
    float duration_right = 0;
    Polynomial p_right(coeff_right, 6);
    float startTime_right = 0;
    float endTime_right = 0;
    uint16_t sectionNum_right = 500;
    Trajectory trajectory_right(startTime_right, endTime_right, sectionNum_right, &p_right);

    // right wheel speed control trajecotory initialization

    float coeff_speed_right[2] = {0,0};
    Polynomial p_speed_right(coeff_speed_right, 2);
    uint16_t positionNum_right = 164;
    Trajectory trajectory_speed_right(positionNum_right, min_pos, max_pos, &p_speed_right);
    trajectory_speed_right.GeneratePositionVelocityTrajectory(path_speed_right, &velocity_speed_right);
    update_times_speed_right = positionNum_right;

    // state goal for right wheel controller initialization
    right_wheel_stateGoal.Position = 0;
    right_wheel_stateGoal.Speed = 0;
    right_wheel_control.setStateGoal(right_wheel_stateGoal);

    // current state for right wheel controller declaration
    PID_State right_wheel_currentState;
    // right face IIC address and state data feedback

    char slave_addr_right = 0x18;
    uint8_t data_right[7] = {0,0,0,0,0,0,0};

    // feedback data for right wheel
    int16_t current_position_right = 0;
    int32_t current_velocity_right = 0;
    int8_t right_wheel_cmd = 0;

// controller for pan wheel and tilt DoF
    float Kd_pan = 0.00011;
    float Kd_tilt = 0.00011;
    PID_Controller wheel_pid_controller_pan(Kp, Ki, Kd_pan, motor_cmd_min, motor_cmd_max);
    PID_Controller wheel_pid_controller_tilt(Kp, Ki, Kd_tilt, motor_cmd_min, motor_cmd_max);
    Wheel_Controller_Pan_Tilt wheel_control_pan_tilt(&wheel_pid_controller_pan, &wheel_pid_controller_tilt, left_pan_tilt_motor, right_pan_tilt_motor);

    // pan wheel position control trajectory initialization

    float coeff_pan[6] = {0,0,0,0,0,0};
    float duration_pan = 0;
    Polynomial p_pan(coeff_pan, 6);
    float startTime_pan = 0;
    float endTime_pan = 0;
    uint16_t sectionNum_pan = 500;
    Trajectory trajectory_pan(startTime_pan, endTime_pan, sectionNum_pan, &p_pan);

    // pan wheel speed control trajectory initialization

    float coeff_speed_pan[2] = {0,0};
    Polynomial p_speed_pan(coeff_speed_pan, 2);
    uint16_t positionNum_pan = 164;
    Trajectory trajectory_speed_pan(positionNum_pan, min_pos, max_pos, &p_speed_pan);
    trajectory_speed_pan.GeneratePositionVelocityTrajectory(path_speed_pan, &velocity_speed_pan);
    update_times_speed_pan = positionNum_pan;

    // state goal for pan wheel initialization

    pan_wheel_stateGoal.Position = 0;
    pan_wheel_stateGoal.Speed = 0;

    // current state for pan wheel declaration
    PID_State pan_wheel_currentState;
    // top face IIC address and state data feedback

    char slave_addr_pan = 0x1A;
    uint8_t data_pan[7] = {0,0,0,0,0,0,0};

    // feedback data for pan wheel

    int16_t current_position_pan = 0;
    int32_t current_velocity_pan = 0;
    int8_t left_motor_cmd = 0;
    int8_t right_motor_cmd = 0;

    // tilt DoF position control trajectory initialization

    float coeff_tilt[6] = {0,0,0,0,0,0};
    float duration_tilt = 0;
    Polynomial p_tilt(coeff_tilt, 6);
    float startTime_tilt = 0;
    float endTime_tilt = 0;
    uint16_t sectionNum_tilt = 500;
    Trajectory trajectory_tilt(startTime_tilt, endTime_tilt, sectionNum_tilt, &p_tilt);

    // state goal for tilt DoF initialization

    tilt_wheel_stateGoal.Position = 0;
    tilt_wheel_stateGoal.Speed = 0;

    // initialize the controller for pan wheel and tilt DoF

    wheel_control_pan_tilt.setStateGoal(pan_wheel_stateGoal, tilt_wheel_stateGoal);

    // current state for tilt DoF declaration
    PID_State tilt_wheel_currentState;
    // bottom face IIC address and state data feedback

    char slave_addr_tilt = 0x16;
    uint8_t data_tilt[7] = {0,0,0,0,0,0,0};

    // feedback data for tilt wheel
    int16_t current_position_tilt = 0;
    int32_t current_velocity_tilt = 0;
    


// initialize all start_flag to be 0 to shut down state goal updating until it gets feedback data
    start_flag_left = 0;
    start_flag_right = 0;
    start_flag_pan = 0;
    start_flag_tilt = 0;
    pan_ready_flag = 0;
    tilt_ready_flag = 0;
    char cmd;

//******* Create Data Manager ******// 
    DATA_MANAGER_t *dataManager =(DATA_MANAGER_t*)malloc(sizeof(DATA_MANAGER_t)); 
//******* Object Arrays  ***********//


    dataManager->wheel_motor[0] = &left_wheel_motor;
    dataManager->wheel_motor[1] = &right_wheel_motor;
    dataManager->wheel_motor[2] = &left_pan_tilt_motor; 
    dataManager->wheel_motor[3] = &right_pan_tilt_motor; 

    dataManager->wheel_control[0] = &left_wheel_control;
    dataManager->wheel_control[1] = &right_wheel_control; 
    dataManager->wheel_control_pt =  &wheel_control_pan_tilt; 

    dataManager->wheel_currentState[0] = &left_wheel_currentState; 
    dataManager->wheel_currentState[1] = &right_wheel_currentState; 
    dataManager->wheel_currentState[2] = &tilt_wheel_currentState; 
    dataManager->wheel_currentState[3] = &pan_wheel_currentState; 

    dataManager->wheel_stateGoal[0] = &left_wheel_stateGoal;
    dataManager->wheel_stateGoal[1] = &right_wheel_stateGoal;
    dataManager->wheel_stateGoal[2] = &tilt_wheel_stateGoal; 
    dataManager->wheel_stateGoal[3] = &pan_wheel_stateGoal;
    // PID_Controller wheel_pid_controller[4] ={left_wheel_pid_controller, right_wheel_pid_controller, 
                                             // wheel_pid_controller_pan, wheel_pid_controller_tilt};

    
    dataManager->poly[0] = &p_left; 
    dataManager->poly[1] = &p_right; 
    dataManager->poly[2] = &p_tilt;
    dataManager->poly[3] = &p_pan; 

    dataManager->poly_speed[0] = &p_speed_left; 
    dataManager->poly_speed[1] = &p_speed_right;
    dataManager->poly_speed[2] = &p_speed_pan;

    dataManager->traj[0] = &trajectory_left;
    dataManager->traj[1] = &trajectory_right; 
    dataManager->traj[2] = &trajectory_tilt; 
    dataManager->traj[3] = &trajectory_pan;

    dataManager->traj_speed[0] = &trajectory_speed_left; 
    dataManager->traj_speed[1] = &trajectory_speed_right; 
    dataManager->traj_speed[2] = &trajectory_speed_pan; 



//****global vars arrays****// 
    dataManager->current_position[0] = current_position_left;
    dataManager->current_position[1] = current_position_right; 
    dataManager->current_position[2] = current_position_tilt;
    dataManager->current_position[3] = current_position_pan; 

    dataManager->current_velocity[0] = current_velocity_left; 
    dataManager->current_velocity[1] = current_velocity_right;
    dataManager->current_velocity[2] = current_velocity_tilt; 
    dataManager->current_velocity[3] = current_velocity_pan; 

    dataManager->motor_cmd[0] = left_wheel_cmd; 
    dataManager->motor_cmd[1] = right_wheel_cmd; 
    dataManager->motor_cmd[2] = left_motor_cmd;
    dataManager->motor_cmd[3] = right_motor_cmd; // check the left motor == pan 

    dataManager->path_pos[0] = path_left; 
    dataManager->path_pos[1] = path_right; 
    dataManager->path_pos[2] = path_tilt; 
    dataManager->path_pos[3] = path_pan; 

    dataManager->velocity_pos[0] = velocity_left; 
    dataManager->velocity_pos[1] = velocity_right; 
    dataManager->velocity_pos[2] = velocity_tilt; 
    dataManager->velocity_pos[3] = velocity_pan; 

    dataManager->duration[0] = duration_left;
    dataManager->duration[1] = duration_right;
    dataManager->duration[2] = duration_tilt;
    dataManager->duration[3] = duration_pan; 

    dataManager->startTime[0] = startTime_left;
    dataManager->startTime[1] = startTime_right; 
    dataManager->startTime[2] = startTime_tilt; 
    dataManager->startTime[3] = startTime_pan; 

    dataManager->timer_counter[0] = timer_counter_left; 
    dataManager->timer_counter[1] = timer_counter_right; 
    dataManager->timer_counter[2] = timer_counter_tilt; 
    dataManager->timer_counter[3] = timer_counter_pan; 

    dataManager->endTime[0] = endTime_left;
    dataManager->endTime[1] = endTime_right; 
    dataManager->endTime[2] = endTime_tilt;
    dataManager->endTime[3] = endTime_pan;

    dataManager->sectionNum[0] = sectionNum_left;
    dataManager->sectionNum[1] = sectionNum_right;
    dataManager->sectionNum[2] = sectionNum_tilt; 
    dataManager->sectionNum[3] = sectionNum_pan; 

    dataManager->update_times[0] = update_times_left;
    dataManager->update_times[1] = update_times_right;
    dataManager->update_times[2] = update_times_tilt; 
    dataManager->update_times[3] = update_times_pan; 

    dataManager->init_flag[0] = init_flag_left;
    dataManager->init_flag[1] = init_flag_right;
    dataManager->init_flag[2] = init_flag_tilt;
    dataManager->init_flag[3] = init_flag_pan; 

    dataManager->power_on_flag = power_on_flag; 
    dataManager->counter_power_on[0] = counter_left_power_on; 
    dataManager->counter_power_on[1] = counter_right_power_on; 
    dataManager->counter_power_on[2] = counter_tilt_power_on; 
    dataManager->counter_power_on[3] = counter_pan_power_on; 

    dataManager->index_position[0] = index_position_left;
    dataManager->index_position[1] = index_position_right; 
    dataManager->index_position[2] = index_position_tilt; 
    dataManager->index_position[3] = index_position_pan; 

    dataManager->global_flag[0] = global_flag_left;
    dataManager->global_flag[1] = global_flag_right; 
    dataManager->global_flag[2] = global_flag_tilt; 
    dataManager->global_flag[3] = global_flag_pan; 

    dataManager->torque_flag[0] = torque_flag_left;
    dataManager->torque_flag[1] = torque_flag_right; 
    dataManager->torque_flag[2] = torque_flag_pan_tilt;
    





//*****Setup Complete*****// 

    // PlayTimebase();
    char test[] = "Init Wifi"; 
    char * test_buf = test; 
    wifi_send(test_buf,sizeof(test)); 




    while(1)
    {

        revLen = recvfrom(revSocket, rev_buf, sizeof(rev_buf), 0, (sockaddr*) &source_addr, &revPacketLen);
            if(revLen>0)
            {
               mRedON;
               wifi_receive_callback(dataManager); 
               mRedOFF; 
            }
        mBlueTOGGLE;
        mGreenTOGGLE;

    }
    return(0);
}



void wifi_send(void* data_to_send, long data_size )
{

    sendto(desSocket, data_to_send, data_size, 0, (sockaddr*) &des_addr, sizeof(des_addr));

} 

void wifi_receive_callback(DATA_MANAGER_t* data_manager)
{
    DATA_MANAGER_t *dataManager = (DATA_MANAGER_t*) data_manager;

    uint8_t  msg_id = rev_buf[0];  
    switch (msg_id)
    {
        case MsgBrainIpType: 
        {
            MsgBrainIp *rx_msg = (MsgBrainIp*)(rev_buf); 
           
            memset((char *)&des_addr, 0, sizeof(des_addr));
            des_addr.sin_family       = AF_INET;
            des_addr.sin_port         = htons(rx_msg->brain_socket);
            des_addr.sin_addr.s_addr  = rx_msg->brain_ip;

            char status_msg[] = "changed dest ip"; 
            char *msg_buf = status_msg; 
            debug_write_ip(des_addr.sin_addr.s_addr); 
            wifi_send(msg_buf, sizeof(status_msg)); 
            //sendto(desSocket,msg_buf,sizeof(status_msg),0, (sockaddr*) &new_addr, sizeof(new_addr));

            break;  
        }
         case MsgControlType: // Setting Controller States 
        {
            MsgControl *rx_msg = (MsgControl*)(rev_buf); 
            dataManager->control_struct.type = MsgControlType; 
            dataManager->control_struct.face_id = rx_msg->face_id;
            dataManager->control_struct.cmd = rx_msg->cmd;
            dataManager->control_struct.goal_position = rx_msg->goal_position;
            dataManager->control_struct.goal_velocity = rx_msg->goal_velocity;

            msgControlHandler(dataManager); 

            break; 
        }
        case MsgCmdType:
        {

            MsgCmd *rx_msg = (MsgCmd*)(rev_buf);  
            dataManager->cmd_struct.type = MsgCmdType; 
            dataManager->cmd_struct.cmd = rx_msg->cmd;
            msgCmdHandler(dataManager);              
            break; 
        }
        case MsgMagnetType:
        { 
            MsgMagnet *rx_msg = (MsgMagnet*)(rev_buf); 
            dataManager->magnet_struct.type = MsgMagnetType; 
            dataManager->magnet_struct.face = rx_msg->face;
            dataManager->magnet_struct.state = rx_msg->state;

            msgMagnetHandler(dataManager); 

            break; 
        }
        case MsgCurrentStateType: 
        {
            
            dataManager->currentstate_struct.type = MsgCurrentStateType; 
      
            wifi_send((void *)&(dataManager->currentstate_struct),sizeof(dataManager->currentstate_struct)); 
            break; 
        }
        case MsgModuleType: 
        {
            MsgModule *rx_msg = (MsgModule*)(rev_buf); 

            dataManager->module_struct.type = MsgModuleType; 
            dataManager->module_struct.data = rx_msg->data;
            break;
        }
        case MsgStatesType:
        {
            break;
        }

        case MsgTrajectoryPositionType:
        { 
            MsgTrajectoryPosition *rx_msg = (MsgTrajectoryPosition*)(rev_buf); 

            dataManager->trajectory_pos_struct.type = MsgTrajectoryPositionType; 
            dataManager->trajectory_pos_struct.face_id = rx_msg->face_id; 
            dataManager->trajectory_pos_struct.cmd = rx_msg->cmd; 
            dataManager->trajectory_pos_struct.coeff_1 = rx_msg->coeff_1; 
            dataManager->trajectory_pos_struct.coeff_2 = rx_msg->coeff_2;
            dataManager->trajectory_pos_struct.coeff_3 = rx_msg->coeff_3;  
            dataManager->trajectory_pos_struct.coeff_4 = rx_msg->coeff_4; 
            dataManager->trajectory_pos_struct.coeff_5 = rx_msg->coeff_5; 
            dataManager->trajectory_pos_struct.coeff_6 = rx_msg->coeff_6; 
            dataManager->trajectory_pos_struct.duration = rx_msg->duration; 

            msgTrajPosHandler(dataManager,left_wheel_stateGoal, right_wheel_stateGoal,
                               pan_wheel_stateGoal,tilt_wheel_stateGoal); 
         
            break;
        }
        case MsgTrajectoryPositionTXType: 
        {
            dataManager->trajectory_postx_struct.type     = MsgTrajectoryPositionTXType;      
            wifi_send((void*)&(dataManager->trajectory_postx_struct),sizeof(dataManager->trajectory_postx_struct));

            break;
        }

        case MsgTrajectoryVelocityType:
        {
           MsgTrajectoryVelocity *rx_msg = (MsgTrajectoryVelocity*)(rev_buf); 
           dataManager->trajectory_vel_struct.type    = MsgTrajectoryVelocityType; 
           dataManager->trajectory_vel_struct.face_id = rx_msg->face_id; 
           dataManager->trajectory_vel_struct.cmd     = rx_msg->cmd; 
           dataManager->trajectory_vel_struct.coeff_1 = rx_msg->coeff_1; 
           dataManager->trajectory_vel_struct.coeff_2 = rx_msg ->coeff_2; 

           break;
        }
    
        case MsgTrajectoryVelocityTXType:
        {
            dataManager->trajectory_veltx_struct.type = MsgTrajectoryVelocityTXType; 
            dataManager->trajectory_veltx_struct.coeff_1 = dataManager->trajectory_vel_struct.coeff_1;
            dataManager->trajectory_veltx_struct.coeff_2 = dataManager->trajectory_vel_struct.coeff_2; 
            wifi_send((void*)&(dataManager->trajectory_veltx_struct),sizeof(dataManager->trajectory_veltx_struct)); 
            break;
        }

        case MsgTorqueType: 
        {   
            MsgTorque *rx_msg = (MsgTorque*)(rev_buf); 

            dataManager->torque_struct.type      = MsgTorqueType;
            dataManager->torque_struct.DoF       = rx_msg->DoF;
            dataManager->torque_struct.direction = rx_msg->direction;
            dataManager->torque_struct.speed     = rx_msg->speed;
            wifi_send((void*)&(dataManager->torque_struct),sizeof(dataManager->torque_struct));
            break;
        }
       
        default:  //Error can't find message type 
            // maybe have a certain led flashing 
            char msg[] = "Error!";
            char* msg_buf = msg; 
            wifi_send(msg_buf,sizeof(msg));
            break; 

    }

}

