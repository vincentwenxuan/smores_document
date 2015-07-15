#ifndef STM_CALI_
#define STM_CALI_

#include "common_peripherals.h"
#include "system_clock.h"
#include "mBus.h"
// common components 
#include "common_message_types.h" 
// common messages
#include "usb_interface.h"        

// common libraries
#include "project_message_types.h"
#include "error_reporting.h"
#include <string.h> 
//needed for memset
//#define   mBUS 1

// user libraries
#include "Wheel_Control.h"
#include "common_macro.h"

// face ping request messages:
#define LEFT_FACE_PING   121
#define RIGHT_FACE_PING  122
#define TOP_FACE_PING    123
#define BOTTOM_FACE_PING 124

void cali_cmd(char address, Motor motor1, Motor motor2);

#endif //STM_CALI_