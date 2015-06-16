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
#include "stm32f3_discovery.h"

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

int main (void){
    InitPeripherals();
    
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
    
    GPIO_init();
    PlayTimebase();
    
    UsbInterface usb = UsbInterface();
    usb.Init();
    
    u32 state = 0;
    
    STM_EVAL_LEDInit(LED3);
    STM_EVAL_LEDInit(LED4);
    STM_EVAL_LEDInit(LED5);
    STM_EVAL_LEDInit(LED6);
    STM_EVAL_LEDInit(LED7);
    STM_EVAL_LEDInit(LED8);
    STM_EVAL_LEDInit(LED9);
    STM_EVAL_LEDInit(LED10);
    STM_EVAL_PBInit(BUTTON_USER, BUTTON_MODE_GPIO);
//    int flag = 100;
    while(1)
    {
//        usb.printf("flag = %d\n\r", flag);
//        usb.SendNow();
        STM_EVAL_LEDOn(LED4);
        STM_EVAL_LEDOn(LED5);
        STM_EVAL_LEDOff(LED6);
        STM_EVAL_LEDOff(LED7);
        STM_EVAL_LEDOn(LED8);
        STM_EVAL_LEDOn(LED9);
        STM_EVAL_LEDToggle(LED10);
        state = STM_EVAL_PBGetState(BUTTON_USER);
        if (state == 0) {
            STM_EVAL_LEDOn(LED3);
        } else {
            STM_EVAL_LEDOff(LED3);
        }
        DelayMilliseconds(100);
    }
    return(0);
}

void timerCallback1(void)
{
    GPIOB->ODR ^= (1<<5);
}
