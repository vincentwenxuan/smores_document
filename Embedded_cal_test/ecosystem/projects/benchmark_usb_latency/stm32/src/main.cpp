// common libraries
#include "common_peripherals.h"
#include "project_message_types.h"
#include "error_reporting.h"

// common components
#include "usb_interface.h"
#include "system_clock.h"

////////////////////////////////////////////////////////////////////////////////
// Entry Point

int main(void) {

  InitPeripherals();

  UsbInterface usb = UsbInterface();
  usb.Init();
  error_reporting_com1 = &usb;

  mRedOFF; mGreenOFF; mBlueOFF;

  //////////////////////////////////////////////////////////////////////////////
  // Main Loop	
  while(1) { 
 
    uint8_t *rx_data;
    uint8_t rx_length;
    uint8_t is_data = 0;
    usb.GetBytes();
    while(usb.PeekPacket(&rx_data, &rx_length)) {
      mGreenON;

      uint8_t type = rx_data[0];
      if(type == kTypeStampedCounterMOSI) {
        MsgStampedCounterMOSI *rx_msg = ((MsgStampedCounterMOSI*) &rx_data);
        MsgStampedCounterMISO tx_msg;
        struct Time t = GetTime();
        tx_msg.sec  = t.sec;
        tx_msg.nsec = t.nsec;
        tx_msg.count = rx_msg->count;
        usb.SendPacket(kTypeStampedCounterMISO, (uint8_t*)&tx_msg, sizeof(tx_msg));
      }

      usb.DropPacket();
      is_data = 1;
    } // while peek...
    if(is_data) {
      usb.SendNow();
      mGreenOFF;
    }
    
  } // while(1)
  return(0);
}