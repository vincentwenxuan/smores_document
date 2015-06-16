#include "common_peripherals.h"

/// Include your custom headers here
#include "usb_interface.h"
#include "zigbee_interface.h"
#include "persistent_memory.h"
#include "usb_radio_msgs.h"
#include "system_clock.h"

#include "common_message_types.h"
#include "usb_radio_msgs.h"
#include "zigbee_interface_msgs.h"
#include "error_reporting.h"

void ReadMsg(CommunicationInterface& com, uint8_t* rx_data, uint8_t rx_length, ZigbeeInterface& zig);
void ReplyByte(CommunicationInterface& com, uint8_t subtype, uint8_t value);

// main function
int main(void)
{
  InitPeripherals();  // configure GPIO, USB, I2C, ect based on peripherals.h

  UsbInterface usb = UsbInterface();
  usb.Init();
  error_reporting_com1 = &usb;
  
  mRedON; mWhiteON; mGreenON; mAmberON;
  
  DelaySeconds(1);
  
  mRedOFF; mWhiteOFF; mGreenOFF; mAmberOFF;
  
  PersistentMemory mem;

  ZigbeeInterface zig = ZigbeeInterface();
  zig.Init(mem);
  error_reporting_com2 = &zig;


  uint8_t *received_usb_data;  // temporary pointer to received type+data bytes
  uint8_t received_usb_length; // number of of received type+data bytes
  uint8_t *received_zigbee_data; // temporary pointer to received type+data bytes
  uint8_t received_zigbee_length; // number of of received type+data bytes

  struct Time timestamp = GetTime(); // timestamp for heartbeat

  mem.Freeze();

  // main loop
  while(1)
  {
    // USB to Zigbee
    usb.GetBytes(); // poll for new USB bytes
    if(usb.PeekPacket(&received_usb_data, &received_usb_length)) // if we got usb data
    {
      mWhiteTOGGLE;

      // handle explicit kTypeRelaySend messages
      if(received_usb_data[0] == kTypeRelaySend)//if packet is type relay send 
      {
        MsgRelaySend *relay_msg = (MsgRelaySend*) &(received_usb_data[1]);
        zig.rx_channel = relay_msg->receiver_channel;
        zig.tx_channel = relay_msg->receiver_channel;
        //reduce length by the size of metadata and move data index forward accordingly
        received_usb_length = received_usb_length - (&(relay_msg->type) - received_usb_data);
        received_usb_data = &(relay_msg->type);
      }

      // act on utility commands to self
      ReadMsg(usb, received_usb_data, received_usb_length, zig);

      // pass on messages
      zig.SendPacket(received_usb_data[0],&received_usb_data[1],received_usb_length-1); //received_usb_data broken into type (1 byte) and data (length-1)

      zig.SendNow();
      usb.SendNow();
      usb.DropPacket();
    }
    
    // Zigbee to USB
    zig.GetBytes(); // poll for new zigbee bytes
    uint8_t rx_zig_packet = 0;
    while(zig.PeekPacket(&received_zigbee_data, &received_zigbee_length)) // if we got zigbee data
    {
      mGreenTOGGLE;
      usb.SendPacket(received_zigbee_data[0],&received_zigbee_data[1],received_zigbee_length-1); //received_zigbee_length broken into type (1 byte) and data (length-1)
      rx_zig_packet = 1;
      zig.DropPacket();
    }
    if(rx_zig_packet) {
      usb.SendNow();
    }
    rx_zig_packet = 0;

    // heartbeat
    if(ToSeconds(SubtractTime(GetTime(), timestamp)) > 1.0f) {
      timestamp = GetTime();
      mAmberTOGGLE;
    }
  }
  return(0);
}


typedef enum{kGet = 0, kSet = 1, kSave = 2, kReply = 3} AccessMode;


// If I get a kTypeUsbRadio message, let ZigbeeInterface ReadMsg it as a 
// kTypeZigbeeInterface message.  (ZigbeeInterface isn't allowed to ReadMsg 
// normal kTypeZigbeeInterface messages.  This ensures the usb radio can pass 
// through these commands to remote devices but be itself unaffected.)
void ReadMsg(CommunicationInterface& com, uint8_t* rx_data, uint8_t rx_length, ZigbeeInterface& zig) {
  uint8_t type = rx_data[0];
  if(type == kTypeUsbRadio) {
    // uint8_t copy[5];
    // memcpy(copy, rx_data, 5);
    // copy[0] = kTypeZigbeeInterface;
    // zig.ReadMsg(com, rx_data, rx_length);


    uint8_t subtype = rx_data[1];
    // uint8_t msg_radio_number = rx_data[2]; // unused
    uint8_t mode = rx_data[3];
    switch(subtype) {
      case kSubSelfId:
        if(     mode == kSet) {
          zig.self_id = rx_data[4];
        }
        else if(mode == kGet) {
          ReplyByte(com, kSubSelfId, zig.self_id);
        }
        else if(mode == kSave) {
          zig.SaveSelfId();
        }
        break;
      case kSubRxChannel:
        if(     mode == kSet) {
          zig.rx_channel = rx_data[4];
          zig.SetModeRx();
        }
        else if(mode == kGet) {
          ReplyByte(com, kSubRxChannel, zig.rx_channel);
        }
        else if(mode == kSave) {
          zig.SaveRxChannel();
        }
        break;
      case kSubTxChannel:
        if(     mode == kSet) {
          zig.tx_channel = rx_data[4];
        }
        else if(mode == kGet) {
          ReplyByte(com, kSubTxChannel, zig.tx_channel);
        }
        else if(mode == kSave) {
          zig.SaveTxChannel();
        }
        break;
      case kSubDestId :
        if(     mode == kSet) {
          zig.dest_id = rx_data[4];
        }
        else if(mode == kGet) {
          ReplyByte(com, kSubDestId, zig.dest_id);
        }
        else if(mode == kSave) {
          zig.SaveDestId();
        }
        break;
    }
  }
}

void ReplyByte(CommunicationInterface& com, uint8_t subtype, uint8_t value) {
  uint8_t type = kTypeUsbRadio;
  uint8_t data[] = {0, 0, 3, 0};
  data[0] = subtype;
  data[1] = 0;
  data[2] = kReply;
  data[3] = value;
  com.SendPacket(type, data, 4);
}