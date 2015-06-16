/** 
** This template will guide you in making a ST M4 project.  
** All locations with double comments require your attention (** or ///)
** Please perform the following tasks:
**
** Edit peripherals.h to enable the desired peripherals
** Edit peripherals.c to enable the desired peripherals
** Include custom headers
** Write your code in the specified areas
** Comment your code
** Delete this comment and put a description of your project and the used hardware
**/

#include "stm32f37x.h"
#include "mGeneral.h"
#include "common_peripherals.h"

/// Include your custom headers here
#include "packet.h"

/// Initialize variables here

// main function
int main(void)
{
  InitPeripherals();  // configure GPIO, USB, I2C, ect based on peripherals.h
  
  mRedON;
  mGreenON;
  mBlueON;
  mWhiteON;
  mOrangeON;
  
  _delay_ms(500);
  _delay_ms(500);
  
  mRedOFF;
  mGreenOFF;
  mBlueOFF;
  mWhiteOFF;
  mOrangeOFF;

  extern uint8_t Receive_Buffer[64];
  extern uint32_t Receive_length;
  uint8_t *rx_ptr;
  uint8_t rx_len;
  int ret = -1;
  uint8_t rx_buf[130];
  uint8_t tx_buf[136];
  uint8_t tx_len;
  int count = 0;
  uint8_t resp;
  
  // main loop
  while(1)
  {
    CDC_Receive_DATA();
    if(Receive_length != 0)
    {
      ret = process_serial_data(Receive_Buffer, Receive_length, &rx_ptr, &rx_len);
      if(ret == 0)
      {
        ret = ZigBee_SetModeTx();
        if(ret != 0)
        {
          ret = ZigBee_ReadState();
          while(ret != STATUS_PLL_ON)
          {
            mBlueTOGGLE;
            ret = ZigBee_ReadState();
          }
        }
        mGreenTOGGLE;
        ZigBee_WriteFrame(rx_ptr, rx_len);
        resp = ZigBee_ReadIrq();
        while((resp & 0x08) == 0x00)
        {
          mBlueTOGGLE;
          resp = ZigBee_ReadIrq();
        }
        ret = ZigBee_SetModeRx();
        if(ret != 0)
        {
          ret = ZigBee_ReadState();
          while(ret != STATUS_RX_ON)
          {
            mBlueTOGGLE;
            ret = ZigBee_ReadState();
          }
        }
      }
      Receive_length = 0;
    }

    resp = ZigBee_ReadIrq();
    if((resp & 0x08) == 0x08)
    {
      uint8_t rx_len = 0;
      ZigBee_ReadFrame(rx_buf, &rx_len);
      if(rx_len > 0)
      {
        if(rx_buf[rx_len-1] & 0x80) // Valid CRC
        {
          if(rx_buf[0] == 't')
          {
            mBlueTOGGLE;
            create_packet(rx_len-6, rx_buf[0], &rx_buf[1], tx_buf, &tx_len);
            CDC_Send_DATA(tx_buf, tx_len);
          }
        }
      }
    }

    if(count == 50000)
    {
      mRedTOGGLE;
      count = 0;
    }
    count++;
  }
  
  return 0;
}
