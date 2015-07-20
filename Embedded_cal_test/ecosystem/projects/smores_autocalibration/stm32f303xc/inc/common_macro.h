#ifndef COMMON_MACRO_H_
#define COMMON_MACRO_H_

// face i2c address macros
#define BOTTOM_FACE_I2C_ADDRESS 0x0B
#define LEFT_FACE_I2C_ADDRESS   0x0A
#define RIGHT_FACE_I2C_ADDRESS  0x0C
#define TOP_FACE_I2C_ADDRESS    0x0D

// common cmd
#define MAGNETS_ON            0xA2  // command to turn on the blue led
#define MAGNETS_OFF           0xA3  // command to turn on green led
#define FACE_PING             0xA5  // Ping request to tell if face is alive
#define SEND_STATES           0xA4
#define GET_STATES            0xA4
#define SEND_ADC_DATA         0xA6

#define CALI_ON               0xA7
#define CALI_SWITCH           0xA8
#define CALI_OFF			  0xA9
#define CALI_ON_TILT_M90	  0xAA
#define CALI_ON_TILT_0        0xAB
#define CALI_ON_TILT_90       0xAC

#endif // COMMON_MACRO_H_
