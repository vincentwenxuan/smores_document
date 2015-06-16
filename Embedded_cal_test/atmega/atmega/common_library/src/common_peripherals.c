#include "common_peripherals.h"

void InitPeripherals()
{
#if defined(LED_GREEN_PORT) && defined(LED_GREEN_PIN)
    set(LED_GREEN_DDR_FULL,LED_GREEN_PIN);
#endif
#if defined(LED_YELLOW_PORT) && defined(LED_YELLOW_PIN)
    set(LED_YELLOW_DDR_FULL,LED_YELLOW_PIN);
#endif
#if defined(LED_BLUE_PORT) && defined(LED_BLUE_PIN)
    set(LED_BLUE_DDR_FULL,LED_BLUE_PIN);
#endif
#if defined(LED_RED_PORT) && defined(LED_RED_PIN)
    set(LED_RED_DDR_FULL,LED_RED_PIN);
#endif
#if defined(USING_SYSTEM_CLOCK)
    system_clock_init();
#endif
}

