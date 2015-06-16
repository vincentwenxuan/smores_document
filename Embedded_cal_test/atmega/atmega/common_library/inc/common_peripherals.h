#ifndef common_peripherals__
#define common_peripherals__

#include "m_general.h"

#if defined(USING_SYSTEM_CLOCK)
    #include "system_clock.h"
#endif

// ------------------------------------------------------------------------------
// initialization function
// ------------------------------------------------------------------------------

void InitPeripherals();

#endif
