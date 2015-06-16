#ifndef PROJECT_PERIPHERALS_BRUSHLESS_V1_H
#define PROJECT_PERIPHERALS_BRUSHLESS_V1_H

#define LED_GREEN_PORT D // Letter, ex: A, B, ...
#define LED_GREEN_PIN 8 // Number, ex: 0, 1, 2, ...
//#define LED_GREEN_SET_ON // define iff on-when-set

#define LED_RED_PORT A // Letter, ex: A, B, ...
#define LED_RED_PIN 9 // Number, ex: 0, 1, 2, ...
//#define LED_RED_SET_ON // define iff on-when-set

//Number of KB of flash memory in purchased chip.  In name, STM32F37xyz z = 8, B, C > 64, 128, 256
#define FLASH_SIZE 128

/// EEPROM emulation
/// Uses onboard flash to emulate EEPROM non volatile memory
/// The last 2 pages (2KB each) are used to store 32bit values
/// Please ensure your compiled code + 4KB is less than the microcontroller's flash size
#define USING_DYNAMIC_EEPROM
#define USING_PERSISTENT_MEMORY

/// System Clock Setup
/// If defined, the system_clock time functionality will be enabled.
/// Value must equal 2 or 5 (a 32 bit timer), or be left undefined to disable.
#define SYSTEM_CLOCK_TIMER 5

#endif // PROJECT_PERIPHERALS_BRUSHLESS_V1_H