#include "handlers.h"



void msgMagnetHandler(DATA_MANAGER_t* data_manager)
{
    uint8_t cmd = MAGNETS_OFF; 
    DATA_MANAGER_t *dataManager = (DATA_MANAGER_t*) data_manager; 

    if(dataManager->magnet_struct.state)
    {
        cmd = MAGNETS_ON; 
    }
    switch(dataManager->magnet_struct.face)
    {
        case LEFT_FACE:
            mBusWriteNoAdd(LEFT_FACE_I2C_ADDRESS << 1, cmd, 1); // cant this just be the state ?
            break;

        case RIGHT_FACE:
            mBusWriteNoAdd(RIGHT_FACE_I2C_ADDRESS << 1, cmd, 1);
            break;

        case TOP_FACE:
            mBusWriteNoAdd(TOP_FACE_I2C_ADDRESS << 1, cmd, 1);
            break;

        case BOTTOM_FACE:
            mBusWriteNoAdd(BOTTOM_FACE_I2C_ADDRESS << 1, cmd, 1);
            break;
    }

    return; 
}


void msgControlHandler(DATA_MANAGER_t* data_manager)
{
    DATA_MANAGER_t *dataManager = (DATA_MANAGER_t*) data_manager; 
    int face_num  = (int)(dataManager->control_struct.face_id)%DOF; 

    switch(dataManager->control_struct.cmd)
    {
        case SEND_DATA:
        {
            msgControlSendHandler(dataManager, face_num); 
            break; 
        }
        case UPDATE_GOAL: 
        {
            msgControlUpdateHandler(dataManager, face_num); 
            break;
        }
        default:
            char msg[] = "Unknown Control Command Error!";
            char* msg_buf = msg; 
            wifi_send(msg_buf,sizeof(msg));
    }

    return; 
}

void msgControlUpdateHandler(DATA_MANAGER_t* data_manager, int face_num)
{
    DATA_MANAGER_t *dataManager = (DATA_MANAGER_t*) data_manager; 
    dataManager->wheel_stateGoal[face_num]->Position = dataManager->control_struct.goal_position;
    dataManager->wheel_stateGoal[face_num]->Speed = dataManager->control_struct.goal_velocity;
    return; 

}
void msgControlSendHandler(DATA_MANAGER_t* data_manager, int face_num) 
{
    DATA_MANAGER_t *dataManager = (DATA_MANAGER_t*) data_manager; 
    mWhiteTOGGLE;
    dataManager->currentstate_struct.type = MsgCurrentStateType; 
    dataManager->currentstate_struct.goal_position = dataManager->wheel_stateGoal[face_num]->Position;
    dataManager->currentstate_struct.goal_velocity = dataManager->wheel_stateGoal[face_num]->Speed;
    // dataManager->currentstate_struct.current_position = current_position[face_num];
    // dataManager->currentstate_struct.current_velocity = current_velocity[face_num];
    dataManager->currentstate_struct.current_position = dataManager->wheel_currentState[face_num]->Position; 
    dataManager->currentstate_struct.current_velocity= dataManager->wheel_currentState[face_num]->Speed;
    dataManager->currentstate_struct.motor_cmd = dataManager->motor_cmd[face_num];

    wifi_send((void *)&(dataManager->currentstate_struct),sizeof(dataManager->currentstate_struct));

}

