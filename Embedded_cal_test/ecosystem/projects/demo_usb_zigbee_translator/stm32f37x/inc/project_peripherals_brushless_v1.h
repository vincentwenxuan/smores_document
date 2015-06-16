// This file provides an interface to each peripheral.
// Edit the #define statements at the top of the file to 
// enable and specifiy the location of the peripherals.

#ifndef PROJECT_PERIPHERALS_BRUSHLESS_V1_H
#define PROJECT_PERIPHERALS_BRUSHLESS_V1_H


////////////////////////////////////////////////////////////////////////////////
// Clock Configuration
// #define M4         // define iff using any Penn Mechatronics M4
// #define M4_WHITE   // define iff using a White Penn Mechatronics M4


////////////////////////////////////////////////////////////////////////////////
// Chip Flash Size (in kB, reference part number against datasheet)
#define FLASH_SIZE 128


////////////////////////////////////////////////////////////////////////////////
// System Clock Setup
// If defined, the system_clock time functionality will be enabled.
// Value must equal 2 or 5 (a 32 bit timer), or be left undefined to disable.
#define SYSTEM_CLOCK_TIMER   5


////////////////////////////////////////////////////////////////////////////////
// LED Configuration
// do not define unused LEDS
// eg. if LED_WHITE_PORT not defined, then mWhiteON compiles to whitespace

#define LED_AMBER_PORT          B   // Letter, ex: A, B, ...
#define LED_AMBER_PIN           15  // Number, ex: 0, 1, 2, ...
// #define LED_AMBER_SET_ON          // define iff on-when-set

// #define LED_BLUE_PORT        ?   // Letter, ex: A, B, ...
// #define LED_BLUE_PIN         ?   // Number, ex: 0, 1, 2, ...
// #define LED_BLUE_SET_ON          // define iff on-when-set

// #define LED_CYAN_PORT        ?   // Letter, ex: A, B, ...
// #define LED_CYAN_PIN         ?   // Number, ex: 0, 1, 2, ...
// #define LED_CYAN_SET_ON          // define iff on-when-set

#define LED_GREEN_PORT          D   // Letter, ex: A, B, ...
#define LED_GREEN_PIN           8   // Number, ex: 0, 1, 2, ...
// #define LED_GREEN_SET_ON          // define iff on-when-set

// #define LED_ORANGE_PORT      ?   // Letter, ex: A, B, ...
// #define LED_ORANGE_PIN       ?   // Number, ex: 0, 1, 2, ...
// #define LED_ORANGE_SET_ON        // define iff on-when-set

// #define LED_PINK_PORT        ?   // Letter, ex: A, B, ...
// #define LED_PINK_PIN         ?   // Number, ex: 0, 1, 2, ...
// #define LED_PINK_SET_ON          // define iff on-when-set

// #define LED_PURPLE_PORT      ?   // Letter, ex: A, B, ...
// #define LED_PURPLE_PIN       ?   // Number, ex: 0, 1, 2, ...
// #define LED_PURPLE_SET_ON        // define iff on-when-set

#define LED_RED_PORT            A   // Letter, ex: A, B, ...
#define LED_RED_PIN             9   // Number, ex: 0, 1, 2, ...
// #define LED_RED_SET_ON            // define iff on-when-set

// #define LED_TURQUOISE_PORT   ?   // Letter, ex: A, B, ...
// #define LED_TURQUOISE_PIN    ?   // Number, ex: 0, 1, 2, ...
// #define LED_TURQUOISE_SET_ON     // define iff on-when-set

#define LED_WHITE_PORT          A   // Letter, ex: A, B, ...
#define LED_WHITE_PIN           8   // Number, ex: 0, 1, 2, ...
// #define LED_WHITE_SET_ON          // define iff on-when-set

// #define LED_YELLOW_PORT      ?   // Letter, ex: A, B, ...
// #define LED_YELLOW_PIN       ?   // Number, ex: 0, 1, 2, ...
// #define LED_YELLOW_SET_ON        // define iff on-when-set


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
#define ZIGBEE_SPI        3 //Number, ex: 1
#define ZIGBEE_SCK_PORT   B //Letter, ex: A
#define ZIGBEE_SCK_PIN    3 //Number, ex: 1
#define ZIGBEE_SCK_AF     6 //Number, ex: 1
#define ZIGBEE_MISO_PORT  B //Letter, ex: A
#define ZIGBEE_MISO_PIN   4 //Number, ex: 1
#define ZIGBEE_MISO_AF    6 //Number, ex: 1
#define ZIGBEE_MOSI_PORT  B //Letter, ex: A
#define ZIGBEE_MOSI_PIN   5 //Number, ex: 1
#define ZIGBEE_MOSI_AF    6 //Number, ex: 1
#define ZIGBEE_SS_PORT    C //Letter, ex: A
#define ZIGBEE_SS_PIN     14 //Number, ex: 1


#endif // PROJECT_PERIPHERALS_BRUSHLESS_V1_H