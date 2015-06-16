// -----------------------------------------------------------------------------
// ATmega88 TWI IIC subsystem
// version: 1.0
// date: August 13, 2014
// authors: Chao Liu (chao.liu0307@gmail.com)
// -----------------------------------------------------------------------------

#ifndef TWI_H_
#define TWI_H_

#include "m_general.h"
#include <util/twi.h>

// -----------------------------------------------------------------------------
// Public
// -----------------------------------------------------------------------------

#define TWI_BUFFER_SIZE 255

#define TWI_ACK()    {(TWCR = _BV (TWIE) | _BV (TWINT) | _BV (TWEN) | _BV (TWEA));}
#define TWI_NACK()   {(TWCR = _BV (TWIE) | _BV (TWINT) | _BV (TWEN));}

extern volatile bool receive_done;

struct TWI_data
{
    uint8_t command;
    uint8_t data_length;
    uint8_t data[];
};

extern struct TWI_data twi_data;

void twi_init(uint8_t slave_address);

// -----------------------------------------------------------------------------
// Private
// -----------------------------------------------------------------------------




#endif
