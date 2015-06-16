// common functional components
#include "usb_interface.h"      // usb interface
#include "biquad_lpf_dynamic_c.h"

// general ecosystem libraries
#include "project_message_types.h" // ecosystem-wide message definitions
#include "common_peripherals.h"   // hardware configuration

#include "arm_math.h"

int main(void)
{
  InitPeripherals();
  
  UsbInterface usb = UsbInterface();
  usb.Init();
  
  

  //////////////////////////////////////////////////////////////////////////////
  // Main Loop
  while(1) {
    
    // USB Input
    usb.GetBytes();
    uint8_t *rx_data;  // temporary pointer to received type+data bytes
    uint8_t rx_length; // number of received type+data bytes
    while(usb.PeekPacket(&rx_data, &rx_length)) {
    
      uint8_t type = rx_data[0];
      if(type == kTypeBodeRequest) {
        mWhiteON;
        MsgBodeRequest *bode = (MsgBodeRequest*) &rx_data[1];

        struct BiquadLpfDynamicC filter;
        BiquadLpfDynamicCInit(&filter,  bode->corner_f);
        
        // exhaust transient
        float trans_time = 100*(1/bode->test_f);
        float dt = 1/bode->sample_f;
        float t = 0;
        while(t < trans_time) {
          BiquadLpfDynamicCUpdate(&filter, bode->test_a*arm_sin_f32(2*PI*bode->test_f*t), dt);
          t = t + dt;
        }

        // determine output amplitude
        float meas_time = 100*(1/bode->test_f);
        float y_max = 0;
        while(t < trans_time + meas_time) {
          float y = BiquadLpfDynamicCUpdate(&filter, bode->test_a*arm_sin_f32(2*PI*bode->test_f*t), dt);
          if(y > y_max) {
            y_max = y;
          }
          t = t + dt;
        }
         
        MsgBodeResult result;
        result.gain = y_max/bode->test_a;
        usb.SendPacket(kTypeBodeResult, (uint8_t*)&result, sizeof(MsgBodeResult));
        mWhiteOFF;
      }
        
      usb.DropPacket();
    } // while(Peek...

    usb.SendNow();
  } // while(1)
  
  return(0);
}