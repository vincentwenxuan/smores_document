#ifndef PROJECT_PERIPHERALS_BRUSHED_COAX_H
#define PROJECT_PERIPHERALS_BRUSHED_COAX_H


////////////////////////////////////////////////////////////////////////////////
// Clock Configuration
#define M4         // define iff using any Penn Mechatronics M4
#define M4_WHITE   // define iff using a White Penn Mechatronics M4


////////////////////////////////////////////////////////////////////////////////
// Chip Flash Size (in kB, reference part number against datasheet)
#define FLASH_SIZE 256


////////////////////////////////////////////////////////////////////////////////
// System Clock Setup
// If defined, the system_clock time functionality will be enabled.
// Supported timer values: 2, 5, 6, 7 depending on device
// Suggest use of timer 7, a basic 16-bit timer
#define SYSTEM_CLOCK_TIMER   7


////////////////////////////////////////////////////////////////////////////////
// LED Configuration
// do not define unused LEDS
// eg. if LED_WHITE_PORT not defined, then mWhiteON compiles to whitespace

#define LED_BLUE_PORT   B 	// Letter, ex: A, B, ...
#define LED_BLUE_PIN    15 	// Number, ex: 0, 1, 2, ...
#define LED_BLUE_SET_ON   	// define iff on-when-set

#define LED_GREEN_PORT  C 	// Letter, ex: A, B, ...
#define LED_GREEN_PIN   13 	// Number, ex: 0, 1, 2, ...
//#define LED_GREEN_SET_ON  // define iff on-when-set

#define LED_RED_PORT    C 	// Letter, ex: A, B, ...
#define LED_RED_PIN     14 	// Number, ex: 0, 1, 2, ...
//#define LED_RED_SET_ON    // define iff on-when-set

#define LED_WHITE_PORT 	D 	// Letter, ex: A, B, ...
#define LED_WHITE_PIN 	8 	// Number, ex: 0, 1, 2, ...
//#define LED_WHITE_SET_ON 	// define iff on-when-set

#define LED_YELLOW_PORT C 	// Letter, ex: A, B, ...
#define LED_YELLOW_PIN 	15 	// Number, ex: 0, 1, 2, ...
//#define LED_YELLOW_SET_ON // define iff on-when-set


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

#endif // PROJECT_PERIPHERALS_H