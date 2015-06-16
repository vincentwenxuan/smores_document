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
#define FLASH_SIZE 256


////////////////////////////////////////////////////////////////////////////////
// System Clock Setup
// If defined, the system_clock time functionality will be enabled.
// Supported timer values: 2, 5, 6, 7 depending on device
// Suggest use of timer 7, a basic 16-bit timer
#define SYSTEM_CLOCK_TIMER 7


////////////////////////////////////////////////////////////////////////////////
// LED Configuration
// do not define unused LEDS
// eg. if LED_WHITE_PORT not defined, then mWhiteON compiles to whitespace

#define LED_AMBER_PORT          B   // Letter, ex: A, B, ...
#define LED_AMBER_PIN           15  // Number, ex: 0, 1, 2, ...
// #define LED_AMBER_SET_ON          // define iff on-when-set

#define LED_GREEN_PORT          D   // Letter, ex: A, B, ...
#define LED_GREEN_PIN           8   // Number, ex: 0, 1, 2, ...
// #define LED_GREEN_SET_ON          // define iff on-when-set

#define LED_RED_PORT            A   // Letter, ex: A, B, ...
#define LED_RED_PIN             9   // Number, ex: 0, 1, 2, ...
// #define LED_RED_SET_ON            // define iff on-when-set

#define LED_WHITE_PORT          A   // Letter, ex: A, B, ...
#define LED_WHITE_PIN           8   // Number, ex: 0, 1, 2, ...
// #define LED_WHITE_SET_ON          // define iff on-when-set


////////////////////////////////////////////////////////////////////////////////
// I2C Configuration

#define USING_MBUS_MASTER_MODE  // uncomment to use feature
// #define USING_MBUS_SLAVE_MODE   // uncomment to use feature
// #define USING_MBUS_LISTEN_MODE  // uncomment to use feature

#define USING_MBUS1         // uncomment to use feature
#define MBUS1_SCL_PORT  A   // Letter, ex: A
#define MBUS1_SCL_PIN   15  // Number, ex: 15
#define MBUS1_SDA_PORT  A   // Letter, ex: A
#define MBUS1_SDA_PIN   14  // Number, ex: 14

#define USING_MBUS2         // uncomment to use feature
#define MBUS2_SCL_PORT  F   // Letter, ex: F
#define MBUS2_SCL_PIN   6   // Number, ex: 6
#define MBUS2_SDA_PORT  F   // Letter, ex: F
#define MBUS2_SDA_PIN   7   // Number, ex: 7

// IMU I2C Setup
// Used with: Sensor_IMU_MPU-6050_I2C
#define IMU_MBUS 1 // Number, 1 or 2

// IMU Interface
#define USING_MPU_60XX_IMU


////////////////////////////////////////////////////////////////////////////////
// Quadrature Encoder Setup
// Used with: Encoder_Rotary_Magnetic_AS5XXX
#define ENCODER_QEP_TIMER   3     //Number, 2, 3, 4, 5, 15 or 19
#define ENCODER_QEPA_PORT   A     //Letter, ex: A
#define ENCODER_QEPA_PIN    6     //Number, ex: 1
#define ENCODER_QEPA_AF     2     //Number, ex: 1
#define ENCODER_QEPB_PORT   A     //Letter, ex: A
#define ENCODER_QEPB_PIN    4     //Number, ex: 1
#define ENCODER_QEPB_AF     2     //Number, ex: 1
#define ENCODER_COUNTS      4096  //Number, ex: 4096
#define ENCODER_INDEX_PORT  F     //Letter, ex: A
#define ENCODER_INDEX_PIN   1     //Number, ex: 1
#define ENCODER_PWM_TIMER   15    //Number, 2, 3, 4, 5, 12, 15 or 19
#define ENCODER_PWM_PORT    A     //Letter, ex: A
#define ENCODER_PWM_PIN     3     //Number, ex: 1
#define ENCODER_PWM_AF      9     //Number, ex: 1
#define ENCODER_PWM_CHANNEL 2     //Number, 1 or 2
#define ENCODER_REVERSE_DIRECTION // define to reverse encoder direction


////////////////////////////////////////////////////////////////////////////////
// Non Volatile Memory
// Uses onboard flash to emulate non volatile memory (like EEPROM)
// The last 2 pages (2KB each) are used to store 32bit values
// Ensure compiled code + 4KB is less than the microcontroller's FLASH_SIZE
#define USING_DYNAMIC_EEPROM
#define USING_PERSISTENT_MEMORY


////////////////////////////////////////////////////////////////////////////////
// Battery Monitor
#define USING_BATTERY_MONITOR


////////////////////////////////////////////////////////////////////////////////
// Three Phase Inverter Setup
// Used with 3x: Inverter_Half_H-Bridge_40A_50khz, Inverter_Half_H-Bridge_20A_50khz
#define SYNC_TIMER_HIGH       19  // Timer used for high side (2,3,4,5,19)
#define SYNC_TIMER_LOW        4   // Timer used for low side (2,3,4,5,19)
#define SYNC_TIMER_HIGH1_PORT A   // Letter, ex: A
#define SYNC_TIMER_HIGH1_PIN  0   // Number, ex: 1
#define SYNC_TIMER_HIGH1_AF   11  // Number, ex: 1
#define SYNC_TIMER_HIGH2_PORT A   // Letter, ex: A
#define SYNC_TIMER_HIGH2_PIN  1   // Number, ex: 1
#define SYNC_TIMER_HIGH2_AF   11  // Number, ex: 1
#define SYNC_TIMER_HIGH3_PORT A   // Letter, ex: A
#define SYNC_TIMER_HIGH3_PIN  2   // Number, ex: 1
#define SYNC_TIMER_HIGH3_AF   11  // Number, ex: 1
#define SYNC_TIMER_LOW1_PORT  B   // Letter, ex: A
#define SYNC_TIMER_LOW1_PIN   6   // Number, ex: 1
#define SYNC_TIMER_LOW1_AF    2   // Number, ex: 1
#define SYNC_TIMER_LOW2_PORT  B   // Letter, ex: A
#define SYNC_TIMER_LOW2_PIN   7   // Number, ex: 1
#define SYNC_TIMER_LOW2_AF    2   // Number, ex: 1
#define SYNC_TIMER_LOW3_PORT  B   // Letter, ex: A
#define SYNC_TIMER_LOW3_PIN   8   // Number, ex: 1
#define SYNC_TIMER_LOW3_AF    2   // Number, ex: 1
#define SYNC_TIMER_DEADTIME   36  // Number in ticks.  Time = ticks/system speed = ticks/72000000


////////////////////////////////////////////////////////////////////////////////
// Time Base Setup
// For each TIMEBASE_INTx_FREQ, create a timerCallbackx() function where the x's match numbers
// The callbacks will be called at the desired TIMEBASE_INTx_FREQ 
// Define the frequencies in descending frequency order wrt to INT#.
// Do not skip INT#s.  First define INT1, then INT2, then INT3, then INT4.
#define TIMEBASE_TIMER        2
#define TIMEBASE_INT1_FREQ    1000 // Number 
// #define TIMEBASE_INT2_FREQ 1 // Number
// #define TIMEBASE_INT3_FREQ 1 // Number
// #define TIMEBASE_INT4_FREQ 1 // Number


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