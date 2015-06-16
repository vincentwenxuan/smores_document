#include "persistent_memory.h"
#include "dynamic_eeprom.h"
#include "common_message_types.h"
#include <stdio.h>

#include "common_peripherals.h"

#ifdef USING_PERSISTENT_MEMORY

// command types
const uint8_t kTypeFormat = 0;

PersistentMemory::PersistentMemory():
  next_key(0),
  is_frozen(0)
{
}

uint16_t PersistentMemory::AddKey(uint16_t key_value) {
  keys[next_key] = key_value;
  next_key++;
  return 1;
}

void PersistentMemory::Freeze() {
  is_frozen = 1;
  DynamicEE_Init(keys, next_key);
}

/// @retval Success or error status:
///   - 0: no write required
///   - FLASH_COMPLETE: on success
///   - PAGE_FULL: if valid page is full
///   - NO_VALID_PAGE: if no valid page was found
///   - Flash error code: on write Flash error
uint16_t PersistentMemory::Write(uint16_t key, uint32_t new_value) {
  uint16_t retval;
  uint32_t old_value;
  retval = Read(key, &old_value);
  if(old_value != new_value) {
    retval = DynamicEE_WriteVariable(keys, next_key, key, new_value);
    return retval;
  }
  else {
    return 0;
  }
}

/// @retval Success or error status:
///   - 0: if variable was found
///   - 1: if the variable was not found
///   - NO_VALID_PAGE: if no valid page was found.
uint16_t PersistentMemory::Read(uint16_t key, uint32_t* value)  {
  uint16_t retval = DynamicEE_ReadVariable(key, value);
  return retval;
}

void PersistentMemory::Format() {
  DynamicEE_Format();
}

void PersistentMemory::ReadMsg(CommunicationInterface& com, uint8_t* rx_data, uint8_t rx_length) {
  // com intentionally unused
  // rx_length intentionally unused, probably should be used

  uint8_t type = rx_data[0];
  if(type == kTypePersistentMemory) {
    MsgPersistentMemory *data_in = (MsgPersistentMemory*) &rx_data[1];
    if(data_in->command == kTypeFormat) {
      Format();
    }
  }
}

#endif // USING_PERSISTENT_MEMORY