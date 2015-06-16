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
#define M4 // use for any M4
#define M4_WHITE // use for the white M4


/// Onboard LED Setup
/// do not define unused LEDS
/// eg. if LED_WHITE_PORT not defined, then mWhiteON compiles to whitespace
/*
#define LED_AMBER_PORT ? // Letter, ex: A, B, ...
#define LED_AMBER_PIN ? // Number, ex: 0, 1, 2, ...
#define LED_AMBER_SET_ON // define iff on-when-set
*/

#define LED_BLUE_PORT B // Letter, ex: A, B, ...
#define LED_BLUE_PIN 15 // Number, ex: 0, 1, 2, ...
#define LED_BLUE_SET_ON // define iff on-when-set

/*
#define LED_CYAN_PORT ? // Letter, ex: A, B, ...
#define LED_CYAN_PIN ? // Number, ex: 0, 1, 2, ...
#define LED_CYAN_SET_ON // define iff on-when-set
*/

#define LED_GREEN_PORT C // Letter, ex: A, B, ...
#define LED_GREEN_PIN 13 // Number, ex: 0, 1, 2, ...
//#define LED_GREEN_SET_ON // define iff on-when-set

/*
#define LED_ORANGE_PORT ? // Letter, ex: A, B, ...
#define LED_ORANGE_PIN ? // Number, ex: 0, 1, 2, ...
#define LED_ORANGE_SET_ON // define iff on-when-set
*/
/*
#define LED_PINK_PORT ? // Letter, ex: A, B, ...
#define LED_PINK_PIN ? // Number, ex: 0, 1, 2, ...
#define LED_PINK_SET_ON // define iff on-when-set
*/
/*
#define LED_PURPLE_PORT ? // Letter, ex: A, B, ...
#define LED_PURPLE_PIN ? // Number, ex: 0, 1, 2, ...
#define LED_PURPLE_SET_ON // define iff on-when-set
*/

#define LED_RED_PORT C // Letter, ex: A, B, ...
#define LED_RED_PIN 14 // Number, ex: 0, 1, 2, ...
//#define LED_RED_SET_ON // define iff on-when-set

/*
#define LED_TURQUOISE_PORT ? // Letter, ex: A, B, ...
#define LED_TURQUOISE_PIN ? // Number, ex: 0, 1, 2, ...
#define LED_TURQUOISE_SET_ON // define iff on-when-set
*/

#define LED_WHITE_PORT D // Letter, ex: A, B, ...
#define LED_WHITE_PIN 8 // Number, ex: 0, 1, 2, ...
//#define LED_WHITE_SET_ON // define iff on-when-set

#define LED_YELLOW_PORT C // Letter, ex: A, B, ...
#define LED_YELLOW_PIN 15 // Number, ex: 0, 1, 2, ...
//#define LED_YELLOW_SET_ON // define iff on-when-set


/// IMU I2C Setup
/// Used with: Sensor_IMU_MPU-6050_I2C
/*
#define IMU_I2C ? //Number, 1 or 2
#define IMU_SCL_PORT ? //Letter, ex: A
#define IMU_SCL_PIN ? //Number, ex: 1
#define IMU_SDA_PORT ? //Letter, ex: A
#define IMU_SDA_PIN ? //Number, ex: 1
*/


/// IMU SPI Setup
/// Used with: Sensor_IMU_MPU-6050_SPI

/// ms Counter Setup

/// Pressure Sensor I2C Setup
/// Used with: Sensor_Pressure_MS5611-01BA03_I2C

/// Pressure Sensor SPI Setup
/// Used with: Sensor_Pressure_MS5611-01BA03_SPI

