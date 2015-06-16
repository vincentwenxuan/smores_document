/** 
** This file provides an interface to each peripheral.
** Edit the #define statements at the top of the file to 
** enable and specifiy the location of the peripherals.
** The remainder of the file does the setting up of the peripherals.
**/

#ifndef PROJECT_PERIPHERALS_STM32F37X_H
#define PROJECT_PERIPHERALS_STM32F37X_H

/// MAEVARM M4
// If compiling for an M4 module, uncomment the below
#define M4 // use for any M4
#define M4_WHITE // use for the white M4

/// Onboard LED Setup
/// do not define unused LEDS
/// eg. if LED_WHITE_PORT not defined, then mWhiteON compiles to whitespace

#define LED_BLUE_PORT B // Letter, ex: A, B, ...
#define LED_BLUE_PIN 15 // Number, ex: 0, 1, 2, ...
#define LED_BLUE_SET_ON // define iff on-when-set

#define LED_GREEN_PORT C // Letter, ex: A, B, ...
#define LED_GREEN_PIN 13 // Number, ex: 0, 1, 2, ...
//#define LED_GREEN_SET_ON // define iff on-when-set

#define LED_RED_PORT C // Letter, ex: A, B, ...
#define LED_RED_PIN 14 // Number, ex: 0, 1, 2, ...
//#define LED_RED_SET_ON // define iff on-when-set

#define LED_WHITE_PORT D // Letter, ex: A, B, ...
#define LED_WHITE_PIN 8 // Number, ex: 0, 1, 2, ...
//#define LED_WHITE_SET_ON // define iff on-when-set

#define LED_YELLOW_PORT C // Letter, ex: A, B, ...
#define LED_YELLOW_PIN 15 // Number, ex: 0, 1, 2, ...
//#define LED_YELLOW_SET_ON // define iff on-when-set


#define USING_MBUS_MASTER_MODE  // uncomment to use feature
#define USING_MBUS_SLAVE_MODE   // uncomment to use feature
#define USING_MBUS_LISTEN_MODE  // uncomment to use feature


// #define USING_MBUS1         // uncomment to use feature
// #define MBUS1_SCL_PORT  A   //Letter, ex: A
// #define MBUS1_SCL_PIN   15  //Number, ex: 15
// #define MBUS1_SDA_PORT  A   //Letter, ex: A
// #define MBUS1_SDA_PIN   14  //Number, ex: 14


#define USING_MBUS2         // uncomment to use feature
#define MBUS2_SCL_PORT  F   //Letter, ex: F
#define MBUS2_SCL_PIN   6   //Number, ex: 6
#define MBUS2_SDA_PORT  F   //Letter, ex: F
#define MBUS2_SDA_PIN   7   //Number, ex: 7


/// EEPROM emulation
/// Uses onboard flash to emulate EEPROM non volatile memory
/// The last 2 pages (2KB each) are used to store 32bit values
/// Please ensure your compiled code + 4KB is less than the microcontroller's flash size

#define FLASH_SIZE 256 //Number of KB of flash memory in purchased chip.  In name, STM32F37xyz z = 8, B, C > 64, 128, 256

#define USING_DYNAMIC_EEPROM

#define USING_PERSISTENT_MEMORY



/// System Clock Setup
/// If defined, the system_clock time functionality will be enabled.
/// Value must equal 2 or 5 (a 32 bit timer), or be left undefined to disable.
#define SYSTEM_CLOCK_TIMER 5


/// USB Setup
#define USING_USB // define if using USB
/// Used with Communication_Plug_USB_Micro_B_FCI_10103594-0001LF, Communication_Plug_USB_Mini_B_Wurth_651305142821
/// Enable these only if a USB software disconnect pin is used
#define USB_DISCONNECT_PORT F //Letter, ex: A
#define USB_DISCONNECT_PIN  1 //Number, ex: 1

#endif // PROJECT_PERIPHERALS_STM32F37X_H