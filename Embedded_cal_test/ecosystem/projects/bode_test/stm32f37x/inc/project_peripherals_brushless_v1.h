/** 
** This file provides an interface to each peripheral.
** Edit the #define statements at the top of the file to 
** enable and specifiy the location of the peripherals.
** The remainder of the file does the setting up of the peripherals.
**/

#ifndef PROJECT_PERIPHERALS_BRUSHLESS_V1_H
#define PROJECT_PERIPHERALS_BRUSHLESS_V1_H

/// MAEVARM M4
// If compiling for an M4 module, uncomment the below
//#define M4 // use for any M4
//#define M4_WHITE // use for the white M4


/// Onboard LED Setup
/// do not define unused LEDS
/// eg. if LED_WHITE_PORT not defined, then mWhiteON compiles to whitespace

#define LED_AMBER_PORT B // Letter, ex: A, B, ...
#define LED_AMBER_PIN 15 // Number, ex: 0, 1, 2, ...
//#define LED_AMBER_SET_ON // define iff on-when-set

/*
#define LED_BLUE_PORT ? // Letter, ex: A, B, ...
#define LED_BLUE_PIN ? // Number, ex: 0, 1, 2, ...
#define LED_BLUE_SET_ON // define iff on-when-set
*/
/*
#define LED_CYAN_PORT ? // Letter, ex: A, B, ...
#define LED_CYAN_PIN ? // Number, ex: 0, 1, 2, ...
#define LED_CYAN_SET_ON // define iff on-when-set
*/

#define LED_GREEN_PORT D // Letter, ex: A, B, ...
#define LED_GREEN_PIN 8 // Number, ex: 0, 1, 2, ...
//#define LED_GREEN_SET_ON // define iff on-when-set

/*
#define LED_ORANGE_PORT ? // Letter, ex: A, B, ...
#define LED_ORANGE_PIN ? // Number, ex: 0, 1, 2, ...
//#define LED_ORANGE_SET_ON // define iff on-when-set
*/
/*
#define LED_PINK_PORT ? // Letter, ex: A, B, ...
#define LED_PINK_PIN ? // Number, ex: 0, 1, 2, ...
//#define LED_PINK_SET_ON // define iff on-when-set
*/
/*
#define LED_PURPLE_PORT ? // Letter, ex: A, B, ...
#define LED_PURPLE_PIN ? // Number, ex: 0, 1, 2, ...
#define LED_PURPLE_SET_ON // define iff on-when-set
*/

#define LED_RED_PORT A // Letter, ex: A, B, ...
#define LED_RED_PIN 9 // Number, ex: 0, 1, 2, ...
//#define LED_RED_SET_ON // define iff on-when-set

/*
#define LED_TURQUOISE_PORT ? // Letter, ex: A, B, ...
#define LED_TURQUOISE_PIN ? // Number, ex: 0, 1, 2, ...
#define LED_TURQUOISE_SET_ON // define iff on-when-set
*/

#define LED_WHITE_PORT A // Letter, ex: A, B, ...
#define LED_WHITE_PIN 8 // Number, ex: 0, 1, 2, ...
//#define LED_WHITE_SET_ON // define iff on-when-set

/*
#define LED_YELLOW_PORT ? // Letter, ex: A, B, ...
#define LED_YELLOW_PIN ? // Number, ex: 0, 1, 2, ...
#define LED_YELLOW_SET_ON // define iff on-when-set
*/

/// Number of KB of flash memory in purchased chip.  In name, STM32F37xyz z = 8, B, C > 64, 128, 256
#define FLASH_SIZE 128

/// System Clock Setup
/// If defined, the system_clock time functionality will be enabled.
/// Value must equal 2 or 5 (a 32 bit timer), or be left undefined to disable.
#define SYSTEM_CLOCK_TIMER 5

/// USB Setup
#define USING_USB // define if using USB
/// Used with Communication_Plug_USB_Micro_B_FCI_10103594-0001LF, Communication_Plug_USB_Mini_B_Wurth_651305142821
/// Enable these only if a USB software disconnect pin is used
// #define USB_DISCONNECT_PORT F //Letter, ex: A
// #define USB_DISCONNECT_PIN 1 //Number, ex: 1

#endif // PROJECT_PERIPHERALS_BRUSHLESS_V1_H