/** 
** This file provides an interface to each peripheral.
** Edit the #define statements at the top of the file to 
** enable and specifiy the location of the peripherals.
** The remainder of the file does the setting up of the peripherals.
**/

#ifndef PERIPHERALS_STM32F37X_H
#define PERIPHERALS_STM32F37X_H

#include <stdint.h>

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


// #define LED_BLUE_PORT F // Letter, ex: A, B, ...
// #define LED_BLUE_PIN 7 // Number, ex: 0, 1, 2, ...
//#define LED_BLUE_SET_ON // define iff on-when-set


// #define LED_CYAN_PORT A // Letter, ex: A, B, ...
// #define LED_CYAN_PIN 13 // Number, ex: 0, 1, 2, ...
//#define LED_CYAN_SET_ON // define iff on-when-set


#define LED_GREEN_PORT D // Letter, ex: A, B, ...
#define LED_GREEN_PIN 8 // Number, ex: 0, 1, 2, ...
//#define LED_GREEN_SET_ON // define iff on-when-set


#define LED_ORANGE_PORT F // Letter, ex: A, B, ...
#define LED_ORANGE_PIN 6 // Number, ex: 0, 1, 2, ...
#define LED_ORANGE_SET_ON // define iff on-when-set

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

/// IMU I2C Setup
/// Used with: Sensor_IMU_MPU-6050_I2C
#define IMU_I2C 1 //Number, 1 or 2
#define IMU_SCL_PORT A //Letter, ex: A
#define IMU_SCL_PIN 15 //Number, ex: 1
#define IMU_SDA_PORT A //Letter, ex: A
#define IMU_SDA_PIN 14 //Number, ex: 1

/// IMU SPI Setup
/// Used with: Sensor_IMU_MPU-6050_SPI

/// IMU Interface
#define USE_IMU_INTERFACE

/// ms Counter Setup

/// Pressure Sensor I2C Setup
/// Used with: Sensor_Pressure_MS5611-01BA03_I2C

/// Pressure Sensor SPI Setup
/// Used with: Sensor_Pressure_MS5611-01BA03_SPI

/// Quadrature Encoder Setup
/// Used with: Encoder_Rotary_Magnetic_AS5XXX
#define ENCODER_QEP_TIMER 3 //Number, 2, 3, 4, 5, 15 or 19
#define ENCODER_QEPA_PORT A //Letter, ex: A
#define ENCODER_QEPA_PIN 6 //Number, ex: 1
#define ENCODER_QEPA_AF 2 //Number, ex: 1
#define ENCODER_QEPB_PORT A //Letter, ex: A
#define ENCODER_QEPB_PIN 4 //Number, ex: 1
#define ENCODER_QEPB_AF 2 //Number, ex: 1
#define ENCODER_COUNTS 4096 //Number, ex: 4096
#define ENCODER_INDEX_PORT F //Letter, ex: A
#define ENCODER_INDEX_PIN 1 //Number, ex: 1
#define ENCODER_PWM_TIMER 15 //Number, 2, 3, 4, 5, 12, 15 or 19
#define ENCODER_PWM_PORT A //Letter, ex: A
#define ENCODER_PWM_PIN 3 //Number, ex: 1
#define ENCODER_PWM_AF 9 //Number, ex: 1
#define ENCODER_PWM_CHANNEL 2 //Number, 1 or 2

/// EEPROM emulation
/// Uses onboard flash to emulate EEPROM non volatile memory
/// The last 2 pages (2KB each) are used to store 32bit values
/// Requires uint16_t eeprom_addresses[EEPROM_NUMBER_VALUES] to be defined in user code with addresses filled in.
/// Please ensure your compiled code + 4KB is less than the microcontroller's flash size
#define EEPROM_NUMBER_VALUES 17 //Number of values stored.  1-340
#define FLASH_SIZE 128 //Number of KB of flash memory in purchased chip.  In name, STM32F37xyz z = 8, B, C > 64, 128, 256

typedef enum {EEPROM_MAX_PWM=0, EEPROM_MOTOR_SPEED_SMOOTHING_FACTOR, EEPROM_MOTOR_LEAD_ELEC_FACTOR, \
              EEPROM_MOTOR_ENCODER_CALIBRATION_ANGLE, EEPROM_POLE_PAIRS, \
              kEepromAccelOffsetX,  kEepromAccelOffsetY,  kEepromAccelOffsetZ, \
              kEepromAccelScaleX,   kEepromAccelScaleY,   kEepromAccelScaleZ, \
              kEepromGyroOffsetX,   kEepromGyroOffsetY,   kEepromGyroOffsetZ, \
              kEepromGyroScaleX,    kEepromGyroScaleY,    kEepromGyroScaleZ} eeprom_addresses_t;

