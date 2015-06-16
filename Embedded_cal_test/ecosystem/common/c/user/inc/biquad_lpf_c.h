// A second order low pass filter instance with fixed corner frequency and 
// sampling rate. The filter strives for a flat passband, -3 db attenuation at 
// the corner frequency, and a -40 db/decade rolloff to high frequency 
// (Butterworth response).
//
// The filter will not attempt to achieve a corner frequency higher than 1/2 
// the sampling rate.  Numerical results are good for corner frequencies lower 
// than 1/4 the sampling rate.

#ifndef BIQUAD_LPF_C_H
#define BIQUAD_LPF_C_H

#ifdef __cplusplus
extern "C" {
#endif

/// BiquadLpfC instance state struct, voluntarily opaque.
struct BiquadLpfC {
    float a1;
    float a2;
    float b0;
    float b1;
    float x1;
    float x2;
    float y1;
    float y2;
};

void BiquadLpfCInit(volatile struct BiquadLpfC* filt, float f_corner, float f_sample);
void BiquadLPFCSetup(volatile struct BiquadLpfC* filt, float f_corner, float f_sample);
float BiquadLpfCUpdate(volatile struct BiquadLpfC* filt, float x);
float BiquadLpfCGet(volatile struct BiquadLpfC* filt);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // BIQUAD_LPF_DYNAMIC_C_H