/// Quadrature Encoder Setup
/// Used with: Encoder_Rotary_Magnetic_AS5XXX
/*
#define ENCODER_QEP_TIMER ? //Number, 2, 3, 4, 5, 15 or 19
#define ENCODER_QEPA_PORT ? //Letter, ex: A
#define ENCODER_QEPA_PIN ? //Number, ex: 1
#define ENCODER_QEPA_AF ? //Number, ex: 1
#define ENCODER_QEPB_PORT ? //Letter, ex: A
#define ENCODER_QEPB_PIN ? //Number, ex: 1
#define ENCODER_QEPB_AF ? //Number, ex: 1
#define ENCODER_COUNTS ? //Number, ex: 4096
#define ENCODER_INDEX_PORT ? //Letter, ex: A
#define ENCODER_INDEX_PIN ? //Number, ex: 1
#define ENCODER_PWM_TIMER ? //Number, 2, 3, 4, 5, 12, 15 or 19
#define ENCODER_PWM_PORT ? //Letter, ex: A
#define ENCODER_PWM_PIN ? //Number, ex: 1
#define ENCODER_PWM_AF ? //Number, ex: 1
#define ENCODER_PWM_CHANNEL ? //Number, 1 or 2
*/

/// Three Phase Inverter Setup
/// Used with 3x: Inverter_Half_H-Bridge_40A_50khz, Inverter_Half_H-Bridge_20A_50khz
/*
#define SYNC_TIMER_HIGH ? //Timer used for high side (2,3,4,5,19)
#define SYNC_TIMER_LOW ? //Timer used for low side (2,3,4,5,19)
#define SYNC_TIMER_HIGH1_PORT ? //Letter, ex: A
#define SYNC_TIMER_HIGH1_PIN ? //Number, ex: 1
#define SYNC_TIMER_HIGH1_AF ? //Number, ex: 1
#define SYNC_TIMER_HIGH2_PORT ? //Letter, ex: A
#define SYNC_TIMER_HIGH2_PIN ? //Number, ex: 1
#define SYNC_TIMER_HIGH2_AF ? //Number, ex: 1
#define SYNC_TIMER_HIGH3_PORT ? //Letter, ex: A
#define SYNC_TIMER_HIGH3_PIN ? //Number, ex: 1
#define SYNC_TIMER_HIGH3_AF ? //Number, ex: 1
#define SYNC_TIMER_LOW1_PORT ? //Letter, ex: A
#define SYNC_TIMER_LOW1_PIN ? //Number, ex: 1
#define SYNC_TIMER_LOW1_AF ? //Number, ex: 1
#define SYNC_TIMER_LOW2_PORT ? //Letter, ex: A
#define SYNC_TIMER_LOW2_PIN ? //Number, ex: 1
#define SYNC_TIMER_LOW2_AF ? //Number, ex: 1
#define SYNC_TIMER_LOW3_PORT ? //Letter, ex: A
#define SYNC_TIMER_LOW3_PIN ? //Number, ex: 1
#define SYNC_TIMER_LOW3_AF ? //Number, ex: 1
*/

/// USB Setup
#define USING_USB // define if using USB
/// Used with Communication_Plug_USB_Micro_B_FCI_10103594-0001LF, Communication_Plug_USB_Mini_B_Wurth_651305142821
/// Enable these only if a USB software disconnect pin is used

#define USB_DISCONNECT_PORT F //Letter, ex: A
#define USB_DISCONNECT_PIN 1 //Number, ex: 1


/// Zigbee Setup (for zigbee, uncomment and fill in ? below)
/// Used with: Radio_AT86RF
/*
#define ZIGBEE_SPI ? //Number, ex: 1
#define ZIGBEE_SCK_PORT ? //Letter, ex: A
#define ZIGBEE_SCK_PIN ? //Number, ex: 1
#define ZIGBEE_SCK_AF ? //Number, ex: 1
#define ZIGBEE_MISO_PORT ? //Letter, ex: A
#define ZIGBEE_MISO_PIN ? //Number, ex: 1
#define ZIGBEE_MISO_AF ? //Number, ex: 1
#define ZIGBEE_MOSI_PORT ? //Letter, ex: A
#define ZIGBEE_MOSI_PIN ? //Number, ex: 1
#define ZIGBEE_MOSI_AF ? //Number, ex: 1
#define ZIGBEE_SS_PORT ? //Letter, ex: A
#define ZIGBEE_SS_PIN ? //Number, ex: 1
*/





/// ===========================================================================
/// ======================  Don't touch past here  ============================
/// ===========================================================================

// useful string replacement macros
#define PASTER(x,y) x ## y
#define MAKENAME(x,y)  PASTER(x,y)

