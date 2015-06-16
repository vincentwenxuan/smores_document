#include "common_peripherals.h"
#include "system_clock.h"
// common components 
#include "common_message_types.h" 
// common messages
#include "usb_interface.h"        
// usb interface
#include "mBus.h"

// common libraries
#include "project_message_types.h"
#include "error_reporting.h"
#include <string.h> 
#include "stm32_cc3000.h"
#include "CC3000_socket.h"
#include "stm32_cc3000platform_config.h"
//needed for memset
#define   mBUS 1 
void wifiSetup(int ip_addr); 

typedef enum
{
    RX_PREABLE0, RX_PREABLE1, RX_ID
}wifi_state; 

const char *ssid = WLAN_SSID; 
const char *pass_key = WLAN_PASS; 
char cc3000_device_name[] = "CC3000"; 

int main (void){
	InitPeripherals();
  int ip_addr = 10; // setup for HOME-AE52 
    // usb interface setup 
  UsbInterface usb = UsbInterface();
  usb.Init();
  // cc3000Wifi wifi = cc3000Wifi(usb); 

    
//     struct Time currentTime;
    
    mGreenON; mWhiteON; mBlueON;
    wifiSetup(ip_addr); 
    DelayMilliseconds(3000);
    mGreenOFF; mBlueOFF;mWhiteOFF;
//     mYellowOFF;mRedOFF;mGreenOFF; mWhiteOFF; mBlueOFF;
    int connection_establish =  cc3000_connect_secure(ssid,pass_key,1);
    usb.printf("Error Code: %d\n", connection_establish); 
    if(connection_establish == 0)
    {
      mWhiteON;mGreenON;mYellowOFF; 
      usb.printf("Successfully Connected \n");
    }
  //////////////////////////////////////////////////////////////////////////////
  // Main Loop
    while(1) {
        
       // struct Time t = GetTime();
        
        if (connection_establish == 0 )
          { 
            mWhiteON; 
            mYellowOFF; 
          }
        else {
        mRedON;  
        mWhiteOFF;
        }
        usb.printf("can I print ? \n");
        usb.printf("%d\n",connection_establish);  
        DelayMilliseconds(1000); 

        //revLen = recvfrom(revSocket, rev_buf, 10, 0, (sockaddr*) &source_addr, &revPacketLen);
       // revLen = recvfrom(revSocket,rev_buf,25, 0, (sockaddr*) &source_addr, &revPacketLen); 

        /*if(revLen>0){
            //debug_write_u32(source_addr.sin_addr.s_addr, 16);
            usb.printf("buffer contains: %d ",rev_buf); 
            debug_write_bytes(rev_buf, revLen);
            mRedTOGGLE;         
        }
        */
        
       // currentTime = t;
       // usb.printf("tsec = %d\n\r", t.sec);
        // usb.printf("tnsec = %d\n\r", t.nsec);
        // usb.printf("ctsec = %d\n\r", currentTime.sec);
        // usb.printf("ctnsec = %d\n\r", currentTime.nsec);
      //  usb.printf("hi");
        //usb.SendNow();
    


 
    } // while(1)
    return 0; 
}
// 1 successful write to wifi, 0 Error writing to wifi 
void wifiSetup(int ip_addr){
 //   char cc3000_device_name[] = "CC3000";
    // debug_setup();
    cc3000_initialize_ip(ip_addr); 
    cc3000_startup_check();
    udp_socket_communication();
}


