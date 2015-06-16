/**
  *****************************************************************************************
  * @file    CC3000_socket.c 
  * @author  
  * @version V1.1.0
  * @date    16-June-2014
  * @brief   This source file contains the socket functions to initialize sockets using 
  *			 different protocols, either UDP or TCP, and echos the message in an infinite 
  *			 loop.
  *
  *****************************************************************************************
*/

//*****************************************************************************
//
//! \addtogroup SMORES_wifi_custom_api
//! @{
//
//*****************************************************************************


/* ---------------------------------Includes ------------------------*/
#include "CC3000_socket.h"
#include "common_peripherals.h"
#include "project_peripherals.h"


/*Variables Definitions for wireless communication*/
//buffer to transmit
unsigned char* buf_t_send;
//size of the buffer to transmit
unsigned long buf_t_send_len;
//size of receive buffer is 25
unsigned char rev_buf[25];
//received string length
int revLen;

/*Variable definition for udp socket*/
// Two file descriptors
long revSocket, desSocket;
//Declare socket addresses for source, destionation and local CC3000 
sockaddr_in source_addr, des_addr, my_addr;
//declare variable to store the receive packet length
socklen_t revPacketLen;

int recvtimeout=5; //5ms timeout
int nonBlock=SOCK_ON; //can be used to achieve 0 second timeout

//*****************************************************************************
//
//!  tcp_socket_server
//!
//!  @param        none
//!
//!  @return       none
//!
//!  @brief        This function is called if TCP socket communication is 
//!				   desired, in this case, the MCU would serve as a server and
//!				   listen to the socket for communication.
//
//*****************************************************************************
void tcp_socket_server(){
	debug_write("TCP Socket Server Initialization");
	/*Variable declaration*/
	//One listening socket and one communication socket
	int svrSocket, newSocket, testSocket=-1;
	//declare one server socket address and one client address
	sockaddr_in svr_addr, client_addr;
	//length of the client address
	socklen_t alen=sizeof(client_addr);
	unsigned char msg[sizeof(rev_buf)];
	//Used to set socket options
	unsigned char recvtimeout=5;	//2 msec
	int recv_size=0;
	
	//Initialize the listening socket
	if((svrSocket=socket(AF_INET, SOCK_STREAM, 0))<0){
		debug_write("sever socket initialization failed");
		while(1);
	}
	/*assign fields for server socket address*/
	//Zero out all fields first
	memset((char*) &svr_addr, 0, sizeof(svr_addr)); 
	//AF_INET is used for IPv4
	svr_addr.sin_family	   	   = AF_INET;
	//Use receive port number defined in the beginning 
	svr_addr.sin_port		   = htons(RECV_PORT_NUM);
	//Listen to all IP addresses
	svr_addr.sin_addr.s_addr   = LISTEN_ADDR;
	//Bind the file descriptor (svrSocket) to the socket address
	if(bind(svrSocket, (sockaddr*) &svr_addr, sizeof(svr_addr))<0){
		debug_write("bind failed");
		while(1);
	}
	//inifinity time out specification
	netapp_timeout_values(0,0,0,0);
	/*Start listening to the socket for communication, maximum pending 
	 *number is 4
	 */
	if(listen(svrSocket, MAX_LISTEN_QUEUE)<0){
		debug_write("listen failed");
	}
	//Transmit and Receive in an infinite loop
	for(;;) {
		debug_write("Waiting for TCP connection");
		mYellowON;
		
		//loop until a connection has been established, loop is not really 
		//neccessary since the socket operations are synchronous
		while((newSocket=accept(svrSocket,(sockaddr*)&client_addr, &alen))< 0){
		}
		debug_write("Connected");
		mYellowOFF;
		//Transmit to the client
		send(newSocket, buf_t_send, buf_t_send_len, 0);
		setsockopt(newSocket, SOL_SOCKET, SOCKOPT_RECV_TIMEOUT, &recvtimeout, sizeof(recvtimeout));
		
		do{
			
			//"heart-beat" of the circuit
			mBlueTOGGLE;
			
			
			/*
			//Code to test continuous transmission 
			debug_write("L");
			do{
				test_fail=0;
				send(newSocket, buf_t_send, buf_t_send_len, 0);
			}while(test_fail);
			*/
			
			
			
			/*
			//Code to test Receive timeout functionality			
			//When used with Matlab tcp_recv_test script, problem occurs (CC3000 hangs)
			revLen=recv(newSocket, rev_buf, 3, 0);	
			if(revLen>0){
				//toggle red led when a message is received
				mRedTOGGLE;
				debug_write_bytes(rev_buf, revLen);
				motor_test(rev_buf[0]);
				mWait(200000);
				send(newSocket, buf_t_send, buf_t_send_len, 0);
				mWait(200000);
			}
			*/
			
			
			
			/* 
			//Codes to test two TCP connections
			
			send(newSocket, buf_t_send, buf_t_send_len, 0);
			while(testSocket<0){
				testSocket=accept(svrSocket,(sockaddr*)&client_addr, &alen);
				debug_write("No");
			}
			mWaitms(2000);			
			if(available(testSocket)==1){
				revLen=recv(testSocket, rev_buf, 10, 0);
				mWaitms(20);
				send(testSocket, rev_buf, revLen, 0);
				
			}
			*/
			
			
			
			// Code to test slow continous receiving & transmitting
			// When used with Matlab m4_motor_control, user is able 
			// to control M1 and M2 motors with the controller joystick
			 
			//Receive and display message if available
			//if(available(newSocket)==1){
			revLen=recv(newSocket, rev_buf, 10, 0);
			if(revLen>0){
				mWait(100000);
				//toggle red led when a message is received
				mRedTOGGLE;
				// For debugging purpose
				debug_write_bytes(rev_buf, revLen);
					
				for(int i=0; i<revLen; i++)
				{
					//ChangePWM((signed char)rev_buf[i]);
				}
				
				//echo received message, can be taken out if the CC3000 hangs
				//send(newSocket, rev_buf, revLen, 0);
				send(newSocket, buf_t_send, buf_t_send_len, 0);
				mWait(300000);
				
				//Refresh the buffer(zero out its values)				
				memset(rev_buf, 0, sizeof(rev_buf));
			}
			
		}while(!cc3000_is_socket_closed(newSocket));
		//Call function to reset the status of connection
		cc3000_socket_closed_acknowledge(newSocket);
		debug_write("Connection aborted");
		
		//close the socket connection
		closesocket(newSocket);
		
	}
}

