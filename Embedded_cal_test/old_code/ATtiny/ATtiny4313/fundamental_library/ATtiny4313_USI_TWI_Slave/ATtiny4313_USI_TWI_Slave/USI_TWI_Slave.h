/*
 * USI_TWI_Slave.h
 *
 * Created: 6/22/2014 3:58:13 PM
 *  Author: Chao Liu(chao.liu0307@gmail.com)
 */ 


#ifndef USI_TWI_SLAVE_H_
#define USI_TWI_SLAVE_H_

#ifdef	__cplusplus
extern "C" {
#endif

#include "USI_TWI_Slave_config.h"
#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

void USI_TWI_Slave_Init( uint8_t slave_address );
void USI_TWI_Transmit_Byte( uint8_t data );
uint8_t USI_TWI_Receive_Byte( void );
bool USI_TWI_DataIn_ReceiveBuffer( void );
void USI_TWI_OnStart( void (*function) () );
void USI_TWI_OnStop( void (*function) () );

#ifdef	__cplusplus
}
#endif

#endif /* USI_TWI_SLAVE_H_ */