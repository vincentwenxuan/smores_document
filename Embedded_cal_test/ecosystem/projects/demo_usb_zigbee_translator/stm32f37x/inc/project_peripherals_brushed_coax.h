// This file provides an interface to each peripheral.
// Edit the #define statements at the top of the file to 
// enable and specifiy the location of the peripherals.

#ifndef PROJECT_PERIPHERALS_BRUSHED_COAX_H
#define PROJECT_PERIPHERALS_BRUSHED_COAX_H


////////////////////////////////////////////////////////////////////////////////
// Clock Configuration
// #define M4         // define iff using any Penn Mechatronics M4
// #define M4_WHITE   // define iff using a White Penn Mechatronics M4


////////////////////////////////////////////////////////////////////////////////
// Chip Flash Size (in kB, reference part number against datasheet)
#define FLASH_SIZE 256


////////////////////////////////////////////////////////////////////////////////
// System Clock Setup
// If defined, the system_clock time functionality will be enabled.
// Value must equal 2 or 5 (a 32 bit timer), or be left undefined to disable.
#define SYSTEM_CLOCK_TIMER   2


////////////////////////////////////////////////////////////////////////////////
// LED Configuration
// do not define unused LEDS
// eg. if LED_WHITE_PORT not defined, then mWhiteON compiles to whitespace

#define LED_BLUE_PORT            B   // Letter, ex: A, B, ...
#define LED_BLUE_PIN             4   // Number, ex: 0, 1, 2, ...
#define LED_BLUE_SET_ON              // define iff on-when-set

#define LED_WHITE_PORT          LED_BLUE_PORT
#define LED_WHITE_PIN           LED_BLUE_PIN
#define LED_WHITE_SET_ON

#define LED_GREEN_PORT          B   // Letter, ex: A, B, ...
#define LED_GREEN_PIN           5   // Number, ex: 0, 1, 2, ...
#define LED_GREEN_SET_ON            // define iff on-when-set

#define LED_RED_PORT            B   // Letter, ex: A, B, ...
#define LED_RED_PIN             6   // Number, ex: 0, 1, 2, ...
#define LED_RED_SET_ON              // define iff on-when-set


////////////////////////////////////////////////////////////////////////////////
// Non Volatile Memory
// Uses onboard flash to emulate non volatile memory (like EEPROM)
// The last 2 pages (2KB each) are used to store 32bit values
// Ensure compiled code + 4KB is less than the microcontroller's FLASH_SIZE
#define USING_DYNAMIC_EEPROM
#define USING_PERSISTENT_MEMORY


////////////////////////////////////////////////////////////////////////////////
// USB Setup
#define USING_USB // define if using USB
// Define DISCONNECT values only if a USB software disconnect pin is used
// #define USB_DISCONNECT_PORT ? //Letter, ex: A
// #define USB_DISCONNECT_PIN  ? //Number, ex: 1


////////////////////////////////////////////////////////////////////////////////
// ZigBee Radio Setup
// Used with: Radio_AT86RF
#define ZIGBEE_SPI        1 //Number, ex: 1
#define ZIGBEE_SCK_PORT   A //Letter, ex: A
#define ZIGBEE_SCK_PIN    5 //Number, ex: 1
#define ZIGBEE_SCK_AF     5 //Number, ex: 1
#define ZIGBEE_MISO_PORT  A //Letter, ex: A
#define ZIGBEE_MISO_PIN   6 //Number, ex: 1
#define ZIGBEE_MISO_AF    5 //Number, ex: 1
#define ZIGBEE_MOSI_PORT  A //Letter, ex: A
#define ZIGBEE_MOSI_PIN   7 //Number, ex: 1
#define ZIGBEE_MOSI_AF    5 //Number, ex: 1
#define ZIGBEE_SS_PORT    A //Letter, ex: A
#define ZIGBEE_SS_PIN     4 //Number, ex: 1

#endif // PROJECT_PERIPHERALS_BRUSHED_COAX_H