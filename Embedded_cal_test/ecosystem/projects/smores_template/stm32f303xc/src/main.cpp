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
#define   mBUS 1 

#define SEND_DATA 10
#define M_YELLOW_TOGGLE 20
#define M_WHITE_TOGGLE  30

int main (void){
	InitPeripherals();
    
    UsbInterface usb = UsbInterface();
    usb.Init();
    
    struct Time currentTime;
    
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
            mGreenTOGGLE;
         	uint8_t type = rx_data[0];
         	if(type == MsgMyDataSpecType) {
         		mBlueTOGGLE;
         		MsgMyDataSpec *rx_msg = ((MsgMyDataSpec*) (rx_data+1));
         		switch (rx_msg->cmd) {
         		case SEND_DATA:
         			mRedTOGGLE;
         			MsgMyData tx_msg;
         			tx_msg.data = rx_msg->cmd;
         			usb.SendPacket(MsgMyDataType, (uint8_t*)&tx_msg, sizeof(tx_msg));
         			break;
         		case M_YELLOW_TOGGLE:
         			mYellowTOGGLE;
         			break;
         		case M_WHITE_TOGGLE:
         			mWhiteTOGGLE;
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
        
        
        mRedTOGGLE;
        mGreenTOGGLE;
        mBlueTOGGLE;
        DelayMilliseconds(10);
 
    } // while(1)
    return(0);
}
