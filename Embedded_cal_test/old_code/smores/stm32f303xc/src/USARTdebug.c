/**
  *****************************************************************************************
  * @file    USARTdebug.c 
  * @author  Chaitanya Bhargava (cbhar)
  * @version V1.1.0
  * @date    06-June-2014
  * @brief   This file has basic USART set up and communication functions.
  *          for now it can only do USART send. To enable recieve as well 
  *          comment out the interrupt and the interrupt handler
  *****************************************************************************************
*/

//*****************************************************************************
//
//! \addtogroup Advanced_USART_api
//! @{
//
//*****************************************************************************

#include "USARTplatform_config.h"
#include "USARTdebug.h"
#include "mGeneral.h"

#define TO_HEX(i) ( (((i) & 0xf) <= 9) ? ('0' + ((i) & 0xf)) : ('A' - 10 + ((i) & 0xf)) )

char* itoa(int32_t value, char* result, int base);
char* uitoa(uint32_t value, char* result, int base);
//**************************************************************************************
//
//!  debug_setup
//!
//!  @param    none
//!   
//!  @return   none
//!
//!  @brief    Initializes the USART peripheral based on the setting in USARTplatform_config.h
//!            For now the USART setting are fixed to 8 bit, no parity, 1 stop bit, no 
//!            handshaking
//!
//
//**************************************************************************************
void debug_setup() {
  USART_InitTypeDef usartInitStructure;
  NVIC_InitTypeDef nvicInitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Enable clocks */
  RCC_APB1PeriphClockCmd(DEBUG_USART_RCC, ENABLE);// Make sure this is correct

  GPIO_PinAFConfig(DEBUG_USART_PORT,DEBUG_USART_TX_PIN_SOURCE, DEBUG_USART_AF);    
  GPIO_PinAFConfig(DEBUG_USART_PORT,DEBUG_USART_RX_PIN_SOURCE, DEBUG_USART_AF);
   
  usartInitStructure.USART_BaudRate = DEBUG_USART_BAUD;
  usartInitStructure.USART_WordLength = USART_WordLength_8b;
  usartInitStructure.USART_Parity = USART_Parity_No;
  usartInitStructure.USART_StopBits = USART_StopBits_1;
  usartInitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  usartInitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

 /* Configure pins as AF pushpull */
  GPIO_InitStructure.GPIO_Pin = DEBUG_USART_TX_PIN | DEBUG_USART_RX_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(DEBUG_USART_PORT, &GPIO_InitStructure); 
   
  /* USARTx configured as follow:
  - BaudRate = 19200 baud  
  - Word Length = 8 Bits
  - Stop Bit = 1 Stop Bit
  - Parity = No Parity
  - Hardware flow control disabled (RTS and CTS signals)
  - Receive and transmit enabled
  */

  /* USART configuration */
  USART_Init(DEBUG_USART, &usartInitStructure);

  /* Enable USART */
  USART_Cmd(DEBUG_USART, ENABLE);
  

  /* Enable the USART1 Interrupt */
  // nvicInitStructure.NVIC_IRQChannel = DEBUG_USART_IRQ;
  // nvicInitStructure.NVIC_IRQChannelPreemptionPriority = 3;
  // nvicInitStructure.NVIC_IRQChannelSubPriority = 3;
  // nvicInitStructure.NVIC_IRQChannelCmd = ENABLE;
  // NVIC_Init(&nvicInitStructure);


  // /* Enable the USART interrupts */
  // USART_ITConfig(DEBUG_USART, USART_IT_RXNE, ENABLE);
}
// void USART1_IRQ_Handler(void)
// {
//   USART_ReceiveData(DEBUG_USART);
// }

//**************************************************************************************
//
//!  debug_write_line
//!
//!  @param    str      pointer to the start of the string 
//!   
//!  @return   none
//!
//!  @brief    Send a string over USART and adds '\n' in the end
//!
//
//**************************************************************************************
void debug_write_line(const char* str) {
  debug_write(str);
  debug_write_ch('\n');
}

//**************************************************************************************
//
//!  debug_write_bytes
//!
//!  @param    data      pointer to the start of the array containing the data
//!  @param    len       length of the array
//!   
//!  @return   none
//!
//!  @brief    Send an array over USART
//!
//
//**************************************************************************************
void debug_write_bytes(const uint8_t *data, uint16_t len) {
  for (uint16_t i = 0; i < len; i++) {
    debug_write_ch((char) data[i]);
  }
}

//**************************************************************************************
//
//!  debug_write
//!
//!  @param    str      pointer to the start of the string 
//!   
//!  @return   none
//!
//!  @brief    Send a string over USART. Called by debug write line
//!
//
//**************************************************************************************
void debug_write(const char* str) {
  const char *p = str;
  while (*p) {
    debug_write_ch(*p);
    p++;
  }
}

//**************************************************************************************
//
//!  debug_write_ch
//!
//!  @param    ch      The data byte to be sent over USART
//!   
//!  @return   none
//!
//!  @brief    Sends a byte over USART. Base function called by all other functions
//!
//
//**************************************************************************************
void debug_write_ch(char ch) {
  USART_SendData(DEBUG_USART, ch);
  while (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_TXE) == RESET);
}

