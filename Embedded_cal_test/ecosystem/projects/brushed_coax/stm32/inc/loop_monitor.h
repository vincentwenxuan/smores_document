#ifndef LOOP_MONITOR_H
#define LOOP_MONITOR_H

#include "persistent_memory.h"
#include <stdint.h>

#define NSTAMPS 7

class LoopMonitor {

  public:

    LoopMonitor();

    // Constructor with memory. Requires call to InitFromMemory() after Freeze()
    LoopMonitor(PersistentMemory& mem);
    
    // Initialize object from memory
    void InitFromMemory();
    
    // Read message and act
    void ReadMsg(CommunicationInterface& com, uint8_t* rx_data, uint8_t rx_length);

    void EndMainLoop();

    void Profile(uint8_t k);

    void SendProfile(CommunicationInterface& com);

    void SendFrequency(CommunicationInterface& com);

    uint8_t LoopIsAlive();

  private:
  
    PersistentMemory* mem;

    float target_loop_freq;         // desired main loop frequency
    uint32_t target_loop_period_us; // desired main loop period (microseconds)

    uint32_t loop_timestamp_us;     // achieved main loop timestamp
    uint32_t loop_period_us;        // achieved main loop period

    uint32_t profile_stamp_us[NSTAMPS];
    uint16_t dt[NSTAMPS-1];

    uint32_t is_started;
};
  
#endif // LOOP_MONITOR_H