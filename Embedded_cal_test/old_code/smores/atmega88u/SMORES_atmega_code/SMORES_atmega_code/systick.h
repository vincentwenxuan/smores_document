#ifndef SYSTICK_H
#define SYSTICK_H

#include "m_general.h"

void systick_init();
void systick_reset();
void systick_disable();
float get_curr_time();

#endif //SYSTICK_H
