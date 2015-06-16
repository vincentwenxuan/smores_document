#ifndef PERSISTENT_MEMORY_H
#define PERSISTENT_MEMORY_H

#include "common_peripherals.h"
#include "communication_interface.h"
#include <stdint.h>

#ifdef USING_PERSISTENT_MEMORY

#define VAR_FOUND     0
#define VAR_NOT_FOUND 1

class PersistentMemory {

  public:
    PersistentMemory();     // construct memory object, one per program
    
    uint16_t AddKey(uint16_t key);  // set a unique virtual address for memory access

    void Freeze();          // freeze table, no more entry additions allowed
                            //   table must be frozen before use
    uint16_t Write(uint16_t key, uint32_t value);  // write value
    
    uint16_t Read(uint16_t key, uint32_t* value);  // read value
    
    void Format();          // format storage, all values lost
    
    void ReadMsg(CommunicationInterface& com, uint8_t* rx_data, uint8_t rx_length);
    
  private:
    uint16_t next_key;  // next key allocated
    uint8_t is_frozen;  // if frozen, no entries may be added (not implemented)
    uint16_t keys[128];  // excess length fixed size list of keys
};

#endif // PERSISTENT_MEMORY_H

#endif // USING_PERSISTENT_MEMORY