// Onboard LEDs
#if defined(LED_AMBER_PORT) && defined(LED_AMBER_PIN)
  #define USING_AMBER_LED
  #define LED_AMBER_PORT_FULL MAKENAME(GPIO,LED_AMBER_PORT)
  #ifdef LED_AMBER_SET_ON
    #define mAmberON LED_AMBER_PORT_FULL->BSRR = (1<<LED_AMBER_PIN)
    #define mAmberOFF LED_AMBER_PORT_FULL->BRR = (1<<LED_AMBER_PIN)
  #else
    #define mAmberON LED_AMBER_PORT_FULL->BRR = (1<<LED_AMBER_PIN)
    #define mAmberOFF LED_AMBER_PORT_FULL->BSRR = (1<<LED_AMBER_PIN)
  #endif
  #define mAmberTOGGLE LED_AMBER_PORT_FULL->ODR ^= (1<<LED_AMBER_PIN)
  #define mAmber(val)		if(val==ON){mAmberON;}\
						              else if(val==OFF){mAmberOFF;}\
						                else if(val==TOGGLE){mAmberTOGGLE;}
#else
  #define mAmberON
  #define mAmberOFF
  #define mAmberTOGGLE
  #define mAmber(val)
#endif

#if defined(LED_BLUE_PORT) && defined(LED_BLUE_PIN)
  #define USING_BLUE_LED
  #define LED_BLUE_PORT_FULL MAKENAME(GPIO,LED_BLUE_PORT)
  #ifdef LED_BLUE_SET_ON
    #define mBlueON LED_BLUE_PORT_FULL->BSRR = (1<<LED_BLUE_PIN)
    #define mBlueOFF LED_BLUE_PORT_FULL->BRR = (1<<LED_BLUE_PIN)
  #else
    #define mBlueON LED_BLUE_PORT_FULL->BRR = (1<<LED_BLUE_PIN)
    #define mBlueOFF LED_BLUE_PORT_FULL->BSRR = (1<<LED_BLUE_PIN)
  #endif
  #define mBlueTOGGLE LED_BLUE_PORT_FULL->ODR ^= (1<<LED_BLUE_PIN)
  #define mBlue(val)		if(val==ON){mBlueON;}\
						              else if(val==OFF){mBlueOFF;}\
						                else if(val==TOGGLE){mBlueTOGGLE;}
#else
  #define mBlueON
  #define mBlueOFF
  #define mBlueTOGGLE
  #define mBlue(val)
#endif

#if defined(LED_CYAN_PORT) && defined(LED_CYAN_PIN)
  #define USING_CYAN_LED
  #define LED_CYAN_PORT_FULL MAKENAME(GPIO,LED_CYAN_PORT)
  #ifdef LED_CYAN_SET_ON
    #define mCyanON LED_CYAN_PORT_FULL->BSRR = (1<<LED_CYAN_PIN)
    #define mCyanOFF LED_CYAN_PORT_FULL->BRR = (1<<LED_CYAN_PIN)
  #else
    #define mCyanON LED_CYAN_PORT_FULL->BRR = (1<<LED_CYAN_PIN)
    #define mCyanOFF LED_CYAN_PORT_FULL->BSRR = (1<<LED_CYAN_PIN)
  #endif
  #define mCyanTOGGLE LED_CYAN_PORT_FULL->ODR ^= (1<<LED_CYAN_PIN)
  #define mCyan(val)		if(val==ON){mCyanON;}\
						              else if(val==OFF){mCyanOFF;}\
						                else if(val==TOGGLE){mCyanTOGGLE;}
#else
  #define mCyanON
  #define mCyanOFF
  #define mCyanTOGGLE
  #define mCyan(val)
#endif

#if defined(LED_GREEN_PORT) && defined(LED_GREEN_PIN)
  #define USING_GREEN_LED
  #define LED_GREEN_PORT_FULL MAKENAME(GPIO,LED_GREEN_PORT)
  #ifdef LED_GREEN_SET_ON
    #define mGreenON LED_GREEN_PORT_FULL->BSRR = (1<<LED_GREEN_PIN)
    #define mGreenOFF LED_GREEN_PORT_FULL->BRR = (1<<LED_GREEN_PIN)
  #else
    #define mGreenON LED_GREEN_PORT_FULL->BRR = (1<<LED_GREEN_PIN)
    #define mGreenOFF LED_GREEN_PORT_FULL->BSRR = (1<<LED_GREEN_PIN)
  #endif
  #define mGreenTOGGLE LED_GREEN_PORT_FULL->ODR ^= (1<<LED_GREEN_PIN)
  #define mGreen(val)		if(val==OFF){mGreenOFF;}\
						              else if(val==ON){mGreenON;}\
						                else if(val==TOGGLE){mGreenTOGGLE;}