//**************************************************************************************
//
//!  debug_write_u8
//!
//!  @param    val      unsigned int 8 value to send over USART
//!  @param    base     The base of the value (16,10,2,random)
//!   
//!  @return   none
//!
//!  @brief    Sends an unsigned int 8 over USART converted to entered base
//!
//
//**************************************************************************************
void debug_write_u8(uint8_t val, uint8_t base) {
  if (base == 16) {
    debug_write_ch(TO_HEX(val >> 4));
    debug_write_ch(TO_HEX(val >> 0));
  } else {
    char buffer[20];
    uitoa(val, buffer, base);
    debug_write(buffer);
  }
}

//**************************************************************************************
//
//!  debug_write_u16
//!
//!  @param    val      unsigned int 16 value to send over USART
//!  @param    base     The base of the value (16,10,2,random)
//!   
//!  @return   none
//!
//!  @brief    Sends an unsigned int 16 over USART converted to entered base
//!
//
//**************************************************************************************
void debug_write_u16(uint16_t val, uint8_t base) {
  if (base == 16) {
    debug_write_ch(TO_HEX(val >> 12));
    debug_write_ch(TO_HEX(val >> 8));
    debug_write_ch(TO_HEX(val >> 4));
    debug_write_ch(TO_HEX(val >> 0));
  } else {
    char buffer[20];
    uitoa(val, buffer, base);
    debug_write(buffer);
  }
}

//**************************************************************************************
//
//!  debug_write_u32
//!
//!  @param    val      unsigned int 32 value to send over USART
//!  @param    base     The base of the value (16,10,2,random)
//!   
//!  @return   none
//!
//!  @brief    Sends an unsigned int 32 over USART converted to entered base
//!
//
//**************************************************************************************
void debug_write_u32(uint32_t val, uint8_t base) {
  if (base == 16) {
    debug_write_ch(TO_HEX(val >> 28));
    debug_write_ch(TO_HEX(val >> 24));
    debug_write_ch(TO_HEX(val >> 20));
    debug_write_ch(TO_HEX(val >> 16));
    debug_write_ch(TO_HEX(val >> 12));
    debug_write_ch(TO_HEX(val >> 8));
    debug_write_ch(TO_HEX(val >> 4));
    debug_write_ch(TO_HEX(val >> 0));
  } else {
    char buffer[20];
    uitoa(val, buffer, base);
    debug_write(buffer);
  }
}

//**************************************************************************************
//
//!  debug_write_i32
//!
//!  @param    val      int 32 value to send over USART
//!  @param    base     The base of the value (16,10,2,random)
//!   
//!  @return   none
//!
//!  @brief    Sends an int 32 over USART converted to entered base
//!
//
//**************************************************************************************
void debug_write_i32(int32_t val, uint8_t base) {
  char buffer[20];
  itoa(val, buffer, base);
  debug_write(buffer);
}

//**************************************************************************************
//
//!  debug_write_u8_array
//!
//!  @param    p       pointer to the unsigned int 8 array to send over USART
//!  @param    len     The length of data to send
//!   
//!  @return   none
//!
//!  @brief    Sends an unsigned int 8 array over USART
//!
//
//**************************************************************************************
void debug_write_u8_array(uint8_t *p, int len) {
  for (int i = 0; i < len; i++) {
    debug_write_u8(p[i], 16);
    debug_write_ch(' ');
  }
}

//**************************************************************************************
//
//!  itoa
//!
//!  @param    value      int 32 value to convert to ascii
//!  @param    result     pointer to the buffer into which to put the ascii value
//!  @param    base       base to which to convert to
//!   
//!  @return   '\0'                 if base not valid
//!  @return   result               ascii value of the entered value
//!
//!  @brief    Sends the int 32 value converted to ascii in the entered base
//!
//
//**************************************************************************************
char* itoa(int32_t value, char* result, int base) {
  // check that the base if valid
  if (base < 2 || base > 36) {
    *result = '\0';
    return result;
  }

  char* ptr = result, *ptr1 = result, tmp_char;
  int tmp_value;

  do {
    tmp_value = value;
    value /= base;
    *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
  } while (value);

  // Apply negative sign
  if (tmp_value < 0) {
    *ptr++ = '-';
  }
  *ptr-- = '\0';
  while (ptr1 < ptr) {
    tmp_char = *ptr;
    *ptr-- = *ptr1;
    *ptr1++ = tmp_char;
  }
  return result;
}

//**************************************************************************************
//
//!  uitoa
//!
//!  @param    value      unsigned int 32 value to convert to ascii
//!  @param    result     pointer to the buffer into which to put the ascii value
//!  @param    base       base to which to convert to
//!   
//!  @return   '\0'                 if base not valid
//!  @return   result               ascii value of the entered value
//!
//!  @brief    Sends the unsigned int 32 value converted to ascii in the entered base
//!
//
//**************************************************************************************
char* uitoa(uint32_t value, char* result, int base) {
  // check that the base if valid
  if (base < 2 || base > 36) {
    *result = '\0';
    return result;
  }

  char* ptr = result, *ptr1 = result, tmp_char;
  int tmp_value;

  do {
    tmp_value = value;
    value /= base;
    *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
  } while (value);

  *ptr-- = '\0';
  while (ptr1 < ptr) {
    tmp_char = *ptr;
    *ptr-- = *ptr1;
    *ptr1++ = tmp_char;
  }
  return result;
}
