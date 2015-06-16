/*
Author Chaitanya Bhargava (cbhar@seas.upenn.edu)
Settings for USART usage on the STM32F303
mGeneral includes all the ST library files and the wait functions
The user will have to define the RXNE interrupt and what needs to be 
done in there. (NOTE- Not tested)
*/
#include "USARTplatform_config.h"
#include "USARTdebug.h"
#include "common_peripherals.h"

#define TO_HEX(i) ( (((i) & 0xf) <= 9) ? ('0' + ((i) & 0xf)) : ('A' - 10 + ((i) & 0xf)) )

char* itoa(int32_t value, char* result, int base);
char* uitoa(uint32_t value, char* result, int base);

void debug_setup() {
  USART_InitTypeDef usartInitStructure;
//  NVIC_InitTypeDef nvicInitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Enable clocks */
  RCC_APB1PeriphClockCmd(DEBUG_USART_RCC, ENABLE);

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
void debug_write_line(const char* str) {
  debug_write(str);
  debug_write_ch('\n');
}

void debug_write_bytes(const uint8_t *data, uint16_t len) {
  for (uint16_t i = 0; i < len; i++) {
    debug_write_ch((char) data[i]);
  }
}

void debug_write(const char* str) {
  const char *p = str;
  while (*p) {
    debug_write_ch(*p);
    p++;
  }
}

void debug_write_ch(char ch) {
  USART_SendData(DEBUG_USART, ch);
  while (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_TXE) == RESET);
}

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

void debug_write_i32(int32_t val, uint8_t base) {
  char buffer[20];
  itoa(val, buffer, base);
  debug_write(buffer);
}

void debug_write_u8_array(uint8_t *p, int len) {
  for (int i = 0; i < len; i++) {
    debug_write_u8(p[i], 16);
    debug_write_ch(' ');
  }
}

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
