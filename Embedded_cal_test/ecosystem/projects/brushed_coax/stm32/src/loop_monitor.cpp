#include "project_message_types.h"
#include "communication_interface.h"
#include "persistent_memory.h"
#include "math_helper.h"
#include "loop_monitor.h"

#define MAIN_FREQ_DEFAULT 500.0f

const uint32_t kLoopKeepAliveDuration = 2000000;   // microseconds

// Persistent memory key values (based on component type and subtypes)
const uint16_t key_target_loop_freq = (uint16_t)256*kTypeLoopMonitor + kSubtypeTargetLoopFreq;

// Default Constructor (Useless)
LoopMonitor::LoopMonitor()
{
}

// Memory Constructor: Follow with InitFromMemory() after Freeze()
LoopMonitor::LoopMonitor(PersistentMemory& memory):
  target_loop_freq(MAIN_FREQ_DEFAULT),                  // default flash parameter
  target_loop_period_us(1.0e6f/MAIN_FREQ_DEFAULT),  // default derived value
  loop_timestamp_us(0),
  loop_period_us(0),
  is_started(0)
{
  mem = &memory;
  mem->AddKey(key_target_loop_freq);
}

// Initialize object from memory
void LoopMonitor::InitFromMemory() {
  if(mem->Read(key_target_loop_freq, (uint32_t*)&target_loop_freq) == VAR_FOUND) {
    target_loop_period_us = (uint32_t)(1.0e6f/target_loop_freq);
  }
}

// Read Messages
void LoopMonitor::ReadMsg(CommunicationInterface& com, uint8_t* rx_data, uint8_t rx_length) {
  uint8_t type = rx_data[0];

  if(type == kTypeLoopMonitor && rx_length >= 3) {
    uint8_t subtype = rx_data[1];
    uint8_t direction = rx_data[2];
      
    if(direction == GET_CMD) {
      uint8_t tx_msg[6];
      tx_msg[0] = subtype;
      tx_msg[1] = RESPONSE_CMD;
      switch(subtype) {
        case kSubtypeTargetLoopFreq:
          memcpy(&tx_msg[2], &target_loop_freq, 4);
          com.SendPacket(kTypeLoopMonitor, tx_msg, 6); break;
      }
    } // end if get

    else if(direction == SET_CMD) {
      switch(subtype) {
        case kSubtypeTargetLoopFreq:
          target_loop_freq =      PunToFloat(&rx_data[3]);
          target_loop_period_us = (uint32_t)(1.0e6f/target_loop_freq); break;
      }
    } // end if set

    else if(direction == SAVE_CMD) {
      switch(subtype) {
        case kSubtypeTargetLoopFreq:
          mem->Write(key_target_loop_freq,  PunToUint32(target_loop_freq)); break;
      }
    } // end if save
  }
}

// Block to throttle main loop frequency, and mark achieved loop frequency.
void LoopMonitor::EndMainLoop() {
  if(is_started) {
    uint32_t next = loop_timestamp_us + target_loop_period_us;  // block-until time
    uint32_t temp = GetMicroseconds();
    while(temp <= next) {temp = GetMicroseconds();}   // block and fetch time
    loop_period_us = temp - loop_timestamp_us;        // calculate loop period
    loop_timestamp_us = temp;                         // update loop timestamp
  }
  else {
    loop_timestamp_us = GetMicroseconds();
    is_started = 1;
  }
}

void LoopMonitor::SendProfile(CommunicationInterface& com) {
  com.SendPacket(kTypeLoopProfile, (uint8_t*)dt, (NSTAMPS-1)*sizeof(uint16_t));
}

void LoopMonitor::SendFrequency(CommunicationInterface& com) {
  MsgLoopFreq tx_msg;
  tx_msg.loop_freq = 1.0e6f / loop_period_us;
  com.SendPacket(kTypeLoopFreq, (uint8_t*)&tx_msg, sizeof(MsgLoopFreq));
}

void LoopMonitor::Profile(uint8_t k) {
  profile_stamp_us[k] = GetMicroseconds();

  if(k == NSTAMPS-1) {
    for(k = 0; k<NSTAMPS-2; k++) {
      dt[k] = profile_stamp_us[k+1] - profile_stamp_us[k];
    }
  }
}

uint8_t LoopMonitor::LoopIsAlive() {
  return !is_started || (GetMicroseconds() < loop_timestamp_us + kLoopKeepAliveDuration);
}