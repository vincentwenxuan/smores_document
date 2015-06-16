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

#define SEND_DATA 10
#define M_YELLOW_TOGGLE 20
#define M_WHITE_TOGGLE  30

#define DoF_left  11
#define Dof_right 12
#define Dof_pan   13
#define Dof_tilt  14

#define face1 21
#define face2 22
#define face3 23
#define face4 24

/*   face i2c address macros:  */
#define BOTTOM_FACE_I2C_ADDRESS 0x0B
#define LEFT_FACE_I2C_ADDRESS   0x0A
#define RIGHT_FACE_I2C_ADDRESS  0x0C
#define TOP_FACE_I2C_ADDRESS    0x0D

#define MAGNETS_ON 0xA2
#define MAGNETS_OFF 0xA3

int main (void){
    InitPeripherals();
    
    UsbInterface usb = UsbInterface();
    usb.Init();

    char slave_cmd;
    
    STM_EVAL_LEDInit(LED3);
    STM_EVAL_LEDInit(LED4);
    STM_EVAL_LEDInit(LED5);
    STM_EVAL_LEDInit(LED6);
    STM_EVAL_LEDInit(LED7);
    STM_EVAL_LEDInit(LED8);
    STM_EVAL_LEDInit(LED9);
    STM_EVAL_LEDInit(LED10);
    STM_EVAL_PBInit(BUTTON_USER, BUTTON_MODE_GPIO);
    
    while(1)
    {
        uint8_t *rx_data;
        uint8_t rx_length;
        uint8_t is_data = 0;
        
        // for talking between laptop and micro
        usb.GetBytes();
        while(usb.PeekPacket(&rx_data, &rx_length)) {
            uint8_t type = rx_data[0];
            if(type == MsgCmdType) {

                MsgCmd *rx_msg = ((MsgCmd*) (rx_data+1));
                switch (rx_msg->cmd) {
                case SEND_DATA:

                    MsgModule tx_msg;
                    tx_msg.data = rx_msg->cmd;
                    usb.SendPacket(MsgModuleType, (uint8_t*)&tx_msg, sizeof(tx_msg));
                    break;
                case M_YELLOW_TOGGLE:

                    break;
                case M_WHITE_TOGGLE:

                    break;
                }
            } else if (type == MsgMagnetType) {
                MsgMagnet *rx_msg = ((MsgMagnet*) (rx_data+1));
                switch (rx_msg->face) {
                case face1:
                    STM_EVAL_LEDToggle(LED3);
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
                case face2:
                    STM_EVAL_LEDToggle(LED4);
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
                case face3:
                    STM_EVAL_LEDToggle(LED5);
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
                case face4:
                    STM_EVAL_LEDToggle(LED6);
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
            usb.DropPacket();
            is_data = 1;
        }
        if(is_data) {
            is_data = 0;
            usb.SendNow();
        }
       
    }
    return(0);
}

