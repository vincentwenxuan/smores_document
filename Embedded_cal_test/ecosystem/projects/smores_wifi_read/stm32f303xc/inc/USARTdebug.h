/*
Author Chaitanya Bhargava (cbhar@seas.upenn.edu)
Settings for USART usage on the STM32F303
mGeneral includes all the ST library files and the wait functions
The user will have to define the RXNE interrupt and what needs to be 
done in there. (NOTE- Not tested)
*/
#include "USARTplatform_config.h"

#ifndef USARTDEBUG_H
#define	USARTDEBUG_H

#include <stdint.h>

#ifdef	__cplusplus
extern "C" {
#endif

  void debug_setup();
  void debug_write_line(const char* str);
  void debug_write_bytes(const uint8_t *data, uint16_t len);
  void debug_write(const char* str);
  void debug_write_ch(char ch);
  void debug_write_u8(uint8_t val, uint8_t base);
  void debug_write_u8_array(uint8_t *p, int len);
  void debug_write_u16(uint16_t val, uint8_t base);
  void debug_write_u32(uint32_t val, uint8_t base);
  void debug_write_i32(int32_t val, uint8_t base);
  extern void debug_on_rx(uint8_t* data, uint16_t len);

#ifdef	__cplusplus
}
#endif

#endif	/* DEBUG_H */

