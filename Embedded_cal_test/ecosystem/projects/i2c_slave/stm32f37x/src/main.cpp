// I2C Slave Listen Mode Example
// Accepts reads and writes to a small set of 32-bit registers.

// LED OPERATION
// GREEN:  ON if reached main loop
// WHITE:
// YELLOW: TOGGLE on register access
// RED:    ERROR

// hardware configuration
#include "common_peripherals.h"

// common components
#include "common_message_types.h" // common messages
#include "usb_interface.h"        // usb interface
#include "mBus.h"
#include "stdio.h"

// common libraries
#include "project_message_types.h"

#define AUX_MBUS 2

#define PIN_TOGGLE GPIOB->ODR ^= (1<<14)

const uint8_t slaveAddr = 0;

volatile uint32_t registers[255];
volatile uint8_t flags[255];

////////////////////////////////////////////////////////////////////////////////
// Entry Point

int main(void)
{
  InitPeripherals();
  mRedON; mGreenON; mBlueON; mWhiteON; mYellowON;
  DelaySeconds(0.5);
  mRedOFF; mGreenOFF; mBlueOFF; mWhiteOFF; mYellowOFF;
  DelaySeconds(1);

  // configure main loop toggle pin
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
  GPIO_InitStructure.GPIO_Pin =  1<<14;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_2;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_PinLockConfig(GPIOB, 1<<14);

  UsbInterface usb;
  usb.Init();

  mGreenON;

  mBusInitSlave(AUX_MBUS);
  mBusSlaveListen(AUX_MBUS);

  //////////////////////////////////////////////////////////////////////////////
  // Main Loop
  while(1) {
    DelayMicroseconds(1000); // to provide realistic main loop rates
    PIN_TOGGLE;
  } // while(1)
  return(0);
}

void SlaveRx_UserCallback(uint16_t reg_addr, uint32_t value) {
  // receive value over I2C and store to internal registers
  mYellowTOGGLE;
  DelayMicroseconds(20);  // to simulate computational delays
  registers[reg_addr-0x1000] = value;
}

uint32_t SlaveTx_UserCallback(uint16_t reg_addr) {
  // read value from internal registers and transmit over I2C
  mYellowTOGGLE;
  DelayMicroseconds(20);  // to simulate computational delays
  return registers[reg_addr-0x1000];
}