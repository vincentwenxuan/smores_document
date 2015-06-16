/*
 * USI_TWI_Slave_config.h
 *
 * Created: 6/22/2014 4:04:25 PM
 *  Author: Chao Liu(chao.liu0307@gmail.com)
 */ 


#ifndef USI_TWI_SLAVE_CONFIG_H_
#define USI_TWI_SLAVE_CONFIG_H_

#ifdef	__cplusplus
extern "C" {
#endif

#define TWI_RX_BUFFER_SIZE  ( 16 )
#define TWI_RX_BUFFER_MASK  ( TWI_RX_BUFFER_SIZE - 1 )

#if ( TWI_RX_BUFFER_SIZE & TWI_RX_BUFFER_MASK )
#  error TWI RX buffer size is not a power of 2
#endif

// permitted TX buffer sizes: 1, 2, 4, 8, 16, 32, 64, 128 or 256

#define TWI_TX_BUFFER_SIZE ( 16 )
#define TWI_TX_BUFFER_MASK ( TWI_TX_BUFFER_SIZE - 1 )

#if ( TWI_TX_BUFFER_SIZE & TWI_TX_BUFFER_MASK )
#  error TWI TX buffer size is not a power of 2
#endif

#ifdef	__cplusplus
}
#endif

#endif /* USI_TWI_SLAVE_CONFIG_H_ */