#else
  #define mGreenON
  #define mGreenOFF
  #define mGreenTOGGLE
  #define mGreen(val)
#endif

#if defined(LED_ORANGE_PORT) && defined(LED_ORANGE_PIN)
  #define USING_ORANGE_LED
  #define LED_ORANGE_PORT_FULL MAKENAME(GPIO,LED_ORANGE_PORT)
  #ifdef LED_ORANGE_SET_ON
    #define mOrangeON LED_ORANGE_PORT_FULL->BSRR = (1<<LED_ORANGE_PIN)
    #define mOrangeOFF LED_ORANGE_PORT_FULL->BRR = (1<<LED_ORANGE_PIN)
  #else
    #define mOrangeON LED_ORANGE_PORT_FULL->BRR = (1<<LED_ORANGE_PIN)
    #define mOrangeOFF LED_ORANGE_PORT_FULL->BSRR = (1<<LED_ORANGE_PIN)
  #endif
  #define mOrangeTOGGLE LED_ORANGE_PORT_FULL->ODR ^= (1<<LED_ORANGE_PIN)
  #define mOrange(val)		if(val==OFF){mOrangeOFF;}\
						              else if(val==ON){mOrangeON;}\
						                else if(val==TOGGLE){mOrangeTOGGLE;}
#else
  #define mOrangeON
  #define mOrangeOFF
  #define mOrangeTOGGLE
  #define mOrange(val)
#endif

#if defined(LED_PINK_PORT) && defined(LED_PINK_PIN)
  #define USING_PINK_LED
  #define LED_PINK_PORT_FULL MAKENAME(GPIO,LED_PINK_PORT)
  #ifdef LED_PINK_SET_ON
    #define mPinkON LED_PINK_PORT_FULL->BSRR = (1<<LED_PINK_PIN)
    #define mPinkOFF LED_PINK_PORT_FULL->BRR = (1<<LED_PINK_PIN)
  #else
    #define mPinkON LED_PINK_PORT_FULL->BRR = (1<<LED_PINK_PIN)
    #define mPinkOFF LED_PINK_PORT_FULL->BSRR = (1<<LED_PINK_PIN)
  #endif
  #define mPinkTOGGLE LED_PINK_PORT_FULL->ODR ^= (1<<LED_PINK_PIN)
  #define mPink(val)		if(val==OFF){mPinkOFF;}\
						              else if(val==ON){mPinkON;}\
						                else if(val==TOGGLE){mPinkTOGGLE;}
#else
  #define mPinkON
  #define mPinkOFF
  #define mPinkTOGGLE
  #define mPink(val)
#endif

#if defined(LED_PURPLE_PORT) && defined(LED_PURPLE_PIN)
  #define USING_PINK_LED
  #define LED_PURPLE_PORT_FULL MAKENAME(GPIO,LED_PURPLE_PORT)
  #ifdef LED_PURPLE_SET_ON
    #define mPurpleON LED_PURPLE_PORT_FULL->BSRR = (1<<LED_PURPLE_PIN)
    #define mPurpleOFF LED_PURPLE_PORT_FULL->BRR = (1<<LED_PURPLE_PIN)
  #else
    #define mPurpleON LED_PURPLE_PORT_FULL->BRR = (1<<LED_PURPLE_PIN)
    #define mPurpleOFF LED_PURPLE_PORT_FULL->BSRR = (1<<LED_PURPLE_PIN)
  #endif
  #define mPurpleTOGGLE LED_PURPLE_PORT_FULL->ODR ^= (1<<LED_PURPLE_PIN)
  #define mPurple(val)		if(val==OFF){mPurpleOFF;}\
						              else if(val==ON){mPurpleON;}\
						                else if(val==TOGGLE){mPurpleTOGGLE;}
#else
  #define mPurpleON
  #define mPurpleOFF
  #define mPurpleTOGGLE
  #define mPurple(val)
