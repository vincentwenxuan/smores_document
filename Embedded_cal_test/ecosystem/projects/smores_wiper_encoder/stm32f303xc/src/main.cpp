#include "common_peripherals.h"
#include "system_clock.h"
#include "mBus.h"
// common components 
#include "common_message_types.h" 
// common messages
#include "usb_interface.h"        
// usb interface
//#include "mBus.h"

// common libraries
#include "project_message_types.h"
#include "error_reporting.h"
#include <string.h> 
//needed for memset
//#define   mBUS 1

// user libraries
#include "Wheel_Control_naive.h"

/*   face i2c address macros:  */
#define BOTTOM_FACE_I2C_ADDRESS 0x0B
#define LEFT_FACE_I2C_ADDRESS   0x0A
#define RIGHT_FACE_I2C_ADDRESS  0x0C
#define TOP_FACE_I2C_ADDRESS    0x0D

#define M_BLUE_TOGGLE 0xA0
#define GET_ADC_DATA 0xA1
#define M_YELLOW_TOGGLE 0xA2

int main (void){
    InitPeripherals();
    
    UsbInterface usb = UsbInterface();
    usb.Init();

    init_Dof();
	
    char slave_addr = (TOP_FACE_I2C_ADDRESS << 1);
    char cmd;
    uint8_t data[7] = {0,0,0,0,0,0,0};

    struct Time t;

    pan_control(1, 50);
    while(1)
    {
        mGreenTOGGLE;
        cmd = GET_ADC_DATA;
        mBusReadBurst(slave_addr, cmd, 7, data, 1);
        
        t = GetTime();
        if (data[6] == 0xAA)
        {
            // for matlab communication
            usb.printf("%d ", t.sec);
            usb.printf("%d ", t.nsec);
            usb.printf("%d ", data[2] + data[3] * 255);
            usb.printf("%d ", data[4] + data[5] * 255);
            usb.printf("%d\n\r", data[6]);
        }

        // for terminal debug
        // usb.printf("tsec: %d\n\r", t.sec);
        // usb.printf("tnsec: %d\n\r", t.nsec);
        // usb.printf("adc0: %d\n\r", data[2] + data[3] * 255);
        // usb.printf("adc1: %d\n\r", data[4] + data[5] * 255);
        // //usb.printf("encoder1 = %d\n\r", counter1);
        // usb.printf("encoder2: %d\n\r", counter2);
		
        usb.SendNow();
        DelayMilliseconds(10);
    }
    return(0);
}

void timerCallback1(void)
{
    
}
