#ifndef MESSAGE_TYPES_H
#define MESSAGE_TYPES_H

#include <stdint.h>

   // Test struct
const uint8_t test_msg_type = 0x1A;
 typedef struct  __attribute__ ((__packed__)){
    uint8_t motor_0_vel;
    uint8_t motor_1_vel;
    uint8_t motor_0_direction;
    float motor_1_direction;
  }Motormsg;

  // Test struct2
 struct Motormsg2{
    uint32_t type;
    uint8_t motor_0_vel;
    uint8_t motor_1_vel;
    uint8_t motor_0_direction;
    float motor_1_direction;
  } __attribute__ ((__packed__)) init2 = {11};// Initialization not working

#endif // MESSAGE_TYPES_H