#endif

#if defined(LED_RED_PORT) && defined(LED_RED_PIN)
  #define USING_RED_LED
  #define LED_RED_PORT_FULL MAKENAME(GPIO,LED_RED_PORT)
  #ifdef LED_RED_SET_ON
    #define mRedON LED_RED_PORT_FULL->BSRR = (1<<LED_RED_PIN)
    #define mRedOFF LED_RED_PORT_FULL->BRR = (1<<LED_RED_PIN)
  #else
    #define mRedON LED_RED_PORT_FULL->BRR = (1<<LED_RED_PIN)
    #define mRedOFF LED_RED_PORT_FULL->BSRR = (1<<LED_RED_PIN)
  #endif
  #define mRedTOGGLE LED_RED_PORT_FULL->ODR ^= (1<<LED_RED_PIN)
  #define mRed(val)		if(val==OFF){mRedOFF;}\
						            else if(val==ON){mRedON;}\
						              else if(val==TOGGLE){mRedTOGGLE;}
#else
  #define mRedON
  #define mRedOFF
  #define mRedTOGGLE
  #define mRed(val)
#endif

#if defined(LED_TURQUOISE_PORT) && defined(LED_TURQUOISE_PIN)
  #define USING_RED_LED
  #define LED_RED_TURQUOISE_FULL MAKENAME(GPIO,LED_TURQUOISE_PORT)
  #ifdef LED_TURQUOISE_SET_ON
    #define mTurquoiseON LED_TURQUOISE_PORT_FULL->BSRR = (1<<LED_TURQUOISE_PIN)
    #define mTurquoiseOFF LED_TURQUOISE_PORT_FULL->BRR = (1<<LED_TURQUOISE_PIN)
  #else
    #define mTurquoiseON LED_TURQUOISE_PORT_FULL->BRR = (1<<LED_TURQUOISE_PIN)
    #define mTurquoiseOFF LED_TURQUOISE_PORT_FULL->BSRR = (1<<LED_TURQUOISE_PIN)
  #endif
  #define mTurquoiseTOGGLE LED_TURQUOISE_PORT_FULL->ODR ^= (1<<LED_TURQUOISE_PIN)
  #define mTurquoise(val)		if(val==OFF){mTurquoiseOFF;}\
						            else if(val==ON){mTurquoiseON;}\
						              else if(val==TOGGLE){mTurquoiseTOGGLE;}
#else
  #define mTurquoiseON
  #define mTurquoiseOFF
  #define mTurquoiseTOGGLE
  #define mTurquoise(val)
#endif

#if defined(LED_WHITE_PORT) && defined(LED_WHITE_PIN)
  #define USING_WHITE_LED
  #define LED_WHITE_PORT_FULL MAKENAME(GPIO,LED_WHITE_PORT)
  #ifdef LED_WHITE_SET_ON
    #define mWhiteON LED_WHITE_PORT_FULL->BSRR = (1<<LED_WHITE_PIN)
    #define mWhiteOFF LED_WHITE_PORT_FULL->BRR = (1<<LED_WHITE_PIN)
  #else
    #define mWhiteON LED_WHITE_PORT_FULL->BRR = (1<<LED_WHITE_PIN)
    #define mWhiteOFF LED_WHITE_PORT_FULL->BSRR = (1<<LED_WHITE_PIN)
  #endif
  #define mWhiteTOGGLE LED_WHITE_PORT_FULL->ODR ^= (1<<LED_WHITE_PIN)
  #define mWhite(val)		if(val==OFF){mWhiteOFF;}\
						            else if(val==ON){mWhiteON;}\
						              else if(val==TOGGLE){mWhiteTOGGLE;}
#else
  #define mWhiteON
  #define mWhiteOFF
  #define mWhiteTOGGLE
  #define mWhite(val)
#endif

