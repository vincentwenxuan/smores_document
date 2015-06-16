// A second order low pass filter instance with fixed corner frequency.  The 
// sampling rate is recomputed based on the time of each sample, so if a fixed 
// sampling rate is known a faster implementation is possible.  The filter 
// strives for a flat passband, -3 db attenuation at the corner frequency, and 
// a -40 db/decade rolloff to high frequency (Butterworth response).
//
// The filter will not attempt to achieve a corner frequency higher than 1/2 
// the sampling rate.  Numerical results are good for corner frequencies lower 
// than 1/4 the sampling rate.

#ifndef BIQUAD_LPF_DYNAMIC_C_H
#define BIQUAD_LPF_DYNAMIC_C_H

#ifdef __cplusplus
extern "C" {
#endif

/// BiquadLpfDynamicC instance state struct, voluntarily opaque.
struct BiquadLpfDynamicC {
    float f0;
    float x1;
    float x2;
    float y1;
    float y2;
};

// initialize filter with a target corner frequency
void BiquadLpfDynamicCInit(volatile struct BiquadLpfDynamicC* filt, float f0);

// update with a measurement and time dt in seconds since the last measurement
float BiquadLpfDynamicCUpdate(volatile struct BiquadLpfDynamicC* filt, float x, float dt);

// retrieve output value
float BiquadLpfDynamicCGet(volatile struct BiquadLpfDynamicC* filt);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // BIQUAD_LPF_DYNAMIC_C_H