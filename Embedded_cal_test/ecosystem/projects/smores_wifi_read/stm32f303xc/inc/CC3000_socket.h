/**
  ******************************************************************************
  * @file    CC3000_socket.h 
  * @author  
  * @version V1.1.0
  * @date    16-June-2014
  * @brief   Header for cc3000_socket.c module
  ******************************************************************************
*/
 
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef CC3000_SOCKET_H
#define	CC3000_SOCKET_H
/*Includes---------------------------------------------------------------------*/
#include "mGeneral.h"
#include "stm32_cc3000.h"

#ifdef	__cplusplus
extern "C" {
#endif
  
/* Function prototypes */
    void tcp_socket_server();
    void tcp_socket_client();
    void udp_socket_communication();
  
/*Variables Declaration for wireless communication*/
	//buffer to transmit
	extern unsigned char* buf_t_send;
	//size of the buffer to transmit
	extern unsigned long buf_t_send_len;
	//size of receive buffer is 25
	extern unsigned char rev_buf[25];
	//received string length
	extern int revLen;

	/*Variable declaration for udp socket*/
	// Two file descriptors
	extern long revSocket, desSocket;
	//Declare socket addresses for source, destionation and local CC3000 
	extern sockaddr_in source_addr, des_addr, my_addr;
	//declare variable to store the receive packet length
	extern socklen_t revPacketLen;

#ifdef	__cplusplus
}
#endif

#endif	/* CC3000_SOCKET_H */

