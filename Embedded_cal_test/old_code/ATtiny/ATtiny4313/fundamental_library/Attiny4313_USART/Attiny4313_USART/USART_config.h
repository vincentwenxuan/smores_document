/*
 * USART_config.h
 *
 * Created: 6/19/2014 7:49:55 AM
 *  Author: Chao Liu (chao.liu0307@gmail.com)
 */ 


#ifndef USART_CONFIG_H_
#define USART_CONFIG_H_

#ifdef	__cplusplus
extern "C" {
#endif
#define ENABLE 1
#define DISABLE 0

#define USART_BAUD	9600	// baud rate choice for 1 MHz Clock: 2400, 4800, 9600(default), 14400, 19200, 28800, 38400, 57600 
#define USART_RECEIVER_STATUS	ENABLE
#define USART_TRANSMITTER_STATUS	ENABLE
#define USART_STOP_BIT	1	// User can choose 1 stop-bit or 2 stop-bit
#define USART_CHARACTER_SIZE	8    // User can choose 5-bit, 6-bit, 7-bit, 8-bit. Other selection will be set to be 8-bit.


#ifdef	__cplusplus
}
#endif

#endif /* USART_CONFIG_H_ */