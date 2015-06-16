#include "zigbee_interface.h"
#include "zigbee.h"
#include "crc_helper.h"
#include "zigbee_interface_msgs.h"
#include "stdio.h"

const uint16_t key_self_id    = (uint16_t)256*kTypeZigbeeInterface + kSubSelfId;
const uint16_t key_rx_channel = (uint16_t)256*kTypeZigbeeInterface + kSubRxChannel;
const uint16_t key_tx_channel = (uint16_t)256*kTypeZigbeeInterface + kSubTxChannel;
const uint16_t key_dest_id    = (uint16_t)256*kTypeZigbeeInterface + kSubDestId;


/*******************************************************************************
 * Initialize
 ******************************************************************************/
 
ZigbeeInterface::ZigbeeInterface()
{
  //InitZigbeeInterface();
}

void ZigbeeInterface::Init()
{
  InitBQ(&index_queue, pf_index_data, ZI_PF_INDEX_DATA_SIZE);
  InitPacketFinder(&pf, &index_queue);
  InitFQ(&out_queue, out_queue_data, ZI_OUT_QUEUE_DATA_SIZE, MAX_ZIGBEE_PACKET_BYTES);
  //SetSendInCallback(SendUsbNow); Missing zigbee version

  rx_channel = 73; // Init to default ch.
  tx_channel = 73; // Init to default ch.
  dest_id = 0xFF;  // default: I only send broadcast
  self_id = 0xFF;  // default: I only hear broadcast

  radio_number = 0;
  SetModeRx();
}

#ifdef USING_PERSISTENT_MEMORY
void ZigbeeInterface::Init(PersistentMemory& memory)
{
  // reserve memory
  mem = &memory;
  mem->AddKey(key_self_id);
  mem->AddKey(key_dest_id);
  mem->AddKey(key_rx_channel);
  mem->AddKey(key_tx_channel);

  // load values from memory, or defaults if the read fails
  uint32_t temp;
  temp = 0xFF;  // default: I only hear broadcast
    mem->Read(key_self_id, (uint32_t*)&temp);
    self_id = temp;
  temp = 0xFF;  // default: I only send broadcast
    mem->Read(key_dest_id, (uint32_t*)&temp);
    dest_id = temp;
  temp = 73;
    mem->Read(key_rx_channel, (uint32_t*)&temp);
    rx_channel = temp;
  temp = 73;
    mem->Read(key_tx_channel, (uint32_t*)&temp);
    tx_channel = temp;
  InitBQ(&index_queue, pf_index_data, ZI_PF_INDEX_DATA_SIZE);
  InitPacketFinder(&pf, &index_queue);
  InitFQ(&out_queue, out_queue_data, ZI_OUT_QUEUE_DATA_SIZE, MAX_ZIGBEE_PACKET_BYTES);

  radio_number = 0;
  SetModeRx();
}
#endif

/*******************************************************************************
 * Receive
 ******************************************************************************/
 
int8_t ZigbeeInterface::GetBytes()
{
  uint8_t receive_buffer[130]; // Max buffer size of AT86RF should be 130
  uint8_t receive_length = 0;

  int8_t status = 0;
  uint8_t zb_irq_stat = ZigBee_ReadIrq();
  if((zb_irq_stat & 0x08) == 0x08) {
    ZigBee_ReadFrame(receive_buffer, &receive_length);
    if(receive_length > 3) { // 3 status bytes from the radio + actual data
      if(receive_buffer[receive_length-1] & 0x80) { // Valid CRC
        status = PutBytes(&pf, receive_buffer, receive_length-5); // -3 for status bytes - 2 for zigbee's crc
      }
    }
  }
  return(status);
}

