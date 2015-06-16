// common components
#include "common_peripherals.h"
#include "common_message_types.h" // common messages
#include "usb_interface.h"        // usb interface
// local libraries
#include "encoder.h" 
#include "system_clock.h"
#include "interrupt.h" // <My custom header for interruptss>
// #include "stm32f30x_syscfg.h"
#include STM32FXXX_GPIO_H
#include STM32FXXX_MISC_H
#include STM32FXXX_RCC_H
#include STM32FXXX_TIM_H
#include STM32FXXX_SYSCFG_H
#include STM32FXXX_EXTI_H
#include "sync_timer.h"
#include "arm_math.h"

// common libraries
#include "project_message_types.h"
#include "error_reporting.h"


#define ENCODER_INDEX_PIN 1 // EXTI1
#define EXTI_line EXTI_Line1  // look in the stm exti.h file to know more
#define ENCODER_INTERRUPT EXTI1_IRQn


void InitInterrupt(void)
{
  // RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  
  /* Enable SYSCFG clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  
  // initialise the hall_sensor input pin (PB1):
  GPIO_InitTypeDef hall_input_init;
  hall_input_init.GPIO_Pin = GPIO_Pin_1;
  hall_input_init.GPIO_Mode = GPIO_Mode_IN;
  hall_input_init.GPIO_Speed = GPIO_Speed_50MHz;
  hall_input_init.GPIO_PuPd = GPIO_PuPd_NOPULL; //Pulled -up by default, assuming the presence of magnet pulls the line low
  GPIO_Init(GPIOB,&hall_input_init);

  // Select the input source pin for the EXTI line using SYSCFG_EXTILineConfig().
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB,EXTI_PinSource1); //Pin PB1

   //Interrupt Setup:
  EXTI_InitTypeDef   EXTI_InitStructure;
  /* Configure EXTI0 line */
 // GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource1);
  EXTI_InitStructure.EXTI_Line = EXTI_Line1;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  EXTI_ClearITPendingBit(EXTI_Line1);
 
  // Configure NVIC IRQ channel mapped to the EXTI line using NVIC_Init().
  //Definition in ecosystem/stm32f30x/stm32f30x_misc.h
  NVIC_InitTypeDef   NVIC_InitStructure;
  /* Enable and set EXTI0 Interrupt to the highest priority */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
    
 
}

////////////////////////////////////////////////////////////////////////////////
// Entry Point

int main(void) {

  double position ;
  double pi = 22.0/7.0;

  InitPeripherals();
  InitInterrupt(); // Local Function that initialises the EXTI - PB1 interrupt
  flag = 0;
  int interrupt_cnt = 0;
  UsbInterface usb = UsbInterface();
  usb.Init();

  mRedOFF; mGreenOFF; mBlueOFF;

  // startup blink
  const float kShortDelay = 0.1;
  const float kLongDelay = 0.3;
  mRedON; mGreenON; mBlueON;
  DelaySeconds(kShortDelay);
  mRedOFF; mGreenOFF; mBlueOFF;
  DelaySeconds(kLongDelay);
  mRedON; mGreenON; mBlueON;
  DelaySeconds(kShortDelay);
  mRedOFF; mGreenOFF; mBlueOFF;
  DelaySeconds(kLongDelay);
  mRedON; mGreenON; mBlueON;
  DelaySeconds(kShortDelay);
  mRedOFF; mGreenOFF; mBlueOFF;
  DelaySeconds(kLongDelay);
  
    //  angular_velocity = (2*pi)/(time[1]-time[0]); // in rad/us
    //Start dumping predicted position based on velocity at t =0:
   // rotor_position = rotor_position + (angular_velocity * (GetMicroseconds()-time[1]) * (1.0e-6)); // in radians

  
//Get initial velocity measurement
/*  while(time_count<=1) 
 { mRedON;
   hall_in = GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1);
   if(hall_in == 0)
   { mGreenON;
     time[time_count] = GetMicroseconds();
     time_count++;
     while (!GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1)); // wait until the sensor passes over
     mGreenOFF;
    }
   mRedOFF;
 }
*/ 
  usb.printf("Initialised ");

  double strobe_position = 0.3;

  //////////////////////////////////////////////////////////////////////////////
  // Main Loop	
  while(1) { 
  	 double current_t, predicted_dt ;
// 	struct Time t = GetTime();

//  	float dt = GetSeconds(SubtractTime(t2,t1))
/*    if (predicted_dt==GetMicroseconds())
    {
    	mGreenON;
    	DelayMicroseconds(100);
    }
*/
    //mRedOFF;
   // mGreenOFF;

   // DelaySeconds(1.0);
    // mGreenON;
    double t = GetMicroseconds();
    double rep ;
   
    //position = position + (t - prev_time)* velocity; //Position in radians
    position =   (t - prev_time)* velocity; //Position in radians
   // usb.printf("unsaturated position is %f.\r\n  ", position);
   
   // saturate position here:
    if (position > pi)
    { rep = (int)(position / pi); 
    
        if (fmod(rep,2.0) == 0.0) // if even number of pi is in the angle
      {// Then angle is in the first 2 quads
        position = fmod(position , pi) ; //subtract all pi angles from the position  to get an angle between zero an pi 
      }
       else if(fmod(rep,2.0) != 0.0)// odd number of pi
       {// Then angle is in the last 2 quads
       	position =  - pi + fmod(position,pi) ;
       }
      
    }
    
    current_t = GetMicroseconds();
    predicted_dt =  current_t + (strobe_position-position) * velocity;

    /*
    usb.printf ("difference %d \n ",(int)(position*10.0) - (int)(strobe_postion*10.0));
    usb.SendNow();
    if ((int)(position*10.0) == (int)(strobe_postion*10.0))
    {
    	mRedON;
    	mGreenON;
    	DelayMicroseconds(10000);
    }*/
   
  //  usb.printf("position is %+f.\r\n  ", position);
   // usb.printf("velocity is %f.\r\n  ", 1.0e06*velocity);
    // usb.printf("time array is %f %f.\n", time[0],time[1]);
    // usb.printf("velocity is %f\n",angular_velocity);
       // usb.printf("position is %f\n",rotor_position);
 // usb.SendNow();
 //   mRedOFF;
    //DelaySeconds(0.05);
    
  //  DelayMicroseconds(10000);
    // mGreenOFF;
    // 
    
    
  } // while(1)
  return(0);
}
