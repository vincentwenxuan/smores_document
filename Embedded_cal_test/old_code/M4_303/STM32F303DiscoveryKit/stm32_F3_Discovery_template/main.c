#include "main.h"

static uint16_t __timer = 0;

void Delay(uint16_t n)
{
    __timer = n;
    while (__timer > 0);
}

int main()
{
    uint32_t state = 0;
    STM_EVAL_LEDInit(LED3);
    STM_EVAL_LEDInit(LED4);
    STM_EVAL_LEDInit(LED5);
    STM_EVAL_LEDInit(LED6);
    STM_EVAL_LEDInit(LED7);
    STM_EVAL_LEDInit(LED8);
    STM_EVAL_LEDInit(LED9);
    STM_EVAL_LEDInit(LED10);
    STM_EVAL_PBInit(BUTTON_USER, BUTTON_MODE_GPIO);

    if (SysTick_Config(SystemCoreClock / 1000))
    { 
        while (1);
    }

    while (1) {
        //STM_EVAL_LEDToggle(LED3);
        //STM_EVAL_LEDToggle(LED4);
        //STM_EVAL_LEDToggle(LED5);
        //STM_EVAL_LEDToggle(LED6);
        //STM_EVAL_LEDToggle(LED7);
        //STM_EVAL_LEDToggle(LED8);
        STM_EVAL_LEDToggle(LED9);
        STM_EVAL_LEDOn(LED10);
        
        
        state = STM_EVAL_PBGetState(BUTTON_USER);
        if (state == 0) {
            STM_EVAL_LEDOn(LED3);
        } else {
            STM_EVAL_LEDOff(LED3);
        }
        Delay(100);
    }
}

void Timer_Decrement()
{
    if (__timer > 0)
        __timer--;
} 

void assert_param(int x)
{

}