int8_t ZigbeeInterface::PeekPacket(uint8_t **packet, uint8_t *length) 
{
  int8_t ret = ::PeekPacket(&pf, packet, length);
  //loop until found non-filtered packet or queue empty
  while(ret != 0) // while we have packets, search for a valid message
  {
    if (((*packet)[0]) == kTypeRadio)//if packet is type radio 
    {
      MsgRadio *radio_msg = (MsgRadio*) &((*packet)[1]);
      if(radio_msg->receiver_id == self_id || radio_msg->receiver_id == 0xFF) //if is destined for our ID
      {
        //forward message
        *packet = &(radio_msg->type);
        *length = *length - 2; // remove the radio_msg's type and receiver_id
        dest_id = radio_msg->sender_id; // any replies should go to the sender of this message, 0xFF for everyone
        //ret remains the same
        break; // This is a valid result, so break to the return
      }
      else
      {
        //auto drop packet if filtered out
        ::DropPacket(&pf);
        ret = ::PeekPacket(&pf, packet, length); // look for another packet
        // back to the while loop
      }
    }
    else // we got a packet that's not type radio
    {
      dest_id = 0xFF;
      break; // Let's get out of here!
    }
  }
  return(ret);
}

int8_t ZigbeeInterface::DropPacket() 
{
  return(::DropPacket(&pf));
}

/*******************************************************************************
 * Send
 ******************************************************************************/

int8_t ZigbeeInterface::SendPacket(uint8_t msg_type, uint8_t *data, uint16_t length) 
{
  // This function must not be interrupted by another call to SendZigbeeBytes or 
  // SendZigbeePacket, or else the packet it builds will be spliced/corrupted.
  
  MsgRadio radio_msg; 
  uint8_t header[3];
  header[0] = kStartByte;                   // const defined by packet_finder.c
  header[1] = length;
  uint8_t footer[2];
  uint16_t crc;
  
  if (dest_id != 255) //if we're not supposed to broadcast
  {
    // encapsulate the message in a radio message
    header[2] = kTypeRadio;
    crc = MakeCrc(&(header[1]), 2);
    SendBytes(header, 3);
    radio_msg.receiver_id = dest_id;
    radio_msg.sender_id = self_id;
    radio_msg.type = msg_type;
    SendBytes((uint8_t*)&radio_msg, sizeof(MsgRadio)-1); // -1 because the data is added later
    crc = ArrayUpdateCrc(crc, (uint8_t*)&radio_msg, sizeof(MsgRadio)-1); // -1 because the data is added later
  }
  else
  {
    // send unencapsulated message
    header[2] = msg_type;
    crc = MakeCrc(&(header[1]), 2);
    SendBytes(header, 3);
  }
  
  SendBytes(data, length);
  crc = ArrayUpdateCrc(crc, data, length);
  
  footer[0] = crc & 0x00FF;
  footer[1] = crc >> 8;
  SendBytes(footer, 2);
  
  return(1);
}

int8_t ZigbeeInterface::SendBytes(uint8_t *bytes, uint16_t length)
{
  PutBytesFQ(&out_queue, bytes, length);
  
  uint8_t* next_packet;
  uint16_t next_length;
  PeekFrameFQ(&out_queue, &next_packet, &next_length);
  
  if(next_length >= MAX_ZIGBEE_PACKET_BYTES-1) {
    SendNow();
  }
  
  return(1);
}

void ZigbeeInterface::SendNow()
{
  //TODO:: write non-blocking zigbee code
  if(!IsEmptyFQ(&out_queue)) 
  {
    SetModeTx();  // does not guarentee mode reaches PLL_ON, so future WriteFrame may not work
    uint8_t* packet;
    uint16_t length;
    PeekFrameFQ(&out_queue, &packet, &length);
    ZigBee_WriteFrame(packet, length);
    DropFrameFQ(&out_queue);

    // as soon as status is busy we may write command to RX_ON
    // transition will occur automatically as soon transmission completes
    uint8_t status = ZigBee_ReadState();

    while(status != STATUS_BUSY_TX && status != STATUS_RX_ON)
    {
      status = ZigBee_ReadState();
      DelayMicroseconds(1);
    }

    ZigBee_WriteReg(REG_TRX_STATE, CMD_RX_ON);
  }
}