#if defined(LED_YELLOW_PORT) && defined(LED_YELLOW_PIN)
  #define USING_YELLOW_LED
  #define LED_YELLOW_PORT_FULL MAKENAME(GPIO,LED_YELLOW_PORT)
  #ifdef LED_YELLOW_SET_ON
    #define mYellowON LED_YELLOW_PORT_FULL->BSRR = (1<<LED_YELLOW_PIN)
    #define mYellowOFF LED_YELLOW_PORT_FULL->BRR = (1<<LED_YELLOW_PIN)
  #else
    #define mYellowON LED_YELLOW_PORT_FULL->BRR = (1<<LED_YELLOW_PIN)
    #define mYellowOFF LED_YELLOW_PORT_FULL->BSRR = (1<<LED_YELLOW_PIN)
  #endif
  #define mYellowTOGGLE LED_YELLOW_PORT_FULL->ODR ^= (1<<LED_YELLOW_PIN)
  #define mYellow(val) 	if(val==OFF){mYellowOFF;}\
						              else if(val==ON){mYellowON;}\
						                else if(val==TOGGLE){mYellowTOGGLE;}
#else
  #define mYellowON
  #define mYellowOFF
  #define mYellowTOGGLE
  #define mYellow(val)
#endif

// LED Initialization
#if defined(USING_AMBER_LED) || defined(USING_BLUE_LED) || defined(USING_CYAN_LED) \
    || defined(USING_GREEN_LED) || defined(USING_ORANGE_LED) || defined(USING_PINK_LED) \
    || defined(USING_PURPLE_LED) || defined(USING_RED_LED) || defined(USING_TURQUOISE_LED) \
    || defined(USING_WHITE_LED) || defined(USING_YELLOW_LED)
  #define USING_LEDS
  #define USING_GPIO
#endif

// Encoder Configuration
#ifdef ENCODER_INDEX_PIN
  #if ENCODER_INDEX_PIN == 0
    #ifdef USING_EXTI0_IRQn
      #pragma message ( "Multiple EXTI Definitions in peripherals.h" )
    #endif
    #define USING_EXTI0_IRQn
    #define ENCODER_INTERRUPT EXTI0_IRQn
    #define ENCODER_INTERRUPT_FCN EXTI0_IRQHandler
  #elif ENCODER_INDEX_PIN == 1
    #ifdef USING_EXTI1_IRQn
      #pragma message ( "Multiple EXTI Definitions in peripherals.h" )
    #endif
    #define USING_EXTI1_IRQn
    #define ENCODER_INTERRUPT EXTI1_IRQn
    #define ENCODER_INTERRUPT_FCN EXTI1_IRQHandler
  #elif ENCODER_INDEX_PIN == 2
    #ifdef USING_EXTI2_TS_IRQn
      #pragma message ( "Multiple EXTI Definitions in peripherals.h" )
    #endif
    #define USING_EXTI2_TS_IRQn
    #define ENCODER_INTERRUPT EXTI2_TS_IRQn
    #define ENCODER_INTERRUPT_FCN EXTI2_TS_IRQHandler
  #elif ENCODER_INDEX_PIN == 3
    #ifdef USING_EXTI3_IRQn
      #pragma message ( "Multiple EXTI Definitions in peripherals.h" )
    #endif
    #define USING_EXTI3_IRQn
    #define ENCODER_INTERRUPT EXTI3_IRQn
    #define ENCODER_INTERRUPT_FCN EXTI3_IRQHandler
  #elif ENCODER_INDEX_PIN == 4
    #ifdef USING_EXTI4_IRQn
      #pragma message ( "Multiple EXTI Definitions in peripherals.h" )
    #endif
    #define USING_EXTI4_IRQn
    #define ENCODER_INTERRUPT EXTI4_IRQn
    #define ENCODER_INTERRUPT_FCN EXTI4_IRQHandler
  #elif ENCODER_INDEX_PIN >= 5 && ENCODER_INDEX_PIN <= 9
    #ifdef USING_EXTI9_5_IRQn
      #pragma message ( "Multiple EXTI Definitions in peripherals.h" )
    #endif
    #define USING_EXTI9_5_IRQn
    #define ENCODER_INTERRUPT EXTI9_5_IRQn
    #define ENCODER_INTERRUPT_FCN EXTI9_5_IRQHandler
  #elif ENCODER_INDEX_PIN >= 10 && ENCODER_INDEX_PIN <= 15
    #ifdef USING_EXTI15_10_IRQn
      #pragma message ( "Multiple EXTI Definitions in peripherals.h" )
    #endif
    #define USING_EXTI15_10_IRQn
    #define ENCODER_INTERRUPT EXTI15_10_IRQn
    #define ENCODER_INTERRUPT_FCN EXTI15_10_IRQHandler
  #endif
