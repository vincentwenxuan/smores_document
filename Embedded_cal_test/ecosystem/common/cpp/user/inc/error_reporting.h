#ifndef ERROR_REPORTING_H
#define ERROR_REPORTING_H

#include <stdint.h>

// error codes must be defined here
#define ERROR_NMI         1
#define ERROR_HARD_FAULT  2
#define ERROR_MEM_MANAGE  3
#define ERROR_BUS_FAULT   4
#define ERROR_USAGE_FAULT 5
#define ERROR_SVC         6
#define ERROR_DEBUG_MON   7
#define ERROR_PEND_SV     8

#define ERROR_ZIGBEE_INIT     9
#define ERROR_TIMEBASE_CONFIG 10

#define ERROR_MBUS_MASTER_WRITE_BURST       11
#define ERROR_MBUS_MASTER_READ_BURST_START  12
#define ERROR_CPAL_TIMEOUT                  13
#define ERROR_CPAL_I2C_ERR                  14

#define ERROR_ENCODER_DIRECTION_UNIMP       15
#define ERROR_ENCODER_SPEED_CHANGE_ERR      16


////////////////////////////////////////////////////////////////////////////////
// Only C++ files can set the communication interface pointers.
#ifdef __cplusplus

class CommunicationInterface;
extern CommunicationInterface* error_reporting_com1;
extern CommunicationInterface* error_reporting_com2;

#endif // __cplusplus


////////////////////////////////////////////////////////////////////////////////
// Both C and C++ files can make use of error reporting
#ifdef __cplusplus
extern "C" {
#endif

// ErrorTrap() must be used only with #define error codes.  Add codes in 
// error_reporting.h as needed.
void ErrorTrap(unsigned char code);


void WarnReport(uint8_t code, const char* string, ...);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // ERROR_REPORTING_H