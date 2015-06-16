#ifndef USB_INTERFACE_H
#define	USB_INTERFACE_H

#include "communication_interface.h"
#include "packet_finder.h"
#include "frame_queue.h"
#include <stdint.h>

#define PF_INDEX_DATA_SIZE 20   // size of index buffer in packet_finder
#define OUT_QUEUE_DATA_SIZE 252 // size of out buffer in usb_interface

class UsbInterface: public CommunicationInterface{
  private:
    
  public:
    /*******************************************************************************
     * Member Variables
     ******************************************************************************/
    struct PacketFinder pf;        // packet_finder instance

    struct ByteQueue index_queue;              // needed by pf for storing indices
    uint8_t pf_index_data[PF_INDEX_DATA_SIZE]; // data for index_queue used by pf

    struct FrameQueue out_queue;   // frame_queue instance for outgoing bytes
    uint8_t out_queue_data[OUT_QUEUE_DATA_SIZE];  // data for out_queue
    
    /// Default Constructor, must call Init() after construction.
    UsbInterface();
    
    /* Initialize the packetizing interface
     */    
    void Init();
    
    /// Inherited functions
    
    /*******************************************************************************
     * Receive
     ******************************************************************************/

    /// Poll the hardware for new byte data.
    ///   Returns: 1 packet ready
    ///            0 normal operation
    ///           -1 failure
    ///
    int8_t GetBytes();

    /// Peek at the next available incoming packet. If a packet is ready, pointer 
    /// 'packet' will point to the first byte of type+data and 'length' will give 
    /// the length of packet type+data. Arguments 'packet' and 'length' are ignored 
    /// if no packet is ready.  Repeated calls to Peek will return pointers to the 
    /// same packet data until Drop is used.
    ///   Returns: 1 packet peek available
    ///            0 no packet peek available
    ///           -1 failure
    ///
    int8_t PeekPacket(uint8_t **packet, uint8_t *length);

    /// Drop the next available packet from queue. Usually called after Peek.
    ///   Returns: 1 packet removed
    ///            0 no packet ready to remove
    ///           -1 failure
    ///
    int8_t DropPacket();


    /*******************************************************************************
     * Send
     ******************************************************************************/

    /// Add a packet to the outgoing USB queue with automatically generated header 
    /// and CRC. A hardware transmission is not immediately initiated unless the 
    /// endpoint is filled. To force a transmission, follow with SendNow(). This 
    /// operation is nonblocking. If the buffer fills, the most recent data is lost.
    int8_t SendPacket(uint8_t msg_type, uint8_t *data, uint16_t length);

    /// Add bytes to the outgoing USB queue. A hardware transmission is not 
    /// immediately initiated unless the endpoint is filled. To force a 
    /// transmission, follow with SendUsbNow(). This operation is 
    /// nonblocking. If the buffer fills, the most recent data is lost.
    int8_t SendBytes(uint8_t *bytes, uint16_t length);

    /// Add a (unterminated) string created through the standard printf syntax 
    /// to the outgoing USB queue.  A hardware transmission is not immediately 
    /// initiated unless the endpoint is filled. To force a transmission, follow 
    /// with SendUsbNow(). This operation is nonblocking. If the buffer fills, 
    /// the most recent data is lost.
    void printf(const char* format, ...);

    /// Initiate a hardware transmission, which will chain transmissions through 
    /// the endpoint IN interrupt until the buffer empties completely.
    void SendNow();
    
    /*******************************************************************************
     * Parsing
     ******************************************************************************/
     
    /// Read a given message and act appropriately.
    void ReadMsg(CommunicationInterface& com, uint8_t* data, uint8_t length);
}; // class UsbInterface
#endif // USB_INTERFACE_H
