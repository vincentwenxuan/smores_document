//-----------------------------------------------------------------------------
// MAEVARM M4 STM32F373 general
// version: 1.0
// date: March 22, 2013
// author: Neel Shah (neels@seas.upenn.edu)
//-----------------------------------------------------------------------------

#include "mGeneral.h"

void mInit(void)
{

	// GPIO_InitTypeDef GPIO_InitStructure;

	// RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	// RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	// RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
	// RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);
	// RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOE, ENABLE);
	// RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOF, ENABLE);

	// //red,green,yellow led
	// GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_15 | GPIO_Pin_14 | GPIO_Pin_13;
	// GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	// GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	// GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	// GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	// GPIO_Init(GPIOC, &GPIO_InitStructure);
	// // GPIO_PinLockConfig(GPIOC, GPIO_Pin_14 | GPIO_Pin_13);

	// //purple led
	// GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_12 ;
	// GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	// GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	// GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	// GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	// GPIO_Init(GPIOB, &GPIO_InitStructure);
	// // GPIO_PinLockConfig(GPIOB, GPIO_Pin_12);

	// //pink led
	// GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_13;
	// GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	// GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	// GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	// GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	// GPIO_Init(GPIOB, &GPIO_InitStructure);




	//End of Gabby comment
	// GPIO_PinLockConfig(GPIOB, GPIO_Pin_13);
	
	// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; 
 //    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
 //    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; 
	// GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	// GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
 //    GPIO_Init(GPIOA, &GPIO_InitStructure);
	// GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_0);
 //    RCC_MCOConfig(  RCC_MCOSource_PLLCLK_Div2 );

// 	mGreenOFF;
// 	mYellowOFF;
// 	mRedOFF;
// 	mPurpleOFF;
// 	mPinkOFF;
}








