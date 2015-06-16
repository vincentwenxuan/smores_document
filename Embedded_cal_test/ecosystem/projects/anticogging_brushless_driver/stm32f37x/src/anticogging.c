#include "common_peripherals.h"
#include "stm32f37x_flash.h"
#include "stm32f37x.h"
#include <string.h>

#ifndef PI
  #define PI 3.14159265358979f
#endif

#ifdef USING_ANTICOGGING

/* Define the STM32F3xx Flash page size depending on the used STM32 device */
#ifndef PAGE_SIZE
  #define PAGE_SIZE  (uint16_t)0x800  /* Page size = 2KByte */
#endif

/* ANTICOGGING start address in Flash */
#ifndef ENCODER_COUNTS
  #error "Anticogging requires ENCODER_COUNTS to be defined"
#else
  #define ANTICOGGING_EEPROM_START ((uint32_t)0x08000000 + (FLASH_SIZE/2-2)*PAGE_SIZE)
  #define ANTICOGGING_START_ADDRESS (ANTICOGGING_EEPROM_START - ENCODER_COUNTS*4)
#endif

float GetAnticogging(uint16_t index)
{
  float* anticogging_array = (float*)ANTICOGGING_START_ADDRESS;
  return anticogging_array[index];
}

float GetAnticoggingFloat(float mech_angle)
{
  // mech_angle = 0-2*PI
  // map to index, 0 to ENCODER_COUNTS
  uint16_t index = (uint16_t)(mech_angle*ENCODER_COUNTS/(2*PI));
  if(index >= ENCODER_COUNTS)
    return GetAnticogging(0);
  return GetAnticogging(index);
}

int8_t SetAnticogging(uint16_t index, float value)
{
  uint32_t* anticogging_array = (uint32_t*)ANTICOGGING_START_ADDRESS;
  if(anticogging_array[index] == 0xFFFFFFFF) // if the flash is erased
  {
    uint32_t new_value;
    memcpy(&new_value,&value,4);
    FLASH_Unlock();
    FLASH_ProgramWord((uint32_t)&anticogging_array[index],new_value);
    FLASH_Lock();
    return SUCCESS;
  }
  else
  {
    return ERROR;
  }
}

void EraseAnticogging()
{
  uint16_t addresses_erased = 0;
  __disable_irq();
  FLASH_Unlock();
  while(addresses_erased < ENCODER_COUNTS*4)
  {
    FLASH_ErasePage(ANTICOGGING_START_ADDRESS + addresses_erased);
    addresses_erased = addresses_erased + PAGE_SIZE;
  }
  FLASH_Lock();
  __enable_irq();
}

#endif // USING_ANTICOGGING