//*****************************************************************************
//
//!  tcp_socket_client
//!
//!  @param        none
//!
//!  @return       none
//!
//!  @brief        This function is called if TCP socket communication is 
//!				   desired, in this case, the MCU would serve as a client and
//!				   requests connection to the server.
//!
//! @TODO		  This function needs some way to exit the connection and 
//!				  reconnect back to the server without having to reset the MCU
//!
//!	@Possible solution  fcntl function (but some constants are not defined in 
//!										header file)
//!						setsockopt function (but SOCKOPT_NONBLOCK is not 
//!											 defined in the header file)
//
//*****************************************************************************
void tcp_socket_client(){
	debug_write("TCP socket client Initialization");
	/*Variable declaration*/
	//Only one file descriptor is needed
	int fd_socket;
	//declare one server socket address and one client address (local information)
	sockaddr_in svr_addr;
	//Used to set socket options
	unsigned long inact_timeout=2;	//20 seconds (minimum value) before connect returns
	unsigned long dummy_var=0;		//dummy variable
	unsigned char recvtimeout=200; //0.2 sec
	//Initialize the file descriptor socket
	if((fd_socket=socket(AF_INET, SOCK_STREAM, 0))<0){
		debug_write("socket initialization failed");
		while(1);
	}
	
	/*assign fields for client socket address*/
	//Zero out all fields first
	memset((char *)&my_addr, 0, sizeof(my_addr));
	//AF_INET is used for IPv4
	my_addr.sin_family = AF_INET;
	//Listen to all IP addresses
	my_addr.sin_addr.s_addr = LISTEN_ADDR;
	//Use receive port number defined in the beginning
	my_addr.sin_port = htons(RECV_PORT_NUM);
	//Bind the file descriptor socket to the socket address
	if (bind(fd_socket, (sockaddr *)&my_addr, sizeof(my_addr)) < 0) {
		debug_write("bind failed");
		while(1);
	}
	
	/*assign fields for server socket address*/
	//Zero out all fields first
	memset((char *)&svr_addr, 0, sizeof(svr_addr));
	//AF_INET is used for IPv4
	svr_addr.sin_family	      = AF_INET;
	//Use transmit port number defined in the beginning
	svr_addr.sin_port		  = htons(SEND_PORT_NUM);
	//Connect to the predefined IP address (IP of your laptop or target)
	svr_addr.sin_addr.s_addr  = SEND_ADDR2;
	//inifinity time out specification for all but 20 seconds for inactivity timeout
	netapp_timeout_values(&dummy_var, &dummy_var, &dummy_var, &inact_timeout);
	
	
	//attempts to connect to the server until success 
	debug_write("Attempting to connect");
	if(connect(fd_socket,(sockaddr*) &svr_addr, sizeof(svr_addr))<0){
		debug_write("connect failed");
		debug_write("Attempting to connect");
	}
	//Set receive timeout value
	setsockopt(fd_socket, SOL_SOCKET, SOCKOPT_RECV_TIMEOUT, &recvtimeout, sizeof(recvtimeout));
	debug_write("Connection established");
	//Transmit to the server
    send(fd_socket, buf_t_send, buf_t_send_len, 0);
	mWait(100000);
	mGreenON;
	//Transmit and Receive in an infinite loop
	while(1){
		//Receive and display msg if available
		if(available(fd_socket)==1){
			revLen=recv(fd_socket, rev_buf, 10, 0);
			//echo received message
			send(fd_socket, rev_buf, revLen, 0);
			debug_write_bytes(rev_buf, revLen);
			mRedTOGGLE;
			
		}
		//"Heart-beat" of the program
		mBlueTOGGLE;
		mWait(100000);
	}
}

