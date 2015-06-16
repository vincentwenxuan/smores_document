// THIS FILE IS NOT MAINTAINED.  LOOK ELSEWHERE FOR EXAMPLE MAIN.

/** 
** This template will guide you in making a ST M4 project.  
** All locations with double comments require your attention (** or ///)
** Please perform the following tasks:
**
** Edit project_peripherals.h to enable the desired peripherals
** Include custom headers
** Write your code in the specified areas
** Delete this comment and put a description of your project and the used hardware
**/

#include "common_peripherals.h"

/// Include your custom headers here

/// Initialize variables here

// main function
int main(void)
{
  InitPeripherals();  // configure GPIO, USB, I2C, ect based on peripherals.h
  
  /// Initialize code here
  
  // main loop
  while(1)
  {
    
  }
  
  return(0);
}