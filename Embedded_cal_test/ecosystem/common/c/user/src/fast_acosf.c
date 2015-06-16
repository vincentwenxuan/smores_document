#include "fast_acosf.h"
#include "stdint.h"

#include "math.h"


// constants
#define PI                3.14159265358979323f
#define PIBY2             1.5707963267948966f
#define INV_SQRT_2        7.071067812e-01f
#define INV_DX            43.8406204336f        // ACOSF_TABLE_SIZE / INV_SQRT_2
#define ACOSF_TABLE_SIZE  32



// function values
float p_table[ACOSF_TABLE_SIZE] =
{
  1.570796327e+00f, 1.547984452e+00f, 1.525160696e+00f, 1.502313120e+00f, 
  1.479429676e+00f, 1.456498142e+00f, 1.433506068e+00f, 1.410440708e+00f, 
  1.387288956e+00f, 1.364037272e+00f, 1.340671602e+00f, 1.317177293e+00f, 
  1.293538997e+00f, 1.269740564e+00f, 1.245764922e+00f, 1.221593940e+00f, 
  1.197208272e+00f, 1.172587179e+00f, 1.147708318e+00f, 1.122547502e+00f, 
  1.097078411e+00f, 1.071272253e+00f, 1.045097359e+00f, 1.018518691e+00f, 
  9.914972478e-01f, 9.639893316e-01f, 9.359456395e-01f, 9.073101205e-01f, 
  8.780185244e-01f, 8.479965290e-01f, 8.171572880e-01f, 7.853981634e-01f, 
};

// function derivatives, multiplied by (INV_SQRT_2 / ACOSF_TABLE_SIZE)
float m_table[ACOSF_TABLE_SIZE] =
{
  -2.280989617e-02f, -2.281583238e-02f, -2.283366886e-02f, -2.286348952e-02f, 
  -2.290543542e-02f, -2.295970663e-02f, -2.302656495e-02f, -2.310633744e-02f, 
  -2.319942095e-02f, -2.330628784e-02f, -2.342749283e-02f, -2.356368148e-02f, 
  -2.371560027e-02f, -2.388410886e-02f, -2.407019476e-02f, -2.427499107e-02f, 
  -2.449979788e-02f, -2.474610821e-02f, -2.501563966e-02f, -2.531037318e-02f, 
  -2.563260079e-02f, -2.598498502e-02f, -2.637063314e-02f, -2.679319111e-02f, 
  -2.725696334e-02f, -2.776706724e-02f, -2.832963498e-02f, -2.895208054e-02f, 
  -2.964345834e-02f, -3.041495323e-02f, -3.128056235e-02f, -3.225806452e-02f, 
};

float fast_acosf(float x) {
  
  uint8_t positive_arg; // original argument was positive
  uint8_t small_arg;    // original argument was small
  
  // adjust for negative inputs
  if(x >= 0) {
    positive_arg = 1;
  }
  else {
    positive_arg = 0;
    x = -x;
  }
  
  // adjust for small inputs
  if(x <= INV_SQRT_2) {
    small_arg = 1;
  }
  else {
    small_arg = 0;
    x = sqrtf(1-x*x);
  }

  // use table
  // precondition: x >= 0
  // precondition: x <= 1/sqrt(2)
  // find interval and interval fraction
  float t = x * INV_DX;
  uint8_t index = (uint8_t)t;
  t = t - index;
  
  float t2 = t * t;
  float t3 = t2 * t;
  
  x = (2*t3 - 3*t2 + 1) * p_table[index]
      + (t3 - 2*t2 + t) * m_table[index]
      + (-2*t3 + 3*t2) * p_table[index+1]
      + (t3 - t2) * m_table[index+1];
  
  if(!small_arg)
    x = PIBY2 - x;
  if(!positive_arg)
    x = PI - x;

  return(x);
}