//*****************************************************************************
//
//!  udp_socket_communication
//!
//!  @param        none
//!
//!  @return       none
//!
//!  @brief        This function is called if UDP socket communication is 
//!				   desired.
//
//*****************************************************************************
void udp_socket_communication(){
	debug_write("UDP Socket Initialization");

	//Initialize the file descriptor socket for destination
	if((desSocket=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))<0){
		debug_write("Destination socket initialization failed");
	}
	/*assign fields for destination socket address*/
	//Zero out all fields first
	memset((char *)&des_addr, 0, sizeof(des_addr));
	des_addr.sin_family	  = AF_INET;
	des_addr.sin_port		  = htons(SEND_PORT_NUM);
	des_addr.sin_addr.s_addr = SEND_ADDR1;
		
	
	//Initialize the file descriptor socket for local CC3000
	if((revSocket=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))<0){
		debug_write("Receive socket initialization failed");
	}
	/*assign fields for local CC3000 socket address*/
	//Zero out all fields first
	memset((char *)&my_addr, 0, sizeof(my_addr));
	my_addr.sin_family	  = AF_INET;
	my_addr.sin_port		  = htons(RECV_PORT_NUM);
	my_addr.sin_addr.s_addr = LISTEN_ADDR;
	//Bind the local socket address to local socket
	bind(revSocket, (sockaddr*) &my_addr, sizeof(sockaddr));
	//inifinity time out specification
	netapp_timeout_values(0,0,0,0);
	
	debug_write("Communication begin");
	mGreenON;
	//Set receive timeout value
	if(!setsockopt(revSocket, SOL_SOCKET, SOCKOPT_RECV_TIMEOUT, &recvtimeout, sizeof(recvtimeout)))
	{
		debug_write("Successfully set receive timeout");
	}
	
	return;
}
//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//
//*****************************************************************************