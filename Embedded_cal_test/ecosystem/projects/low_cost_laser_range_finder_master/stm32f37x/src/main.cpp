#include "common_peripherals.h"
#include "system_clock.h"
// common components 
#include "common_message_types.h" 
// common messages
#include "usb_interface.h"        
// usb interface
//#include "mBus.h"

// common libraries
#include "project_message_types.h"
#include "error_reporting.h"
#include <string.h> 
//needed for memset
//#define   mBUS 1

// user libraries

#include "SPI.h"
//#include "mUSART.h"
#include "stm32f37x_tim.h"
#include "stm32f37x.h"
#include "stm32f37x_misc.h"
#include "stm32f37x_rcc.h"
#include "stm32f37x_gpio.h"

#define DATA_LENGTH 1000

void RCC_Init()
{
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOE, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOF, ENABLE);
}



// void TIMER3_Init()    // configure the overflow frequency to be around 1.29KHz, f = 72000000/(TIM_Prescaler+1)/(TIM_Period+1)
// {
//     TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
//     TIM_DeInit(TIM3);
//     RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
//     TIM_TimeBaseStructure.TIM_Period=30;
//     TIM_TimeBaseStructure.TIM_Prescaler=1800-1;
//     TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
//     TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
//     TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
//     TIM_Cmd(TIM3, ENABLE);
// }

// void NVIC_Configuration()
// {
//     NVIC_InitTypeDef NVIC_InitStructure;
//     NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
//     NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;   // interrupt source
//     NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;// interrupt priority
//     NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//     NVIC_Init(&NVIC_InitStructure);
// }

void GPIO_init()
{
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_5;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_2;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOB, &GPIO_InitStruct);
}

volatile int i = 0;
volatile bool flag = 0;
volatile bool flag_end = 0;
volatile bool flag_start = 0;
u16 data[DATA_LENGTH];

 
int main (void){
    InitPeripherals();

	UsbInterface usb = UsbInterface();
    usb.Init();

    RCC_Init();
	GPIO_init();
    //TIMER3_Init();    // initialize the Timer 3
    //NVIC_Configuration();    // initialize the interrupt for Timer 3
    SPI3_Init();    // initialize the SPI3

    // USART_init(USART1, GPIOA, RCC_APB2Periph_USART1, GPIO_Pin_9, GPIO_Pin_10, GPIO_PinSource9, GPIO_PinSource10);     // initialize the USART to transmit the data to the laptop
    // USART_set_baudrate(USART1, 19200);    // set the baudrate for the USART
    
    for (int m=0; m<DATA_LENGTH; m++)    // initialize the buffer to store the data from the sensor
    {
        data[m] = 0;
    }
    //memset(data, 0, DATA_LENGTH);
    
    DelayMilliseconds(300);
	
	PlayTimebase();
    while(1) {
		mGreenON;
		do {
			if (flag) {
				flag = 0;
				data[0] = SPIx_ReadWriteByte(0x0000);		
			}
		} while ((data[0] != 0xABCD) && (flag_start == 0));

		flag_start = 1;
		mBlueTOGGLE;
		
		if (flag && (flag_end == 0)) {
			flag = 0;
			i++;
			data[i] = SPIx_ReadWriteByte(0x0000);
			if (data[i] == 0x1111) {
				i = 0;
				mYellowTOGGLE;
				flag_end = 1;
			}
		}

		if (flag_end) {
			mWhiteTOGGLE;
			for (int ind = 0; ind < DATA_LENGTH; ind++) {
				usb.printf("%d", data[i]);
            }
			flag_end = 0;
			mGreenOFF;
		}

        
        
    }
    return(0);
}

void timerCallback1(void)
{
    GPIOB->ODR ^= (1<<5);
	flag = 1;
    
}

