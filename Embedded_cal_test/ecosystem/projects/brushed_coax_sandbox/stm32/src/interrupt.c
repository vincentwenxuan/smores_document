#include "common_peripherals.h"
#include STM32FXXX_EXTI_H
#include "interrupt.h"// custom file

double velocity;// stores the updated velocity 
int flag;// shows which time value is being read
double time[2];
double pi = 22.0/7.0;
double position;
double prev_time;
void EXTI1_IRQHandler(void) //ISR
{
 if(EXTI_GetITStatus(EXTI_Line1) != RESET)
  {
   // mGreenON;
    prev_time = GetMicroseconds();
    position = 0.0;
    time[flag] = GetMicroseconds();
    mRedON;
    DelayMicroseconds(1000);
    mRedOFF;
    if (flag == 0)
    {
      
      flag = 1;
      //mRedON;
    }
    else if (flag == 1)
    {
      //Calculate velocity 
      velocity = (2.0*pi)/(time[1] - time[0]); // in rad/us
      flag = 0;
      //mRedON;
    }
    
    /* Clear the EXTI line 0 pending bit */
    EXTI_ClearITPendingBit(EXTI_Line1);
  }
return;
}