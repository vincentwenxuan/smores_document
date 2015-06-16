#include "stm32f3_discovery.h"
#include "stdio.h"
#include "mUSB.h"

__asm__(".global _printf_float");
__asm__(".global _scanf_float");


int main( void )
{
    //mBusInit();
    mUSBInit();

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
    
    mWaitms(3000);
    while(1)
    {
        printf("Hello!\n\r");
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
        mWaitms(10);
    }
}
