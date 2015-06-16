// common libraries
#include "common_peripherals.h"
#include "project_message_types.h"
#include "error_reporting.h"

// common components
#include "usb_interface.h"
#include "zigbee_interface.h"
#include "system_clock.h"

////////////////////////////////////////////////////////////////////////////////
// Entry Point

int main(void) {

  InitPeripherals();

  mRedOFF; mGreenOFF; mBlueOFF;

  UsbInterface usb = UsbInterface();
  usb.Init();
  error_reporting_com1 = &usb;

  PersistentMemory mem;

  DelaySeconds(1.0f);
  ZigbeeInterface zig = ZigbeeInterface();
  DelaySeconds(0.05f);
  zig.Init(mem);
  error_reporting_com2 = &zig;

  //////////////////////////////////////////////////////////////////////////////
  // Main Loop	
  while(1) { 
 
    uint8_t *rx_data;
    uint8_t rx_length;
    uint8_t is_data;

    ////////////////////////////////////////////////////////////////////////////
    // USB Input
    is_data = 0;
    usb.GetBytes();
    while(usb.PeekPacket(&rx_data, &rx_length)) {
      zig.ReadMsg(usb, rx_data, rx_length);
      usb.DropPacket();
      is_data = 1;
    } // while peek...
    if(is_data) {
      usb.SendNow();
    }

    ////////////////////////////////////////////////////////////////////////////
    // Radio Input
    is_data = 0;
    zig.GetBytes();
    while(zig.PeekPacket(&rx_data, &rx_length)) {
      zig.ReadMsg(zig, rx_data, rx_length);

      uint8_t type = rx_data[0];
      if(type == kTypeStampedCounterMOSI) {
        MsgStampedCounterMOSI *rx_msg = ((MsgStampedCounterMOSI*) &rx_data);
        MsgStampedCounterMISO tx_msg;
        struct Time t = GetTime();
        tx_msg.sec  = t.sec;
        tx_msg.nsec = t.nsec;
        tx_msg.count = rx_msg->count;
        zig.SendPacket(kTypeStampedCounterMISO, (uint8_t*)&tx_msg, sizeof(tx_msg));
      }
      
      zig.DropPacket();
      is_data = 1;
    } // while peek...
    if(is_data) {
      zig.SendNow();
    }
    
  } // while(1)
  return(0);
}