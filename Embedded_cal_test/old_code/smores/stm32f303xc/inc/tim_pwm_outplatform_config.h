/*
Author Chaitanya Bhargava (cbhar@seas.upenn.edu)
Settings for TIMER PWM OUT usage on the STM32F303
mGeneral includes all the ST library files and the wait functions
For simple control of motors
*/
/* FOR NOW CONFIGURED TO DRIVE 4 MOTORS ON THE SMORES*/
#ifdef	__cplusplus
extern "C" {
#endif
/*-------------TIMA, PA8-----------------*/
#define TIMA			TIM1
#define TIMA_RCC		RCC_APB2Periph_TIM1
#define TIMA_PORT		GPIOA
#define TIMA_PIN		GPIO_Pin_8
#define TIMA_PIN_SOURCE GPIO_PinSource8
#define TIMA_AF			GPIO_AF_6

/*-------------TIMB, PA9-----------------*/
#define TIMB			TIM2
#define TIMB_RCC		RCC_APB1Periph_TIM2
#define TIMB_PORT		GPIOA
#define TIMB_PIN		GPIO_Pin_9
#define TIMB_PIN_SOURCE GPIO_PinSource9
#define TIMB_AF			GPIO_AF_10

/*-------------TIMC, PB14-----------------*/
#define TIMC			TIM15
#define TIMC_RCC		RCC_APB2Periph_TIM15
#define TIMC_PORT		GPIOB
#define TIMC_PIN		GPIO_Pin_14
#define TIMC_PIN_SOURCE GPIO_PinSource14
#define TIMC_AF			GPIO_AF_1

/*-------------TIMD, PA1-----------------*/
#define TIMD			TIM2
#define TIMD_RCC		RCC_APB1Periph_TIM2
#define TIMD_PORT		GPIOA
#define TIMD_PIN		GPIO_Pin_1
#define TIMD_PIN_SOURCE GPIO_PinSource1
#define TIMD_AF			GPIO_AF_1

/*-------------M1 Direction, PB2-----------------*/
#define MA_DIR_PORT		GPIOB
#define MA_DIR_PIN		GPIO_Pin_2

/*-------------M2 Direction, PA10-----------------*/
#define MB_DIR_PORT		GPIOA
#define MB_DIR_PIN		GPIO_Pin_10
	
/*-------------M3 Direction, PB15-----------------*/
#define MD_DIR_PORT		GPIOB
#define MD_DIR_PIN		GPIO_Pin_15

/*-------------M4 Direction, PA0-----------------*/
#define MC_DIR_PORT		GPIOA
#define MC_DIR_PIN		GPIO_Pin_0


#ifdef	__cplusplus
}
#endif