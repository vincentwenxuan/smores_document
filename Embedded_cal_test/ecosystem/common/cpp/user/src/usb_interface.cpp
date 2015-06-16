#include "usb_interface.h"
#include "crc_helper.h"
#include "mUSB.h"
#include <stdarg.h>

UsbInterface* usb_interface_global;

void UsbSendNow()
{
  usb_interface_global->SendNow();
}

/*******************************************************************************
 * Initialize
 ******************************************************************************/
UsbInterface::UsbInterface()
{

}

void UsbInterface::Init() {
  InitBQ(&index_queue, pf_index_data, PF_INDEX_DATA_SIZE);
  InitPacketFinder(&pf, &index_queue);
  InitFQ(&out_queue, out_queue_data, OUT_QUEUE_DATA_SIZE, VIRTUAL_COM_PORT_DATA_SIZE-1);
  usb_interface_global = this;
  SetSendInCallback(UsbSendNow);
}

/*******************************************************************************
 * Receive
 ******************************************************************************/

int8_t UsbInterface::GetBytes() {

  CDC_Receive_DATA();
  if(0 != Receive_length) {
    // Receive_Buffer and Receive_length are extern, filled from mUSB.c
    int8_t status = PutBytes(&pf, Receive_Buffer, Receive_length);
    Receive_length = 0;
    return(status);
  }
  else
    return(0);
}

int8_t UsbInterface::PeekPacket(uint8_t **packet, uint8_t *length) {
  return(::PeekPacket(&pf, packet, length));
}

int8_t UsbInterface::DropPacket() {
  return(::DropPacket(&pf));
}

/*******************************************************************************
 * Send
 ******************************************************************************/

int8_t UsbInterface::SendPacket(uint8_t msg_type, uint8_t *data, uint16_t length) {
  // This function must not be interrupted by another call to SendBytes or 
  // SendPacket, or else the packet it builds will be spliced/corrupted.

  uint8_t header[3];
  header[0] = kStartByte;                   // const defined by packet_finder.c
  header[1] = length;
  header[2] = msg_type;
  SendBytes(header, 3);
  
  SendBytes(data, length);
  
  uint8_t footer[2];
  uint16_t crc;
  crc = MakeCrc(&(header[1]), 2);
  crc = ArrayUpdateCrc(crc, data, length);
  footer[0] = crc & 0x00FF;
  footer[1] = crc >> 8;
  SendBytes(footer, 2);
  
  return(1);
}


int8_t UsbInterface::SendBytes(uint8_t *bytes, uint16_t length) {
  
  PutBytesFQ(&out_queue, bytes, length);
  
  uint8_t* next_packet;
  uint16_t next_length;
  PeekFrameFQ(&out_queue, &next_packet, &next_length);
  
  if(next_length >= VIRTUAL_COM_PORT_DATA_SIZE-1) {
    SendNow();
  }
  
  return(1);
}

void UsbInterface::printf(const char* format, ...) {
  uint8_t buff[128];
  va_list argptr;
  va_start(argptr, format);
  uint8_t length = vsnprintf((char*)buff, 128, format, argptr);
  va_end(argptr);
  SendBytes(buff, length);
}


void UsbInterface::SendNow() {
  // This function is the only code point that removes bytes from the buffer.
  if(!(GetEPTxStatus(ENDP1)==EP_TX_VALID) && !IsEmptyFQ(&out_queue)) {
    uint8_t* packet;
    uint16_t length;
    PeekFrameFQ(&out_queue, &packet, &length);
    CDC_Send_DATA(packet, length);
    DropFrameFQ(&out_queue); 
  }
}

void UsbInterface::ReadMsg(CommunicationInterface& com, uint8_t* rx_data, uint8_t rx_length)
{
  // com        intentionally unused
  // rx_data    intentionally unused
  // rx_length  intentionally unused
}