/// Three Phase Inverter Setup
/// Used with 3x: Inverter_Half_H-Bridge_40A_50khz, Inverter_Half_H-Bridge_20A_50khz
#define SYNC_TIMER_HIGH 19 //Timer used for high side (2,3,4,5,19)
#define SYNC_TIMER_LOW 4 //Timer used for low side (2,3,4,5,19)
#define SYNC_TIMER_HIGH1_PORT A //Letter, ex: A
#define SYNC_TIMER_HIGH1_PIN 0 //Number, ex: 1
#define SYNC_TIMER_HIGH1_AF 11 //Number, ex: 1
#define SYNC_TIMER_HIGH2_PORT A //Letter, ex: A
#define SYNC_TIMER_HIGH2_PIN 1 //Number, ex: 1
#define SYNC_TIMER_HIGH2_AF 11 //Number, ex: 1
#define SYNC_TIMER_HIGH3_PORT A //Letter, ex: A
#define SYNC_TIMER_HIGH3_PIN 2 //Number, ex: 1
#define SYNC_TIMER_HIGH3_AF 11 //Number, ex: 1
#define SYNC_TIMER_LOW1_PORT B //Letter, ex: A
#define SYNC_TIMER_LOW1_PIN 6 //Number, ex: 1
#define SYNC_TIMER_LOW1_AF 2 //Number, ex: 1
#define SYNC_TIMER_LOW2_PORT B //Letter, ex: A
#define SYNC_TIMER_LOW2_PIN 7 //Number, ex: 1
#define SYNC_TIMER_LOW2_AF 2 //Number, ex: 1
#define SYNC_TIMER_LOW3_PORT B //Letter, ex: A
#define SYNC_TIMER_LOW3_PIN 8 //Number, ex: 1
#define SYNC_TIMER_LOW3_AF 2 //Number, ex: 1
#define SYNC_TIMER_DEADTIME 36 //Number in ticks.  Time = ticks/system speed = ticks/72000000

/// Time Base Setup
/// For each TIMEBASE_INTx_FREQ, create a timerCallbackx() function where the x's match numbers
/// The callbacks will be called at the desired TIMEBASE_INTx_FREQ 
/// Define the frequencies in descending frequency order wrt to INT#.
/// Do not skip INT#s.  First define INT1, then INT2, then INT3, then INT4.
#define TIMEBASE_TIMER 2
#define TIMEBASE_INT1_FREQ 10000 //Number 
#define TIMEBASE_INT2_FREQ 1000 //Number
// #define TIMEBASE_INT3_FREQ 1 //Number
// #define TIMEBASE_INT4_FREQ 1 //Number

/// System Clock Setup
/// If defined, the system_clock time functionality will be enabled.
/// Value must equal 2 or 5 (a 32 bit timer), or be left undefined to disable.
#define SYSTEM_CLOCK_TIMER 5

/// USB Setup
#define USING_USB // define if using USB
/// Used with Communication_Plug_USB_Micro_B_FCI_10103594-0001LF, Communication_Plug_USB_Mini_B_Wurth_651305142821
/// Enable these only if a USB software disconnect pin is used
//#define USB_DISCONNECT_PORT F //Letter, ex: A
//#define USB_DISCONNECT_PIN 1 //Number, ex: 1

/// Zigbee Setup (for zigbee, uncomment and fill in ? below)
/// Used with: Radio_AT86RF

#define ZIGBEE_SPI 3 //Number, ex: 1
#define ZIGBEE_SCK_PORT B //Letter, ex: A
#define ZIGBEE_SCK_PIN 3 //Number, ex: 1
#define ZIGBEE_SCK_AF 6 //Number, ex: 1
#define ZIGBEE_MISO_PORT B //Letter, ex: A
#define ZIGBEE_MISO_PIN 4 //Number, ex: 1
#define ZIGBEE_MISO_AF 6 //Number, ex: 1
#define ZIGBEE_MOSI_PORT B //Letter, ex: A
#define ZIGBEE_MOSI_PIN 5 //Number, ex: 1
#define ZIGBEE_MOSI_AF 6 //Number, ex: 1
#define ZIGBEE_SS_PORT C //Letter, ex: A
#define ZIGBEE_SS_PIN 14 //Number, ex: 1


#endif // PERIPHERALS_STM32F37X_H
