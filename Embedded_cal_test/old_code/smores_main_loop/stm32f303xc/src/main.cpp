#include "common_peripherals.h"
#include "system_clock.h"
// common components 
#include "common_message_types.h" 
// common messages
#include "usb_interface.h"        
// usb interface
#include "mBus.h"

// common libraries
#include "project_peripherals_smores.h"
#include "project_message_types.h"
#include "stm32f3_discovery.h"
#include <string.h> 
#include "DC_Motor.h"
//needed for memset
#define   mBUS 1 
void GPIO_init(); 
void setupDiscovery(); 


int main (void){
  InitPeripherals();
  setupDiscovery(); 
  Motor motor_1;
  motor_1.GPIO_Direction_GPIOx = GPIOB;
  motor_1.GPIO_Direction_Pin = GPIO_Pin_2;
  motor_1.GPIO_Direction_RCC = RCC_AHBPeriph_GPIOB;
  motor_1.GPIO_PWM_GPIOx = GPIOA;
  motor_1.GPIO_PWM_Pin = GPIO_Pin_8;
  motor_1.GPIO_PWM_PinSource = GPIO_PinSource8;
  motor_1.GPIO_PWM_AF = GPIO_AF_6;
  motor_1.GPIO_PWM_RCC = RCC_AHBPeriph_GPIOA;
  motor_1.TIMx = TIM1;
  motor_1.TIMx_Channel = TIM_Channel_1;
  motor_1.TIMx_RCC = RCC_APB2Periph_TIM1;
  motor_1.TIMx_Mode = TIM_OCMode_PWM1;
  motor_1.direction = Bit_RESET;
  motor_1.speed = 0;
  
  Motor_Init(motor_1);
    
    UsbInterface usb = UsbInterface();
    usb.Init();

    
   // mYellowON; mRedON; mGreenON; mWhiteON; mBlueON;
    STM_EVAL_LEDOn(LED4);
    STM_EVAL_LEDOn(LED5);
	DelayMilliseconds(1000);
    STM_EVAL_LEDOff(LED4);
    STM_EVAL_LEDOff(LED5);
    //mYellowOFF;mRedOFF;mGreenOFF; mWhiteOFF; mBlueOFF;


  //////////////////////////////////////////////////////////////////////////////
  // Main Loop
    while(1) {
        STM_EVAL_LEDOn(LED6); 
        DelayMilliseconds(500); 
        set_motor_direction(motor_1, Bit_RESET);
        set_motor_speed(motor_1, 20);
        STM_EVAL_LEDOn(LED7);
        uint8_t *rx_data;
        uint8_t rx_length;
        uint8_t is_data  = 0;
        if(usb.GetBytes()){
            STM_EVAL_LEDOn(LED10);
            STM_EVAL_LEDOn(LED3);
        
            while(usb.PeekPacket(&rx_data, &rx_length))
             {
                 STM_EVAL_LEDOn(LED8);       
                 uint8_t type = rx_data[0];
                 if(type ==  msg_muscle_state ) 
                 {
                      STM_EVAL_LEDOn(LED9);            
                 }
                 usb.DropPacket();
                 is_data =1;  
             }
             if(is_data)
             {
                usb.SendNow();
             }
     }
             STM_EVAL_LEDOff(LED8); 
             STM_EVAL_LEDOff(LED9); 
            
    } // while(1)
    return(0);
}
void GPIO_init()
{
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_5;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_2;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOB, &GPIO_InitStruct);
}
void setupDiscovery () { 
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
    GPIO_init();   
    u32 state = 0;
    STM_EVAL_LEDInit(LED3);
    STM_EVAL_LEDInit(LED4);
    STM_EVAL_LEDInit(LED5);
    STM_EVAL_LEDInit(LED6);
    STM_EVAL_LEDInit(LED7);
    STM_EVAL_LEDInit(LED8);
    STM_EVAL_LEDInit(LED9);
    STM_EVAL_LEDInit(LED10);
    STM_EVAL_PBInit(BUTTON_USER, BUTTON_MODE_GPIO);
}