/** 
** This template will guide you in making a ST M4 project.  
** All locations with double comments require your attention (** or ///)
** Please perform the following tasks:
**
** Edit project_peripherals.h to enable the desired peripherals
** Include custom headers
** Write your code in the specified areas
** Delete this comment and put a description of your project and the used hardware
**/

//  #include "common_peripherals.h"
#include "common_peripherals.h"
#include "stm32f30x_gpio.h"
#include "stm32f30x_rcc.h"

//#define BSRR_VAL 0xC000
GPIO_InitTypeDef        GPIO_InitStructure;

/// Include your custom headers here

/// Initialize variables here

// main function
uint32_t counter; 
int main(void)
{
  //InitPeripherals();  // configure GPIO, USB, I2C, ect based on peripherals.h
  counter = 0; 
  
  //GPIO_DeInit(GPIOE); 
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOE, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15 | GPIO_Pin_14;
    //GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_15;

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_2;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOE, &GPIO_InitStructure);

 

  /*RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOF, ENABLE); 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; 
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_2; 
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOF, &GPIO_InitStructure);
  */
  /// Initialize code here
  
  // main loop
  while(1)
  {
   counter= counter + 1; 
   GPIO_WriteBit(GPIOE, GPIO_Pin_15, Bit_SET);
   
   if(counter > 1000){
       GPIO_WriteBit(GPIOE, GPIO_Pin_14, Bit_RESET);
   }  
   GPIO_WriteBit(GPIOE, GPIO_Pin_14,Bit_SET);

  }
  
  return(0);
}