void msgCmdHandler(DATA_MANAGER_t* data_manager)
{
    DATA_MANAGER_t *dataManager = (DATA_MANAGER_t*) data_manager; 
    dataManager->module_struct.type = MsgModuleType; 
    dataManager->module_struct.data = handleFacePing(dataManager); 
    wifi_send((void *)&(dataManager->module_struct),sizeof(dataManager->module_struct)); 

}
uint16_t handleFacePing(DATA_MANAGER_t* data_manager) 
{

    // Send a ping to a face, and receive data back:
    // Note: A left-shift of 1 (<< 1) appends a zero in the least significant bit

    //       of the address.  This indicates I2C read (as opposed to write).
    DATA_MANAGER_t *dataManager = (DATA_MANAGER_t*) data_manager; 
    if(dataManager->cmd_struct.cmd > 100) 
    {
                  
        uint8_t data = 0xff;
        switch(dataManager->cmd_struct.cmd) {
            case LEFT_FACE_PING:
                data = mBusRead(LEFT_FACE_I2C_ADDRESS<<1, FACE_PING, 1);
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
    }

    return 0; 

}

void msgTrajPosHandler(DATA_MANAGER_t* data_manager, PID_State lw_stateGoal,
                         PID_State rw_stateGoal, PID_State pw_stateGoal, 
                         PID_State tw_stateGoal)
{
    DATA_MANAGER_t* dataManager = (DATA_MANAGER_t*) data_manager; 
    int face_num = (dataManager->trajectory_pos_struct.face_id)%DOF; 

    switch(dataManager->trajectory_pos_struct.cmd)
    {
        case INIT: 
        {
            msgTrajPosInitHandler(dataManager,face_num); 
            break;
        }
        case UPDATE_PARAMETER: 
        {
            msgTrajPosUpdateHandler(dataManager,face_num,lw_stateGoal,rw_stateGoal,pw_stateGoal,tw_stateGoal); 
            break;
        }
        case SEND_PARAMETER: 
        {
            msgTrajPosSendHandler(dataManager,face_num); 
            break; 
        }
        default: 
            char msg[] = "Unknown Traj Pos Command Error!";
            char* msg_buf = msg; 
            wifi_send(msg_buf,sizeof(msg));
            break; 

    }

    return; 
}
void msgTrajPosSendHandler(DATA_MANAGER_t* data_manager,int face_num)
{
    DATA_MANAGER_t* dataManager = (DATA_MANAGER_t*) data_manager; 
    dataManager-> trajectory_postx_struct.type = MsgTrajectoryPositionTXType; 
    dataManager->trajectory_postx_struct.coeff_1 = dataManager->pos_coeff[0];
    dataManager->trajectory_postx_struct.coeff_2 = dataManager->pos_coeff[1];
    dataManager->trajectory_postx_struct.coeff_3 = dataManager->pos_coeff[2];
    dataManager->trajectory_postx_struct.coeff_4 = dataManager->pos_coeff[3];
    dataManager->trajectory_postx_struct.coeff_5 = dataManager->pos_coeff[4];
    dataManager->trajectory_postx_struct.coeff_6 = dataManager->pos_coeff[5]; 
    dataManager-> trajectory_postx_struct.duration = dataManager->duration[face_num]; 

    wifi_send((void* )&(dataManager->trajectory_postx_struct), 
                       sizeof(dataManager->trajectory_postx_struct));
    return;
}
void msgTrajPosInitHandler(DATA_MANAGER_t* data_manager, int face_num)
{
    DATA_MANAGER_t* dataManager = (DATA_MANAGER_t*) data_manager; 
    dataManager->init_flag[face_num] = 1; 
    int tilt_num = TILT_DOF%DOF; 
    int pan_num = PAN_DOF%DOF; 

    if(dataManager->trajectory_pos_struct.face_id == TILT_DOF)
    {
        dataManager->wheel_stateGoal[pan_num]->Position = dataManager->wheel_currentState[pan_num]->Position;
        dataManager->wheel_stateGoal[pan_num]->Speed = dataManager->wheel_currentState[pan_num]->Speed;

    }
    else if(dataManager->trajectory_pos_struct.face_id == PAN_DOF)
    {
        dataManager->wheel_stateGoal[tilt_num]->Position = dataManager->wheel_currentState[tilt_num]->Position;
        dataManager->wheel_stateGoal[tilt_num]->Speed = dataManager->wheel_currentState[tilt_num]->Speed;

    }

    return; 
}
void msgTrajPosUpdateHandler(DATA_MANAGER_t* data_manager,int face_num, PID_State lw_stateGoal,PID_State rw_stateGoal, PID_State pw_stateGoal, 
                         PID_State tw_stateGoal) //check the types
{
    DATA_MANAGER_t* dataManager = (DATA_MANAGER_t*) data_manager; 
    float duration = dataManager->trajectory_pos_struct.duration;
    float startTime = 0; 
    int timer_counter = 1; 
    float endTime = startTime + duration;  
    uint16_t sectionNum = (endTime - startTime) / (timer_counter * 0.01); 
    // coefficients for update 
    dataManager->pos_coeff[0] = dataManager->trajectory_pos_struct.coeff_1;
    dataManager->pos_coeff[1] = dataManager->trajectory_pos_struct.coeff_2;
    dataManager->pos_coeff[2] = dataManager->trajectory_pos_struct.coeff_3;
    dataManager->pos_coeff[3] = dataManager->trajectory_pos_struct.coeff_4;
    dataManager->pos_coeff[4] = dataManager->trajectory_pos_struct.coeff_5;
    dataManager->pos_coeff[5] = dataManager->trajectory_pos_struct.coeff_6;


    // update Trajectory coeffs and times 
    dataManager->poly[face_num]->updateCofficient(dataManager->pos_coeff);
    updateTrajTimers(dataManager, face_num, endTime,startTime,sectionNum,timer_counter); 

    // Trajectory Generation 
    dataManager->traj[face_num]->updateTrajectory(startTime, endTime, sectionNum, (dataManager->poly[face_num]));
    dataManager->traj[face_num]->GeneratePositionTrajectory(dataManager->path_pos[face_num]);
    dataManager->traj[face_num]->GenerateVelocityTrajectory(dataManager->velocity_pos[face_num]); // check that it sends all 501 elements of path@ face_num
    
    // update goal states (break out into another function)

    updateTrajGoals(dataManager, face_num, lw_stateGoal,rw_stateGoal,pw_stateGoal,tw_stateGoal);

   
    return; 
}
void updateTrajGoals(DATA_MANAGER_t* data_manager, int face_num, PID_State lw_stateGoal, PID_State rw_stateGoal, PID_State pw_stateGoal, 
                         PID_State tw_stateGoal)
{
    DATA_MANAGER_t* dataManager = (DATA_MANAGER_t*) data_manager;
    int pan_num = PAN_DOF %DOF; 
    int tilt_num = TILT_DOF %DOF;  
    // PID_State test = &*(dataManager->wheel_stateGoal[face_num]); 

    switch(dataManager->trajectory_pos_struct.face_id)
    {
        case  TILT_DOF:
            dataManager->wheel_stateGoal[pan_num]->Position = dataManager->wheel_currentState[pan_num]->Position;
            dataManager->wheel_stateGoal[pan_num]->Speed = 0;
            dataManager->wheel_stateGoal[tilt_num]->Position = dataManager->path_pos[face_num][0];
            dataManager->wheel_stateGoal[tilt_num]->Speed = dataManager->velocity_pos[face_num][0];
            dataManager->wheel_control_pt->setStateGoal(tw_stateGoal, pw_stateGoal);
             // set flags 
   
            dataManager->torque_flag[2]= 0;


            break; 
        case PAN_DOF: 
            dataManager->wheel_stateGoal[tilt_num]->Position = dataManager->wheel_currentState[tilt_num]->Position;
            dataManager->wheel_stateGoal[tilt_num]->Speed = 0;
            dataManager->wheel_stateGoal[pan_num]->Position = dataManager->path_pos[face_num][0];
            dataManager->wheel_stateGoal[pan_num]->Speed = dataManager->velocity_pos[face_num][0];
            dataManager->wheel_control_pt->setStateGoal(pw_stateGoal, tw_stateGoal);
            dataManager->torque_flag[2]= 0;
            break; 
            

        case LEFT_DOF:
            dataManager->wheel_stateGoal[face_num]->Position = dataManager->path_pos[face_num][0];
            dataManager->wheel_stateGoal[face_num]->Speed = dataManager->velocity_pos[face_num][0];
            dataManager->wheel_control[face_num]->setStateGoal(lw_stateGoal);
            dataManager->torque_flag[0]= 0;
            break; 
   

        case RIGHT_DOF: 
            dataManager->wheel_stateGoal[face_num]->Position = dataManager->path_pos[face_num][0];
            dataManager->wheel_stateGoal[face_num]->Speed = dataManager->velocity_pos[face_num][0];
            dataManager->wheel_control[face_num]->setStateGoal(rw_stateGoal);
             dataManager->torque_flag[1]= 0;
            break; 

    }
        dataManager->counter[face_num] = 0;
        dataManager->index_position[face_num] = 0;
        dataManager->init_flag[face_num] = 0;
        dataManager->global_flag[face_num] = 1;
    return; 

}
void updateTrajTimers(DATA_MANAGER_t* data_manager,int face_num, float endTime, float startTime, uint16_t sectionNum, int timer_counter)
{
    DATA_MANAGER_t* dataManager = (DATA_MANAGER_t*) data_manager; 

    dataManager->startTime[face_num] = startTime; 
    dataManager->endTime[face_num] = endTime; 
    dataManager->sectionNum[face_num] = sectionNum; 
    dataManager->timer_counter[face_num] = timer_counter;   // check if this needs to be global  
    dataManager->update_times[face_num] = sectionNum;

    return; 

}
