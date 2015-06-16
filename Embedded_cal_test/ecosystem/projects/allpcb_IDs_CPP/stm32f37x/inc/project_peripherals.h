/** 
** This file provides an interface to each peripheral.
** Edit the #define statements at the top of the file to 
** enable and specifiy the location of the peripherals.
** The remainder of the file does the setting up of the peripherals.
**/

#ifndef PROJECT_PERIPHERALS_H
#define PROJECT_PERIPHERALS_H

#define LED_BLUE_PORT D // Letter, ex: A, B, ...
#define LED_BLUE_PIN 8 // Number, ex: 0, 1, 2, ...

#define LED_GREEN_PORT A // Letter, ex: A, B, ...
#define LED_GREEN_PIN 9 // Number, ex: 0, 1, 2, ...

#define LED_RED_PORT B // Letter, ex: A, B, ...
#define LED_RED_PIN 15 // Number, ex: 0, 1, 2, ...

#define LED_YELLOW_PORT A // Letter, ex: A, B, ...
#define LED_YELLOW_PIN 8 // Number, ex: 0, 1, 2, ...

#define USING_MBUS_MASTER_MODE  // uncomment to use feature

#define USING_MBUS1         // uncomment to use feature
#define MBUS1_SCL_PORT  A   //Letter, ex: A
#define MBUS1_SCL_PIN   15  //Number, ex: 15
#define MBUS1_SDA_PORT  A   //Letter, ex: A
#define MBUS1_SDA_PIN   14  //Number, ex: 14

/// IMU I2C Setup
/// Used with: Sensor_IMU_MPU-6050_I2C
#define IMU_MBUS 1 //Number, 1 or 2

/// IMU Interface
//#define USING_MPU_60XX_IMU

/// IMU SPI Setup
/// Used with: Sensor_IMU_MPU-6050_SPI

/// ms Counter Setup

/// Pressure Sensor I2C Setup
/// Used with: Sensor_Pressure_MS5611-01BA03_I2C

/// Pressure Sensor SPI Setup
/// Used with: Sensor_Pressure_MS5611-01BA03_SPI

/// EEPROM emulation
/// Uses onboard flash to emulate EEPROM non volatile memory
/// The last 2 pages (2KB each) are used to store 32bit values
/// Please ensure your compiled code + 4KB is less than the microcontroller's flash size

#define FLASH_SIZE 256 //Number of KB of flash memory in purchased chip.  In name, STM32F37xyz z = 8, B, C > 64, 128, 256

#define USING_DYNAMIC_EEPROM

//#define USING_PERSISTENT_MEMORY //  currently required by battery monitor
//#define USING_BATTERY_MONITOR

/// Time Base Setup
/// For each TIMEBASE_INTx_FREQ, create a timerCallbackx() function where the x's match numbers
/// The callbacks will be called at the desired TIMEBASE_INTx_FREQ 
/// Define the frequencies in descending frequency order wrt to INT#.
/// Do not skip INT#s.  First define INT1, then INT2, then INT3, then INT4.
//#define TIMEBASE_TIMER 2
//#define TIMEBASE_INT1_FREQ 10000 //Number 
//#define TIMEBASE_INT2_FREQ 1000 //Number
// #define TIMEBASE_INT3_FREQ 1 //Number
// #define TIMEBASE_INT4_FREQ 1 //Number

/// System Clock Setup
/// If defined, the system_clock time functionality will be enabled.
/// Value must equal 2 or 5 (a 32 bit timer), or be left undefined to disable.
#define SYSTEM_CLOCK_TIMER 5

//  motor PWM
#define PWM_TIMER   4
#define USING_PWM1
#define USING_PWM2
#define USING_PWM3
#define USING_PWM4

//
//TIM_SetCompare1(TIM4, pwm_m1);
//TIM_SetCompare4(TIM4, pwm_m2);
//TIM_SetCompare2(TIM4, pwm_m3);
//TIM_SetCompare3(TIM4, pwm_m4);
//

#define PWM1_PORT   B
#define PWM1_PIN    8
#define PWM1_AF     2

#define PWM2_PORT   F
#define PWM2_PIN    6
#define PWM2_AF     2

//#define PWM2_PORT   B
//#define PWM2_PIN    7
//#define PWM2_AF     2

#define PWM3_PORT   B
#define PWM3_PIN    6
#define PWM3_AF     2

//#define PWM3_PORT   F
//#define PWM3_PIN    6
//#define PWM3_AF     2

#define PWM4_PORT   B
#define PWM4_PIN    7
#define PWM4_AF     2

//#define PWM4_PORT   B
//#define PWM4_PIN    6
//#define PWM4_AF     2

#define PWM_FREQUENCY   20000



/// USB Setup
#define USING_USB // define if using USB

/// Zigbee Setup (for zigbee, uncomment and fill in ? below)
/// Used with: Radio_AT86RF
#define ZIGBEE_SPI 3 //Number, ex: 1
#define ZIGBEE_SCK_PORT A //Letter, ex: A
#define ZIGBEE_SCK_PIN 1 //Number, ex: 1
#define ZIGBEE_SCK_AF 6 //Number, ex: 1
#define ZIGBEE_MISO_PORT A //Letter, ex: A
#define ZIGBEE_MISO_PIN 2 //Number, ex: 1
#define ZIGBEE_MISO_AF 6 //Number, ex: 1
#define ZIGBEE_MOSI_PORT A //Letter, ex: A
#define ZIGBEE_MOSI_PIN 3 //Number, ex: 1
#define ZIGBEE_MOSI_AF 6 //Number, ex: 1
#define ZIGBEE_SS_PORT A //Letter, ex: A
#define ZIGBEE_SS_PIN 4 //Number, ex: 1

#endif // PROJECT_PERIPHERALS_H
