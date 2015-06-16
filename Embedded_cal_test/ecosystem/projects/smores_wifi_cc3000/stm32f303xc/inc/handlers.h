#ifndef HANDLERS_H 
#define HANDLERS_H 

#include "wifi.h"
#include "mBus.h"
#include "Wheel_Control.h"
#include "Trajectory.h"


//MSG Magnet Function Handlers
void msgMagnetHandler(DATA_MANAGER_t* data_manager); 

// MSG Control Function Handlers 
void msgControlHandler(DATA_MANAGER_t* data_manager); 
void msgControlUpdateHandler(DATA_MANAGER_t* data_manager, int face_num); 
void msgControlSendHandler(DATA_MANAGER_t* data_manager, int face_num); 

//MSG Cmd Function Handlers
void msgCmdHandler(DATA_MANAGER_t* data_manager);
uint16_t handleFacePing(DATA_MANAGER_t* data_manager); 

//MSG Trajectory Pos Function Handlers 
void msgTrajPosHandler(DATA_MANAGER_t* data_manager, PID_State lw_stateGoal,
                         PID_State rw_stateGoal, PID_State pw_stateGoal, 
                         PID_State tw_stateGoal);

void updateTrajTimers(DATA_MANAGER_t* data_manager,int face_num, float endTime, float startTime, uint16_t sectionNum, int timer_counter);
void updateTrajGoals(DATA_MANAGER_t* data_manager, int face_num, PID_State lw_stateGoal, PID_State rw_stateGoal,
					 PID_State pw_stateGoal,PID_State tw_stateGoal);

void msgTrajPosInitHandler(DATA_MANAGER_t* data_manager, int face_num);
void msgTrajPosUpdateHandler(DATA_MANAGER_t* data_manager,int face_num, PID_State lw_stateGoal, 
							 PID_State rw_stateGoal, PID_State pw_stateGoal,PID_State tw_stateGoal); //check the types
void msgTrajPosSendHandler(DATA_MANAGER_t* data_manager,int face_num); 

#endif //HANDLERS_H