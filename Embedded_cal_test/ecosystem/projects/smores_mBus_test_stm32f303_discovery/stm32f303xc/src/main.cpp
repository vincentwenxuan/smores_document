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
#include "stm32f3_discovery.h"

#define M_BLUE_TOGGLE 0xA0
#define GET_ADC_DATA 0xA1
#define M_GREEN_TOGGLE 0xA2
#define SEND_SLAVE_DATA 0xA4

int main (void){
    InitPeripherals();
    
    PlayTimebase();
    
    UsbInterface usb = UsbInterface();
    usb.Init();
    
    STM_EVAL_LEDInit(LED3);
    STM_EVAL_LEDInit(LED4);
    STM_EVAL_LEDInit(LED5);
    STM_EVAL_LEDInit(LED6);
    STM_EVAL_LEDInit(LED7);
    STM_EVAL_LEDInit(LED8);
    STM_EVAL_LEDInit(LED9);
    STM_EVAL_LEDInit(LED10);

	char slave_addr_read = 0x14;
	char slave_addr_write = 0x15;
	char cmd;
	uint8_t data[7] = {0,0,0,0,0,0,0};
	uint8_t send_data[4] = {110, 120, 119, 255};

	struct Time t;

//    int flag = 100;
    while(1)
    {
        STM_EVAL_LEDToggle(LED10);
		cmd = SEND_SLAVE_DATA;
		mBusWriteBurst(slave_addr_write, cmd, 4, send_data, 1);
		//DelayMilliseconds(10);
		cmd = GET_ADC_DATA;
		//		while( mBusReadBurstStartNB(slave_addr, cmd, 6, 1) == ERROR );
		// mBusReadBurstDataNB(6, data, 1);
		// while( mBusMasterReadBurstStart(slave_addr, cmd, 6, 1) == ERROR );
		mBusReadBurst(slave_addr_read, cmd, 6, data, 1);

		t = GetTime();
		usb.printf("%d ", data[0]);
		usb.printf("%d ", data[1]);
		usb.printf("%d ", data[2]);
		usb.printf("%d ", data[3]);
		usb.printf("%d ", data[4]);
		usb.printf("%d\n\r", data[5]);
		usb.SendNow();
		DelayMilliseconds(10);
    }
    return(0);
}

void timerCallback1(void)
{
	
}
