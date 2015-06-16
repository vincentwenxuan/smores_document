// Radio Band Regulations
//   Chinese WPAN,        779-787 MHz
//   European SRD,        863-870 MHz
//   North American ISM,  902-928 MHz
//   Japanese,            915-930 MHz
//
// Prefered North American Channels (1-10)
//   906, 908, 910, 912, 914, 916, 918, 920, 922, 924 MHz
//   CC_BAND = 0, CC_NUMBER = Not Used
//   CHANNEL = 1-10
//   1/4 Wavelength 82.7 mm to 81.1 mm
//
// General Frequency Configuration
//   833 - 935 MHz, increments of 1 MHz
//   CC_BAND = 5, CC_NUMBER = 0:102
//   CHANNEL = Not Used
//   Fc [MHz] = 833[MHz] + 1[MHz] x CC_NUMBER
//   North American Channels are k = 73, 75, 77, 79, 81, 83, 85, 87, 89, 91
//   1/4 Wavelength 90.0 mm to 80.2 mm
//
// I have 78.9 mm on computer, 80.5 mm on top, 81.0 mm on bottom

#ifndef ZIGBEE_INTERFACE_H
#define ZIGBEE_INTERFACE_H

#include "communication_interface.h"
#include "packet_finder.h"
#include "frame_queue.h"
#include "common_peripherals.h"
#include "persistent_memory.h"

#include <stdint.h>

#define ZI_PF_INDEX_DATA_SIZE 20
#define ZI_OUT_QUEUE_DATA_SIZE 252 // size of out buffer in usb_interface

class ZigbeeInterface: public CommunicationInterface {
  private:
    #ifdef USING_PERSISTENT_MEMORY
    PersistentMemory *mem;
    #endif
    uint8_t current_channel;    // the channel that is currently set
    uint8_t radio_number;       // the physical radio that this zigbee_interface instance is for
    typedef enum{kIdMsg = 0, kRxChannel = 1, kTxChannel = 2, kDestinationId = 3} RadioMessage;

    void ReplyByte(CommunicationInterface& com, uint8_t subtype, uint8_t value);

  public:
    /*******************************************************************************
     * Member Variables
     ******************************************************************************/
    uint8_t self_id;         // My id.  This is my name, don't wear it out.
    uint8_t rx_channel; // When listening, I listen on this channel.  
    uint8_t tx_channel; // When transmitting, I transmit on this channel.  
    uint8_t dest_id; // Send to this ID.  0xFF = I send to everyone.

    struct PacketFinder pf;         // packet_finder instance
    struct ByteQueue index_queue;   // needed by pf for storing indices
    struct FrameQueue out_queue;    // frame_queue instance for outgoing bytes
    uint8_t pf_index_data[ZI_PF_INDEX_DATA_SIZE];   // data for index_queue used by pf
    uint8_t out_queue_data[ZI_OUT_QUEUE_DATA_SIZE]; // data for out_queue

    /// Default Constructor, must call Init() after construction.
    ZigbeeInterface();
    
    /* Initialize the packetizing interface
     */    
    void Init();
    #ifdef USING_PERSISTENT_MEMORY
    void Init(PersistentMemory &mem);
    #endif
    
    /*******************************************************************************
     * Radio Control
     ******************************************************************************/
    /// Set mode to tx (and change channel if needed).
    void SetModeTx(void);
    
    /// Set mode to rx (and change channel if needed).
    void SetModeRx(void);

    #ifdef USING_PERSISTENT_MEMORY
    void SaveSelfId();

    void SaveDestId();

    void SaveRxChannel();

    void SaveTxChannel();
    #endif
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

    /// Initiate a hardware transmission, which will chain transmissions through 
    /// the endpoint IN interrupt until the buffer empties completely.
    void SendNow();
    
    /*******************************************************************************
     * Parsing
     ******************************************************************************/
     
    /// Read a given message and act appropriately.
    void ReadMsg(CommunicationInterface& com, uint8_t* data, uint8_t length);

}; // class ZigbeeInterface

#endif // ZIGBEE_INTERFACE_H
