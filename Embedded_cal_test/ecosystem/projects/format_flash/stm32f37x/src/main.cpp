#include "persistent_memory.h"
#include "common_peripherals.h"
#include "system_clock.h"

int main(void)
{
  InitPeripherals();

  mRedON; mGreenOFF;
  DelaySeconds(1);

  PersistentMemory mem;
  mem.Freeze();
  mem.Format();

  mRedOFF; mGreenON;

  while(1) {}
  return(0);
}