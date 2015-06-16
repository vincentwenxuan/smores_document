/**
  *****************************************************************************************
  * @file    main.c 
  * @author  
  * @version V1.1.0
  * @date    11-June-2014
  * @brief   This source file is the main source code to interface with the CC3000 WiFi 
  *		   	 module, the sequence in which the code is executed can be simplified as 
  *			 follows:
  *			 1. Initializes and configures the neccessary registers to interface with the
  *				CC3000 SPI peripheral interface.
  *			 2. Requests for the static IP address.
  *			 3. Searches and connects to the SSID of the wireless network.
  *			 4. Initializes the UDP sockets for transmission and reception of data.
  *			 5. Transmits and receives data in an infinite loop. Note that the blue LED 
  *				toggles to verify that normal operation of the program, and the red LED
  *				also toggles when a new message has been received.
  *
  *****************************************************************************************
/* Includes ------------------------------------------------------------------*/
#include "mGeneral.h"
#include "stm32_cc3000.h"
#include "CC3000_socket.h"
//#include "tim_pwm_out.h"


/*Declare global variables*/
char cc3000_device_name[] = "CC3000";

// int send_available(int16_t);

/* Main function starts here*/
int main(void) 
{ 
    //Functions to set up the neccessary registers for GPIO, UART and SPI peripherals and motors
    mInit();
    mBlueON;
    debug_setup();
    //Enter Module number to give it corresponding static IP
    //ie 192.168.10.(110+num)
    cc3000_initialize_ip(100); 
    cc3000_startup_check();
	//Initialization for motors
    // motor_dir_init();
    // TIMA_config(100, 20,10); // timer1 channel 1 PA8
    // TIMB_config(100, 20,10); // timer2 channel 3 PA9
    // TIMC_config(100, 20,10); // timer15 channel 1 PB14
    // TIMD_config(100, 20,10); // timer2 channel 2 PA1 // first 2 numbers
                           // in this function are not doing anything  
							
  /* Uncomment the function to use the desired socket for communication
   * ONLY ONE of the following functions should be uncommented
   */
	udp_socket_communication(); //UDP socket communication
//  tcp_socket_server();  //TCP socket communication as a server
//  tcp_socket_client();	//TCP socket communication as a client

	//Transmit and Receive in an infinite loop
	while(1){
		//Transmit data
		sendto(desSocket, buf_t_send, buf_t_send_len, 0, (sockaddr*) &des_addr, sizeof(des_addr));
		mWaitms(2);	//Delay is needed between send and receive functions
		//Receive data
		revLen = recvfrom(revSocket, rev_buf, 10, 0, (sockaddr*) &source_addr, &revPacketLen);
		if(revLen>0){
			//debug_write_u32(source_addr.sin_addr.s_addr, 16);

			debug_write_bytes(rev_buf, revLen);
			//motor_test(rev_buf[0]);
			mRedTOGGLE;			
		}
		
		//Available function seems to cause lots of overhead, avoid using it if speed is desired
		/*
		if(available(revSocket)==1){
			revLen = recvfrom(revSocket, rev_buf, 10, 0, (sockaddr*) &source_addr, &revPacketLen);
			//debug_write_bytes(rev_buf, revLen);
			motor_test(rev_buf[0]);
			mRedTOGGLE;				
		}
		*/
		
		//Heartbeat
		mBlueTOGGLE;
		mWaitms(200); //Delay is needed between send and receive functions
	}
}





