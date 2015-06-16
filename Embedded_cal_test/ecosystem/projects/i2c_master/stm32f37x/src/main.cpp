// I2C Master Mode Example
// Writes to registers, reads back to registers, displays comparison over USB.

// LED OPERATION
// GREEN:   ON if reached main loop
// WHITE:
// YELLOW:  TOGGLE at main loop frequency
// RED:     ERROR

// hardware configuration
#include "common_peripherals.h"

// common components
#include "common_message_types.h" // common messages
#include "usb_interface.h"        // usb interface
#include "mBus.h"

// common libraries
#include "project_message_types.h"
#include "error_reporting.h"

#define AUX_MBUS 2

#define PIN_TOGGLE GPIOB->ODR ^= (1<<14)

const uint8_t slave_addr = 0;

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

  uint32_t in_values[5] = {1000000000, 1000000001, 1000000002, 1000000003, 1000000004};
  uint32_t out_values[5];

  //////////////////////////////////////////////////////////////////////////////
  // Main Loop
  while(1) {
    DelayMilliseconds(5);
    mYellowON;

    // clear retrieved values (no cheating)
    memset(out_values, 0, 5); 

    // write one uint32_t values to register 0x1000
    mBusMasterWriteBurst(slave_addr, 0x1000, 4, (uint8_t*)&in_values[0], AUX_MBUS);
    DelayMicroseconds(500);
    
    // write four uint32_t values starting at register 0x1001
    //   this fills registers 0x1001, 0x1002, 0x1003, 0x1004
    mBusMasterWriteBurst(slave_addr, 0x1001, 16, (uint8_t*)&in_values[1], AUX_MBUS);
    DelayMicroseconds(500);

    // read four uint32_t values starting at register 0x1000
    //   this calls from registers 0x1000, 0x1001, 0x1002, 0x1003
    mBusMasterReadBurstStart(slave_addr, 0x1000, 16, AUX_MBUS);
    DelayMicroseconds(500);
    mBusReadBurstDataNB(16, (uint8_t*)&out_values[0], AUX_MBUS);

    // read one uint32_t value from register 0x1004
    mBusMasterReadBurstStart(slave_addr, 0x1004, 4, AUX_MBUS);
    DelayMicroseconds(500);
    mBusReadBurstDataNB(4, (uint8_t*)&out_values[4], AUX_MBUS);

    // compare out values to in values
    for(uint8_t i = 0; i<5; i++) {
      usb.printf("%x, %i == %i\r\n", 0x1000 + i, (unsigned int)out_values[i], (unsigned int)in_values[i]);
    }
    usb.printf("\r\n");
    usb.SendNow();

    DelayMilliseconds(5);
    mYellowOFF;
    PIN_TOGGLE;
  } // while(1)
  return(0);
}