#endif
  

// IMU I2C configuration
#ifdef IMU_I2C
  #if IMU_I2C == 1
    #define CPAL_I2C1_SCL_GPIO_PORT MAKENAME(GPIO,IMU_SCL_PORT)
    #define CPAL_I2C1_SCL_GPIO_CLK MAKENAME(RCC_AHBPeriph_GPIO,IMU_SCL_PORT)
    #define CPAL_I2C1_SCL_GPIO_PIN MAKENAME(GPIO_Pin_,IMU_SCL_PIN)
    #define CPAL_I2C1_SCL_GPIO_PINSOURCE MAKENAME(GPIO_PinSource,IMU_SCL_PIN)

    #define CPAL_I2C1_SDA_GPIO_PORT MAKENAME(GPIO,IMU_SDA_PORT)
    #define CPAL_I2C1_SDA_GPIO_CLK MAKENAME(RCC_AHBPeriph_GPIO,IMU_SDA_PORT)
    #define CPAL_I2C1_SDA_GPIO_PIN MAKENAME(GPIO_Pin_,IMU_SDA_PIN)
    #define CPAL_I2C1_SDA_GPIO_PINSOURCE MAKENAME(GPIO_PinSource,IMU_SDA_PIN)
    
    #define USING_MBUS1
  #elif IMU_I2C == 2
    #define CPAL_I2C2_SCL_GPIO_PORT MAKENAME(GPIO,IMU_SCL_PORT)
    #define CPAL_I2C2_SCL_GPIO_CLK MAKENAME(RCC_AHBPeriph_GPIO,IMU_SCL_PORT)
    #define CPAL_I2C2_SCL_GPIO_PIN MAKENAME(GPIO_Pin_,IMU_SCL_PIN)
    #define CPAL_I2C2_SCL_GPIO_PINSOURCE MAKENAME(GPIO_PinSource,IMU_SCL_PIN)

    #define CPAL_I2C2_SDA_GPIO_PORT MAKENAME(GPIO,IMU_SDA_PORT)
    #define CPAL_I2C2_SDA_GPIO_CLK MAKENAME(RCC_AHBPeriph_GPIO,IMU_SDA_PORT)
    #define CPAL_I2C2_SDA_GPIO_PIN MAKENAME(GPIO_Pin_,IMU_SDA_PIN)
    #define CPAL_I2C2_SDA_GPIO_PINSOURCE MAKENAME(GPIO_PinSource,IMU_SDA_PIN)
    
    #define USING_MBUS2
  #endif
#endif //#ifdef IMU_I2c


