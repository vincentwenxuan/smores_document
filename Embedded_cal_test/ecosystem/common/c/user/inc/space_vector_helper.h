// Approximates sin(x)/sin(x + pi/3) as a 5th order polynomial on the 
// domain [0, pi/3].  A helpful formula for space vector control.
//
// The max error on the domain [0,pi/3] is 1.3e-5.  
// For comparison, the max floating point quantization on this interval 
// is 1.2e-7, which puts a lower bound on the accuracy of any "exact" solution.
//
// Coefficients determined in Sollya using the following script:
//    f = sin(x)/sin(x + pi/3);
//    domain = [1e-12; pi/3];
//    order = [|1,2,3,4,5|];
//    P = fpminimax(f, order, [|SG...|], domain, absolute, floating);
//    print(horner(P));

#ifndef SPACE_VECTOR_HELPER_H
#define SPACE_VECTOR_HELPER_H

#define SV_POLY_5_C1  1.153952717781066894f
#define SV_POLY_5_C2 -0.652400970458984375f
#define SV_POLY_5_C3  0.677115440368652343f
#define SV_POLY_5_C4 -0.374978035688400268f
#define SV_POLY_5_C5  0.143231034278869628f

#ifdef __cplusplus
extern "C" {
#endif

inline float SpaceVectorHelper(float x) {
  return  x*(SV_POLY_5_C1
            + x*(SV_POLY_5_C2
              + x*(SV_POLY_5_C3
                + x*(SV_POLY_5_C4
                  + x*(SV_POLY_5_C5)))));
}

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // SPACE_VECTOR_HELPER_H