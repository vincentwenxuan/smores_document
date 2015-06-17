#include "biquad_lpf_dynamic_c.h"
#include "arm_math.h"

#define ONE_BY_SQRT2  0.70710678118f
#define TWO_PI        6.28318530718f

void BiquadLpfDynamicCInit(volatile struct BiquadLpfDynamicC* filt, float f0) {
  filt->f0 = f0;
  filt->x1 = 0;
  filt->x2 = 0;
  filt->y1 = 0;
  filt->y2 = 0;
}

float BiquadLpfDynamicCUpdate(volatile struct BiquadLpfDynamicC* filt, float x, float dt) {
  float y = 0;
  if(dt != 0) {
  
    float w0 = TWO_PI * filt->f0 * dt; // normalized frequency
    if(w0 > PI) {
      w0 = PI;
    }
    float alpha = arm_sin_f32(w0) * ONE_BY_SQRT2;
    float cosw0 = arm_cos_f32(w0);
    
    // Cannonical
    // float a0 = 1 + alpha;
    // float a1 = -2*cosw0;
    // float a2 = 1 - alpha;
    // float b0 = (1-cosw0)*0.5f;
    // float b1 = (1-cosw0);
    // float b2 = (1-cosw0)*0.5f;
    // float inv_a0 = 1/a0;
    // y0 = (b0*inv_a0)*x0 + (b1*inv_a0)*x1_ + (b2*inv_a0)*x2_ - (a1*inv_a0)*y1_ - (a2*inv_a0)*y2_;

    float inv_a0 = 1/(1 + alpha);
    float a1 = -2*cosw0     * inv_a0;
    float a2 = (1 - alpha)  * inv_a0;
    float b1 = (1-cosw0)    * inv_a0;
    float b0 = b1*0.5f;   // b0 == b2
    y = b0*(x+filt->x2) + b1*filt->x1 - a1*filt->y1 - a2*filt->y2;
  }
  else {
    y = x;
  }
  
  filt->x2 = filt->x1;
  filt->x1 = x;
  filt->y2 = filt->y1;
  filt->y1 = y;

  return filt->y1;
}

float BiquadLpfDynamicCGet(volatile struct BiquadLpfDynamicC* filt) {
  return filt->y1;
}