// Three Phase Inverter configuration
#if defined(SYNC_TIMER_LOW) && defined(SYNC_TIMER_HIGH)
  #include "sync_timer.h"
  #if SYNC_TIMER_LOW == 2
    #define RCC_APB_LOW RCC_APB1Periph_TIM
    #if SYNC_TIMER_HIGH == 19
      #define RCC_APB_HIGH RCC_APB2Periph_TIM
      #define SYNC_TIMER_ITR TIM_TS_ITR0
    #endif
    #if SYNC_TIMER_HIGH == 3
      #define RCC_APB_HIGH RCC_APB1Periph_TIM
      #define SYNC_TIMER_ITR TIM_TS_ITR2
    #endif
  #endif
  #if SYNC_TIMER_LOW == 3
    #define RCC_APB_LOW RCC_APB1Periph_TIM
    #if SYNC_TIMER_HIGH == 19
      #define RCC_APB_HIGH RCC_APB2Periph_TIM
      #define SYNC_TIMER_ITR TIM_TS_ITR0
    #endif
    #if SYNC_TIMER_HIGH == 2
      #define RCC_APB_HIGH RCC_APB1Periph_TIM
      #define SYNC_TIMER_ITR TIM_TS_ITR1
    #endif
    #if SYNC_TIMER_HIGH == 5
      #define RCC_APB_HIGH RCC_APB1Periph_TIM
      #define SYNC_TIMER_ITR TIM_TS_ITR2
    #endif
  #endif
  #if SYNC_TIMER_LOW == 4
    #define RCC_APB_LOW RCC_APB1Periph_TIM
    #if SYNC_TIMER_HIGH == 19
      #define RCC_APB_HIGH RCC_APB2Periph_TIM
      #define SYNC_TIMER_ITR TIM_TS_ITR0
    #endif
    #if SYNC_TIMER_HIGH == 2
      #define RCC_APB_HIGH RCC_APB1Periph_TIM
      #define SYNC_TIMER_ITR TIM_TS_ITR1
    #endif
    #if SYNC_TIMER_HIGH == 3
      #define RCC_APB_HIGH RCC_APB1Periph_TIM
      #define SYNC_TIMER_ITR TIM_TS_ITR2
    #endif
  #endif
  #if SYNC_TIMER_LOW == 5
    #define RCC_APB_LOW RCC_APB1Periph_TIM
    #if SYNC_TIMER_HIGH == 2
      #define RCC_APB_HIGH RCC_APB1Periph_TIM
      #define SYNC_TIMER_ITR TIM_TS_ITR0
    #endif
    #if SYNC_TIMER_HIGH == 3
      #define RCC_APB_HIGH RCC_APB1Periph_TIM
      #define SYNC_TIMER_ITR TIM_TS_ITR1
    #endif
    #if SYNC_TIMER_HIGH == 4
      #define RCC_APB_HIGH RCC_APB1Periph_TIM
      #define SYNC_TIMER_ITR TIM_TS_ITR2
    #endif
  #endif
  #if SYNC_TIMER_LOW == 19
    #define RCC_APB_LOW RCC_APB2Periph_TIM
    #if SYNC_TIMER_HIGH == 2
      #define RCC_APB_HIGH RCC_APB1Periph_TIM
      #define SYNC_TIMER_ITR TIM_TS_ITR0
    #endif
    #if SYNC_TIMER_HIGH == 3
      #define RCC_APB_HIGH RCC_APB1Periph_TIM
      #define SYNC_TIMER_ITR TIM_TS_ITR1
    #endif
  #endif
  
  #ifndef SYNC_TIMER_ITR
    #pragma message ( "NO TIMER SYNCRONIZATION SOLUTION FOUND FOR THREE PHASE INVERTER" )
  #endif
#endif

// encoder configuration
#if defined(ENCODER_QEP_TIMER) || defined(ENCODER_PWM_TIMER)
  #include "encoder.h"
#endif

// mBus configuration
#if defined(USING_MBUS1) || defined(USING_MBUS2)
  #include "mBus.h"
#endif

// mUSB configuration
#ifdef USING_USB
  #include "mUSB.h"
#endif

// Zigbee configuration
#ifdef ZIGBEE_SPI
  #include "ZigBee.h"
#endif

// GPIO configuration
#ifdef USING_GPIO
  #include "stm32f37x_gpio.h"
#endif

// define any remaining macros that MUST be defined
#ifndef CPAL_I2C1_SCL_GPIO_PORT
  #define CPAL_I2C1_SCL_GPIO_PORT 0
  #define CPAL_I2C1_SCL_GPIO_CLK  0
  #define CPAL_I2C1_SCL_GPIO_PIN 0
  #define CPAL_I2C1_SCL_GPIO_PINSOURCE 0
  #define CPAL_I2C1_SDA_GPIO_PORT 0
  #define CPAL_I2C1_SDA_GPIO_CLK 0
  #define CPAL_I2C1_SDA_GPIO_PIN 0
  #define CPAL_I2C1_SDA_GPIO_PINSOURCE 0
#endif
#ifndef CPAL_I2C2_SCL_GPIO_PORT
  #define CPAL_I2C2_SCL_GPIO_PORT 0
  #define CPAL_I2C2_SCL_GPIO_CLK 0
  #define CPAL_I2C2_SCL_GPIO_PIN 0
  #define CPAL_I2C2_SCL_GPIO_PINSOURCE 0
  #define CPAL_I2C2_SDA_GPIO_PORT 0
  #define CPAL_I2C2_SDA_GPIO_CLK 0
  #define CPAL_I2C2_SDA_GPIO_PIN 0
  #define CPAL_I2C2_SDA_GPIO_PINSOURCE 0
#endif

int8_t InitPeripherals();

#endif // PERIPHERALS_STM32F37X_H