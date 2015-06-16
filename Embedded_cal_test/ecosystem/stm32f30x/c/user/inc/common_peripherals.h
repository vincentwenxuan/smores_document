#ifndef COMMON_PERIPHERALS_H
#define COMMON_PERIPHERALS_H

#include "project_peripherals.h"
#include "ic_header_names.h"

// this is a little gross -- it wraps more than it needs to
#ifdef __cplusplus
extern "C" {
#endif

/// ===========================================================================
/// ======================  Don't touch past here  ============================
/// ===========================================================================

// useful string replacement macros
#define PASTER(x,y) x ## y
#define MAKENAME(x,y)  PASTER(x,y)

#define XSTR(x) STR(x)
#define STR(x) #x

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
#define mErrorON mRedON
#define mErrorOFF mRedOFF
#define mErrorTOGGLE mRedTOGGLE
#define mError(x) mRed(x)

#if defined(LED_TURQUOISE_PORT) && defined(LED_TURQUOISE_PIN)
  #define USING_RED_LED
  #define LED_TURQUOISE_PORT_FULL MAKENAME(GPIO,LED_TURQUOISE_PORT)
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
#ifdef ENCODER_COUNTS
  #define ENCODER_REV_PER_TICK (1.0f/ENCODER_COUNTS)
#endif // ENCODER_COUTNS


////////////////////////////////////////////////////////////////////////////////
/// MBUS Configuration (I2C)
#ifdef USING_MBUS1
  #define CPAL_I2C1_SCL_GPIO_PORT       MAKENAME(GPIO,MBUS1_SCL_PORT)
  #define CPAL_I2C1_SCL_GPIO_CLK        MAKENAME(RCC_AHBPeriph_GPIO,MBUS1_SCL_PORT)
  #define CPAL_I2C1_SCL_GPIO_PIN        MAKENAME(GPIO_Pin_,MBUS1_SCL_PIN)
  #define CPAL_I2C1_SCL_GPIO_PINSOURCE  MAKENAME(GPIO_PinSource,MBUS1_SCL_PIN)
  #define CPAL_I2C1_SCL_GPIO_AF         GPIO_AF_4

  #define CPAL_I2C1_SDA_GPIO_PORT       MAKENAME(GPIO,MBUS1_SDA_PORT)
  #define CPAL_I2C1_SDA_GPIO_CLK        MAKENAME(RCC_AHBPeriph_GPIO,MBUS1_SDA_PORT)
  #define CPAL_I2C1_SDA_GPIO_PIN        MAKENAME(GPIO_Pin_,MBUS1_SDA_PIN)
  #define CPAL_I2C1_SDA_GPIO_PINSOURCE  MAKENAME(GPIO_PinSource,MBUS1_SDA_PIN)
  #define CPAL_I2C1_SDA_GPIO_AF         GPIO_AF_4
#else
  #define CPAL_I2C1_SCL_GPIO_PORT       0
  #define CPAL_I2C1_SCL_GPIO_CLK        0
  #define CPAL_I2C1_SCL_GPIO_PIN        0
  #define CPAL_I2C1_SCL_GPIO_PINSOURCE  0
  #define CPAL_I2C1_SCL_GPIO_AF         0

  #define CPAL_I2C1_SDA_GPIO_PORT       0
  #define CPAL_I2C1_SDA_GPIO_CLK        0
  #define CPAL_I2C1_SDA_GPIO_PIN        0
  #define CPAL_I2C1_SDA_GPIO_PINSOURCE  0
  #define CPAL_I2C1_SDA_GPIO_AF         0
#endif // USING_MBUS1

#ifdef USING_MBUS2
  #define CPAL_I2C2_SCL_GPIO_PORT       MAKENAME(GPIO,MBUS2_SCL_PORT)
  #define CPAL_I2C2_SCL_GPIO_CLK        MAKENAME(RCC_AHBPeriph_GPIO,MBUS2_SCL_PORT)
  #define CPAL_I2C2_SCL_GPIO_PIN        MAKENAME(GPIO_Pin_,MBUS2_SCL_PIN)
  #define CPAL_I2C2_SCL_GPIO_PINSOURCE  MAKENAME(GPIO_PinSource,MBUS2_SCL_PIN)
  #define CPAL_I2C2_SCL_GPIO_AF         GPIO_AF_4

  #define CPAL_I2C2_SDA_GPIO_PORT       MAKENAME(GPIO,MBUS2_SDA_PORT)
  #define CPAL_I2C2_SDA_GPIO_CLK        MAKENAME(RCC_AHBPeriph_GPIO,MBUS2_SDA_PORT)
  #define CPAL_I2C2_SDA_GPIO_PIN        MAKENAME(GPIO_Pin_,MBUS2_SDA_PIN)
  #define CPAL_I2C2_SDA_GPIO_PINSOURCE  MAKENAME(GPIO_PinSource,MBUS2_SDA_PIN)
  #define CPAL_I2C2_SDA_GPIO_AF         GPIO_AF_4
#else
  #define CPAL_I2C2_SCL_GPIO_PORT       0
  #define CPAL_I2C2_SCL_GPIO_CLK        0
  #define CPAL_I2C2_SCL_GPIO_PIN        0
  #define CPAL_I2C2_SCL_GPIO_PINSOURCE  0
  #define CPAL_I2C2_SCL_GPIO_AF         0

  #define CPAL_I2C2_SDA_GPIO_PORT       0
  #define CPAL_I2C2_SDA_GPIO_CLK        0
  #define CPAL_I2C2_SDA_GPIO_PIN        0
  #define CPAL_I2C2_SDA_GPIO_PINSOURCE  0
  #define CPAL_I2C2_SDA_GPIO_AF         0
#endif // USING_MBUS2

////////////////////////////////////////////////////////////////////////////////
// Motor timer configuration
#if defined(MOTOR_TIMER)
  #if defined(MOTOR_TIMER_HIGH3_PORT)
    #define MOTOR_TIMER_CHANNELS 3
  #elif defined(MOTOR_TIMER_HIGH2_PORT)
    #define MOTOR_TIMER_CHANNELS 2
  #else
    #define MOTOR_TIMER_CHANNELS 1
  #endif //defined(MOTOR_TIMER_HIGH?_PORT)
  #include "motor_timer.h"
#endif //defined(MOTOR_TIMER)

////////////////////////////////////////////////////////////////////////////////
// Three Phase Inverter sync timer configuration
#if defined(SYNC_TIMER_LOW) && defined(SYNC_TIMER_HIGH)
  #include "sync_timer.h"
  #if SYNC_TIMER_LOW == 2
    #if SYNC_TIMER_HIGH == 19
      #define SYNC_TIMER_ITR TIM_TS_ITR0
    #endif
    #if SYNC_TIMER_HIGH == 3
      #define SYNC_TIMER_ITR TIM_TS_ITR2
    #endif
  #endif
  #if SYNC_TIMER_LOW == 3
    #if SYNC_TIMER_HIGH == 19
      #define SYNC_TIMER_ITR TIM_TS_ITR0
    #endif
    #if SYNC_TIMER_HIGH == 2
      #define SYNC_TIMER_ITR TIM_TS_ITR1
    #endif
    #if SYNC_TIMER_HIGH == 5
      #define SYNC_TIMER_ITR TIM_TS_ITR2
    #endif
  #endif
  #if SYNC_TIMER_LOW == 4
    #if SYNC_TIMER_HIGH == 19
      #define SYNC_TIMER_ITR TIM_TS_ITR0
    #endif
    #if SYNC_TIMER_HIGH == 2
      #define SYNC_TIMER_ITR TIM_TS_ITR1
    #endif
    #if SYNC_TIMER_HIGH == 3
      #define SYNC_TIMER_ITR TIM_TS_ITR2
    #endif
  #endif
  #if SYNC_TIMER_LOW == 5
    #if SYNC_TIMER_HIGH == 2
      #define SYNC_TIMER_ITR TIM_TS_ITR0
    #endif
    #if SYNC_TIMER_HIGH == 3
      #define SYNC_TIMER_ITR TIM_TS_ITR1
    #endif
    #if SYNC_TIMER_HIGH == 4
      #define SYNC_TIMER_ITR TIM_TS_ITR2
    #endif
  #endif
  #if SYNC_TIMER_LOW == 19
    #if SYNC_TIMER_HIGH == 2
      #define SYNC_TIMER_ITR TIM_TS_ITR0
    #endif
    #if SYNC_TIMER_HIGH == 3
      #define SYNC_TIMER_ITR TIM_TS_ITR1
    #endif
  #endif

  #ifndef SYNC_TIMER_ITR
    #pragma message ( "NO TIMER SYNCRONIZATION SOLUTION FOUND FOR THREE PHASE INVERTER" )
  #endif
#endif

// system clock configuration for timestamp and delay utilities
#ifdef SYSTEM_CLOCK_TIMER
  #define USING_SYSTEM_CLOCK
  #include "system_clock.h"
#endif

// encoder configuration
#if defined(ENCODER_QEP_TIMER) || defined(ENCODER_PWM_TIMER)
  #include "encoder.h"
#endif

// mBus configuration
#if defined(USING_MBUS1) || defined(USING_MBUS2)
  //#include "mBus.h" // including this breaks it because mBus.h depents on this
#endif

// Zigbee configuration
#ifdef ZIGBEE_SPI
  #include "zigbee.h"
#endif

// GPIO configuration
#ifdef USING_GPIO
  #include STM32FXXX_GPIO_H
#endif

// PWM configuration
#if ((defined(PWM_TIMER)) && (defined(PWM_FREQUENCY)))
  #if !(PWM_TIMER == 2 || PWM_TIMER == 3 || PWM_TIMER == 4 ||  PWM_TIMER == 15 || PWM_TIMER == 16 || PWM_TIMER == 17)
    #error "Invalid timer.  Use 2,3,4,15,16,17"
  #endif
  #if ((defined(PWM1_PORT)) && (defined(PWM1_PIN)) && (defined(PWM1_AF)))
    #define USING_PWM1
  #endif //((defined(PWM1_PORT)) && (defined(PWM1_PIN)) && (defined(PWM1_AF)))
  #if ((defined(PWM2_PORT)) && (defined(PWM2_PIN)) && (defined(PWM2_AF)))
    #define USING_PWM2
  #endif //((defined(PWM2_PORT)) && (defined(PWM2_PIN)) && (defined(PWM2_AF)))
  #if ((defined(PWM3_PORT)) && (defined(PWM3_PIN)) && (defined(PWM3_AF)))
    #define USING_PWM3
  #endif //((defined(PWM3_PORT)) && (defined(PWM3_PIN)) && (defined(PWM3_AF)))
  #if ((defined(PWM4_PORT)) && (defined(PWM4_PIN)) && (defined(PWM4_AF)))
    #define USING_PWM4
  #endif //((defined(PWM4_PORT)) && (defined(PWM4_PIN)) && (defined(PWM4_AF)))
  #if ( defined(USING_PWM1) || defined(USING_PWM2) || defined(USING_PWM3) || defined(USING_PWM4) )
    //#include "pwm.h"
    #if PWM_TIMER == 2
      #define PWM_SIZE uint32_t
      #define PWM_CLOCK_PSC (SystemCoreClock/PWM_FREQUENCY/4294967295 + 1)
    #else //PWM_TIMER == 2
      #define PWM_SIZE uint16_t
      #define PWM_CLOCK_PSC (SystemCoreClock/PWM_FREQUENCY/65535 + 1)
    #endif //PWM_TIMER == 2
    #if (PWM_TIMER == 16 || PWM_TIMER == 17) && (defined(USING_PWM2) || defined(USING_PWM3) || defined(USING_PWM4))
      #error "The selected PWM timer can only support 1 PWM"
    #endif
    #if ( PWM_TIMER == 15) && (defined(USING_PWM3) || defined(USING_PWM4))
      #error "The selected PWM timer can only support 2 PWMs"
    #endif
  #else //(((defined(USING_PWM1)) || ((defined(USING_PWM2)) || ((defined(USING_PWM3)) || ((defined(USING_PWM4)))
    #error "A PWM_TIMER was defined but no PWMx_PINs, PWMx_PORTs, and PWMx_AFs were properly defined"
  #endif //(((defined(USING_PWM1)) || ((defined(USING_PWM2)) || ((defined(USING_PWM3)) || ((defined(USING_PWM4)))
#endif //((defined(PWM_TIMER)) && (defined(PWM_FREQUENCY)))
  

// Timer configuration
#ifdef TIMEBASE_TIMER
  #if TIMEBASE_TIMER == 1
    #define TIMER_INTERRUPT TIM1_CC_IRQ
  #elif TIMEBASE_TIMER == 6 
    #define TIMER_INTERRUPT TIM6_DAC_IRQ
  #elif TIMEBASE_TIMER == 8
    #define TIMER_INTERRUPT TIM8_CC_IRQ
  #elif TIMEBASE_TIMER == 15
    #define TIMER_INTERRUPT TIM1_BRK_TIM15_IRQ
  #elif TIMEBASE_TIMER == 16
    #define TIMER_INTERRUPT TIM1_UP_TIM16_IRQ
  #elif TIMEBASE_TIMER == 17
    #define TIMER_INTERRUPT TIM1_TRG_COM_TIM17_IRQ
  #else
    #define TIMER_INTERRUPT MAKENAME(TIM,MAKENAME(TIMEBASE_TIMER,_IRQ))
  #endif
  #define TIMER_INTERRUPT_FCN MAKENAME(TIMER_INTERRUPT,Handler)
  #include "timebase.h"
  #if TIMEBASE_TIMER == 2
    #define TIMEBASE_SIZE uint32_t
    #if defined(TIMEBASE_INT4_FREQ)
      #define TIMEBASE_CLOCK_PSC (SystemCoreClock/TIMEBASE_INT4_FREQ/4294967295 + 1)
    #elif defined(TIMEBASE_INT3_FREQ)
      #define TIMEBASE_CLOCK_PSC (SystemCoreClock/TIMEBASE_INT3_FREQ/4294967295 + 1)
    #elif defined(TIMEBASE_INT2_FREQ)
      #define TIMEBASE_CLOCK_PSC (SystemCoreClock/TIMEBASE_INT2_FREQ/4294967295 + 1)
    #elif defined(TIMEBASE_INT1_FREQ)
      #define TIMEBASE_CLOCK_PSC (SystemCoreClock/TIMEBASE_INT1_FREQ/4294967295 + 1)
    #endif
  #else
    #define TIMEBASE_SIZE uint16_t
    #if defined(TIMEBASE_INT4_FREQ)
      #define TIMEBASE_CLOCK_PSC (SystemCoreClock/TIMEBASE_INT4_FREQ/65535 + 1)
    #elif defined(TIMEBASE_INT3_FREQ)
      #define TIMEBASE_CLOCK_PSC (SystemCoreClock/TIMEBASE_INT3_FREQ/65535 + 1)
    #elif defined(TIMEBASE_INT2_FREQ)
      #define TIMEBASE_CLOCK_PSC (SystemCoreClock/TIMEBASE_INT2_FREQ/65535 + 1)
    #elif defined(TIMEBASE_INT1_FREQ)
      #define TIMEBASE_CLOCK_PSC (SystemCoreClock/TIMEBASE_INT1_FREQ/65535 + 1)
    #endif
  #endif
  #if (TIMEBASE_TIMER == 6 || TIMEBASE_TIMER == 7 || TIMEBASE_TIMER == 16 || TIMEBASE_TIMER == 17) && (defined(TIMEBASE_INT2_FREQ) || defined(TIMEBASE_INT3_FREQ) || defined(TIMEBASE_INT4_FREQ))
    #error "The selected TIMEBASE timer can only support 1 timebase"
  #endif
  #if TIMEBASE_TIMER == 15 && (defined(TIMEBASE_INT3_FREQ) || defined(TIMEBASE_INT4_FREQ))
    #error "The selected TIMEBASE timer can only support 2 timebases"
  #endif
#endif


void InitPeripherals();

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // COMMON_PERIPHERALS_H
