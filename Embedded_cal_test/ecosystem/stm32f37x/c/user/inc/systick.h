#ifndef SYSTICK_H
#define SYSTICK_H

#include <stdint.h>

#define SYSTICK_RELOAD_VALUE        (0x00FFFFFF) // max value is 2^24 9000000
#define SYSTICK_NUM_TICS_PER_SECOND (72000000)

#ifdef __cplusplus
extern "C" {
#endif

void systick_init();
void systick_block(uint32_t ticks);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif //SYSTICK_H