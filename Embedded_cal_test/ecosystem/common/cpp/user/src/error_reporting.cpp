// Blink a one byte binary code in long (1) and short (0) LED blinks. The most 
// significant bit is sent first.  The minimal number of bits is used to 
// represent the number.

#include "common_peripherals.h"
#include "error_reporting.h"
#include "communication_interface.h"
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include "common_message_types.h"

const uint32_t period = 10000000; // period of bits
const float short_duty = 0.01;    // on time fraction for zeros
const float long_duty = 0.4;      // on-time fraction for ones

CommunicationInterface* error_reporting_com1 = NULL;
CommunicationInterface* error_reporting_com2 = NULL;

#define MAX_ERROR_LEN 56 // maximum characters in error/warn message

// the number clock cycles wasted will be 4 * val
#define WaitCycles(val)   do{register uint32_t i;for(i=0;i<val;++i)\
    __asm__ __volatile__ ("nop\n\t":::"memory");} while(0);

// short blink represents zero
void ShortBlink() {
  uint32_t on_time = period * short_duty;
  mErrorON;
  WaitCycles(on_time);
  mErrorOFF;
  WaitCycles(period - on_time);
}

// long blink represents one
void LongBlink() {
  uint32_t on_time = period * long_duty;
  mErrorON;
  WaitCycles(on_time);
  mErrorOFF;
  WaitCycles(period - on_time);
}

// emit a byte code as a binary blink sequence
void BlinkCode(uint8_t byte) {
  // shift out high zeros
  uint8_t shift = 0;
  while((byte & 0b10000000) == 0) {
    byte = byte << 1;
    shift++;
  }
  // blink out remaining bits, high to low
  while(shift < 8) {
    if((byte & 0b10000000) == 0) {
      ShortBlink();
    }
    else {
      LongBlink();
    }
    byte = byte << 1;
    shift++;
  }
}

// Trap execution fatally in an infinite loop, repeatedly emitting the error 
// code byte in binary over the error LED and the error message over selected 
// coms channels.
void ErrorTrap(unsigned char code) {
  while(1) {
    BlinkCode(code);
    WaitCycles(3*period);
  }
}

// Pass execution on but emit the warning message over selected coms channels.
// Requires a byte code and a string message, and optionally accepts 
// printf-style arguments.
//   Examples:
//      do:     WarnReport(17, "CPAL_I2C_Write failed.")
//      yields: "\r\nwarn 17: CPAL_I2C_Write failed\r\n"
//
//      do:     WarnReport(17, "CPAL_I2C_Write failed, CPAL_STATE = %u", 99)
//      yields: "\r\nwarn 17: CPAL_I2C_Write failed, CPAL_STATE = 99\r\n"
void WarnReport(uint8_t warn_code, const char* warn_string, ...) {
  //mErrorON;
  // assemble standard prefix containing err_code
  // and record number of characters used
  uint8_t warn_data[MAX_ERROR_LEN];
  uint8_t prefix_len = snprintf((char*)warn_data, MAX_ERROR_LEN, "\r\nw%u: ", warn_code);

  // append err_string interpreted with any optional printf-type arguments
  // and record number of characters used
  va_list argptr;
  va_start(argptr, warn_string);
  uint8_t string_len = vsnprintf((char*)&warn_data[prefix_len], MAX_ERROR_LEN-prefix_len, warn_string, argptr);
  va_end(argptr);

  // find end of generated string, and restrict length to limits
  uint8_t warn_len = prefix_len + string_len;
  if(warn_len > MAX_ERROR_LEN-2)
    warn_len = MAX_ERROR_LEN-2;

  // append standard postfix to aid terminal readability
  warn_data[warn_len] = '\r';
  warn_data[warn_len+1] = '\n';
  warn_len = warn_len + 2;

  // send warning message
  if(error_reporting_com1 != NULL) {
    error_reporting_com1->SendPacket(kTypeWarn, warn_data, warn_len);
    error_reporting_com1->SendNow();
  }
  if(error_reporting_com2 != NULL) {
    error_reporting_com2->SendPacket(kTypeWarn, warn_data, warn_len);
    error_reporting_com2->SendNow();
  }

  //mErrorOFF;
}