void ZigbeeInterface::SetModeTx()
{
  if(tx_channel != current_channel)
  {
    ZigBee_SetChannel(tx_channel);
    current_channel = tx_channel;
  }
  ZigBee_SetModeTx();
}

void ZigbeeInterface::SetModeRx()
{
  if(rx_channel != current_channel)
  {
    ZigBee_SetChannel(rx_channel);
    current_channel = rx_channel;
  }
  ZigBee_SetModeRx();
}


typedef enum{kGet = 0, kSet = 1, kSave = 2, kReply = 3} AccessMode;


void ZigbeeInterface::ReadMsg(CommunicationInterface& com, uint8_t* rx_data, uint8_t rx_length)
{
  // rx_length intentionally unused, probably should be used

  uint8_t type = rx_data[0];
  if(type == kTypeZigbeeInterface) {
    uint8_t subtype = rx_data[1];
    uint8_t msg_radio_number = rx_data[2];
    uint8_t mode = rx_data[3];

    // if message is for this radio
    if(radio_number == msg_radio_number || msg_radio_number == 0xFF) {
      switch(subtype) {
        case kSubSelfId:
          if(mode == kSet) {
            self_id = rx_data[4];
          }
          else if(mode == kGet) {
            ReplyByte(com, kSubSelfId, self_id);
            //printf("Id: %u\r\n",id);
          }
          #ifdef USING_PERSISTENT_MEMORY
          else if(mode == kSave) {
            mem->Write(key_self_id, self_id);
          }
          #endif
          break;
        case kSubRxChannel:
          if(mode == kSet) {
            rx_channel = rx_data[4];
            SetModeRx();
          }
          else if(mode == kGet) {
            ReplyByte(com, kSubRxChannel, rx_channel);
            //printf("rx ch: %u\r\n",rx_channel);
          }
          #ifdef USING_PERSISTENT_MEMORY
          else if(mode == kSave) {
            mem->Write(key_rx_channel, rx_channel);
          }
          #endif
          break;
        case kSubTxChannel:
          if(mode == kSet) {
            tx_channel = rx_data[4];
          }
          else if(mode == kGet) {
            ReplyByte(com, kSubTxChannel, tx_channel);
            //printf("tx ch: %u\r\n",tx_channel);
          }
          #ifdef USING_PERSISTENT_MEMORY
          else if(mode == kSave) {
            mem->Write(key_tx_channel, tx_channel);
          }
          #endif
          break;
        case kSubDestId :
          if(mode == kSet) {
            dest_id = rx_data[4];
          }
          else if(mode == kGet) {
            ReplyByte(com, kSubDestId, dest_id);
            //printf("destination id: %u\r\n",destination_id);
          }
          #ifdef USING_PERSISTENT_MEMORY
          else if(mode == kSave) {
            mem->Write(key_dest_id, dest_id);
          }
          #endif
          break;     
      }
    }
  }
}
#ifdef USING_PERSISTENT_MEMORY
void ZigbeeInterface::SaveSelfId() {
  mem->Write(key_self_id, self_id);
}

void ZigbeeInterface::SaveDestId() {
  mem->Write(key_dest_id, dest_id);
}

void ZigbeeInterface::SaveRxChannel() {
  mem->Write(key_rx_channel, rx_channel);
}

void ZigbeeInterface::SaveTxChannel() {
  mem->Write(key_tx_channel, tx_channel);
}
#endif
void ZigbeeInterface::ReplyByte(CommunicationInterface& com, uint8_t subtype, uint8_t value) {
  uint8_t type = kTypeZigbeeInterface;
  uint8_t data[] = {0, 0, 3, 0};
  data[0] = subtype;
  data[1] = 0;
  data[2] = kReply;
  data[3] = value;
  com.SendPacket(type, data, 4);
}