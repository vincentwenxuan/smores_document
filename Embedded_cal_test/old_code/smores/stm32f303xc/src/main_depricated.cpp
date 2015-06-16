#include "common_peripherals.h"

// common components 
#include "common_message_types.h" 
// common messages
#include "usb_interface.h"        
// usb interface
#include "mBus.h"

// common libraries
#include "project_message_types.h"
#include "error_reporting.h"
//#include "mGeneral.h"
#include <string.h> 
//needed for memset
#define   mBUS 1 
const uint8_t atmega1_addr = 0x0A <<1; 
const uint8_t atmega1_regaddr = 0xA0; 

int main (void){
	InitPeripherals(); 
	mYellowON; mRedON; mGreenON; 
	DelayMilliseconds(5); 
	mYellowOFF;mRedOFF;mGreenOFF;
  face_state face1; 

	uint32_t in_values[5] = {0xA0, 0xA1, 0xA2,0,0};
  uint32_t out_values[5];

  //////////////////////////////////////////////////////////////////////////////
  // Main Loop
  while(1) {
    DelayMilliseconds(5);
    // clear retrieved values (no cheating)
    memset(out_values, 0, 5); 

    // write one uint32_t values to register 0xA0
    mGreenON; 
    if (GetBytes() == 1)
    {
      mRedON;
      mBusMasterReadBurstStart(atmega1_addr,msg_face_state, sizeof(face1), mBUS);
      DelayMicroseconds(500);
      mBusReadBurstDataNB(16, (uint8_t*)&out_values[0], mBUS);
      face1.pos = out_values[0];
      face1.vel = out_values[1]; 
      face1.acc = out_values[2]; 
    } 
    
    // // write four uint32_t values starting at register 0xA1
    // //   this fills registers 0xA1 through 0xA4

    // mYellowON; 
    // mBusMasterWriteBurst(atmega1_addr, 0xA1, 16, (uint8_t*)&in_values[1], mBUS);
    // DelayMicroseconds(500);
    // mGreenOFF; 
    // mYellowOFF; 

    // // read four uint32_t values starting at register 0xA0
    // //   this calls from registers 0xA1 through 0xA3
    // mRedON;
    // mBusMasterReadBurstStart(atmega1_addr,msg_face_state, sizeof(face1), mBUS);
    // DelayMicroseconds(500);
    // mBusReadBurstDataNB(16, (uint8_t*)&out_values[0], mBUS);
    // face1.pos = out_values[0];
    // face1.vel = out_values[1]; 
    // face1.acc = out_values[2]; 

    // // read one uint32_t value from register 0xA4
    // mYellowON; 
    // mBusMasterReadBurstStart(atmega1_addr, 0xA4, 4, mBUS);
    // DelayMicroseconds(500);
    // mBusReadBurstDataNB(4, (uint8_t*)&out_values[4], mBUS);

    // mRedOFF;
    // mYellowOFF; 

     DelayMilliseconds(5);
     mGreenOFF; 

 
  } // while(1)
  return(0);
}

