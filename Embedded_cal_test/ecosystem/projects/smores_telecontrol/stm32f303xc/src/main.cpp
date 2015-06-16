#include "common_peripherals.h"
#include "system_clock.h"
// common components 
#include "common_message_types.h" 
// common messages
#include "usb_interface.h"        
// usb interface
#include "mBus.h"

// common libraries
#include "project_message_types.h"
#include "error_reporting.h"
#include <string.h> 
//needed for memset
//#define   mBUS 1
#include "Wheel_Control_naive.h"

/* Command macros (for command messages from Matlab): */
#define SEND_DATA 10
#define M_YELLOW_TOGGLE 20
#define M_WHITE_TOGGLE  30

// face ping request messages:
#define LEFT_FACE_PING   121
#define RIGHT_FACE_PING  122
#define TOP_FACE_PING    123
#define BOTTOM_FACE_PING 124

/*  DoF macros:    */
#define DoF_left  11
#define Dof_right 12
#define Dof_pan   13
#define Dof_tilt  14

/*   face macros:   */
#define LEFT_FACE 21  // left
#define RIGHT_FACE 22  // right
#define TOP_FACE 23  // top
#define BOTTOM_FACE 24  // bottom

/*   face i2c address macros:  */
#define BOTTOM_FACE_I2C_ADDRESS 0x0B
#define LEFT_FACE_I2C_ADDRESS   0x0A
#define RIGHT_FACE_I2C_ADDRESS  0x0C
#define TOP_FACE_I2C_ADDRESS    0x0D

/*   face board macros:   */
#define MAGNETS_ON  0xA2
#define MAGNETS_OFF 0xA3
#define FACE_PING   0xA5

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

    init_Dof();
    
    struct Time currentTime;

    //char slave_addr = 0x16;
    //char slave_addr = 0x14;
//    char slave_addr = 0x0A << 1;
    char slave_cmd;
    
    mYellowON; mRedON; mGreenON; mWhiteON; mBlueON;
    DelayMilliseconds(1000);
    mYellowOFF;mRedOFF;mGreenOFF; mWhiteOFF; mBlueOFF;
  //////////////////////////////////////////////////////////////////////////////
  // Main Loop
    while(1) {
        
        struct Time t = GetTime();
        currentTime = t;
        
        uint8_t *rx_data;
        uint8_t rx_length;
        uint8_t is_data = 0;
        
        // for talking between laptop and micro
        usb.GetBytes();
        while(usb.PeekPacket(&rx_data, &rx_length)) {
            uint8_t type = rx_data[0];
            if(type == MsgCmdType) {
                mGreenTOGGLE;
                MsgCmd *rx_msg = ((MsgCmd*) (rx_data+1));
                switch (rx_msg->cmd) {
                case SEND_DATA:
                    mBlueTOGGLE;
                    MsgModule tx_msg;
                    tx_msg.data = rx_msg->cmd;
                    usb.SendPacket(MsgModuleType, (uint8_t*)&tx_msg, sizeof(tx_msg));
                    break;
                case M_YELLOW_TOGGLE:
                    mYellowTOGGLE;
                    break;
                case M_WHITE_TOGGLE:
                    mWhiteTOGGLE;
                    break;
                }
                // face name request messages are all >100:
                if(rx_msg->cmd > 100) {
                    mYellowTOGGLE;
                    MsgModule tx_msg;
                    tx_msg.data = handleFacePing(rx_msg->cmd);
                    usb.SendPacket(MsgModuleType, (uint8_t*)&tx_msg, sizeof(tx_msg));
                }
            } else if (type == MsgDofType) {
                MsgDof *rx_msg = ((MsgDof*) (rx_data+1));
                switch (rx_msg->DoF) {
                case DoF_left:
                    left_control(rx_msg->direction, rx_msg->speed);
                    break;
                case Dof_right:
                    right_control(rx_msg->direction, rx_msg->speed);
                    break;
                case Dof_tilt:
                    tilt_control(rx_msg->direction, rx_msg->speed);
                    break;
                case Dof_pan:
                    pan_control(rx_msg->direction, rx_msg->speed);
                    break;
                }
            } else if (type == MsgMagnetType) {
                MsgMagnet *rx_msg = ((MsgMagnet*) (rx_data+1));
                switch (rx_msg->face) {
                case LEFT_FACE:
                    mWhiteTOGGLE;
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
                    mGreenTOGGLE;
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
                    mRedTOGGLE;
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
                    mBlueTOGGLE;
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
       
        DelayMilliseconds(10);
 
    } // while(1)
    return(0);
}

