#ifndef MATH_HELPER_H
#define MATH_HELPER_H

#include <stdint.h>
#include <string.h> // for memcpy

#ifndef PI
  #define PI 3.14159265358979f
#endif

const float kPi     = 3.141592653589793238462643383279502884f;
const float k2Pi    = 6.283185307179586476925286766559005768f;
const float kPiBy2  = 1.570796326794896619231321691639751442f;
const float kPiBy3  = 1.047197551196597746154214461093167628f;

////////////////////////////////////////////////////////////////////////////////
/// Input Checking

/// Wrap a float value in radians to between -PI and PI inclusive.
static inline float WrapRad(float rad) {
  while(rad > kPi) {
    rad = rad - k2Pi;
  };
  while(rad < -kPi) {
    rad = rad + k2Pi;
  };
  return rad;
}

/// Wrap a float value, in, between max and min inclusive.  Max must be greater than min.
static inline float WrapFloat(float in, float max, float min) {
  float step = max - min;
  while(in > max){
    in = in-step;
  };
  while(in < min){
    in = in + step;
  };
  return in;
}

/// Saturate a float value 'in' to between 'lb' and 'ub', inclusive.
static inline float SatFloat(float in, float lb, float ub) {
  if(in < lb)
    return lb;
  if(in > ub)
    return ub;
  return in;
}

/// Undo a deadband with a range of +- db.  Only shifts, does not scale.
static inline float LivebandFloat(float in, float db) {
  if(in < 0.0f)
    return in - db;
  if(in > 0.0f)
    return in+db;
  return 0.0f;
}

////////////////////////////////////////////////////////////////////////////////
/// Type Punning

/// Dereference pointer to uint32_t as a floating point value
static inline float PunToFloat(const uint32_t* in) {
  float out;
  memcpy(&out, in, 4);
  return out;
}

/// Dereference pointer to 4 uint8_t as a floating point value
static inline float PunToFloat(const uint8_t* in) {
  float out;
  memcpy(&out, in, 4);
  return out;
}

/// Dereference pointer to float as a uint32_t value
static inline uint32_t PunToUint32(const float* in) {
  uint32_t out;
  memcpy(&out, in, 4);
  return out;
}

/// Pun uint32_t as a float value
static inline float PunToFloat(const uint32_t in) {
  float out;
  memcpy(&out, &in, 4);
  return out;
}

/// Pun float as a uint32_t value
static inline uint32_t PunToUint32(const float in) {
  uint32_t out;
  memcpy(&out, &in, 4);
  return out;
}

/// Pun int32 as a uint32_t value
static inline uint32_t PunToUint32(int32_t in) {
  uint32_t out;
  memcpy(&out, &in, 4);
  return out;
}

/// Pun int16 as a uint32_t value
static inline uint32_t PunToUint32(int16_t in) {
  int32_t val = in;
  uint32_t out;
  memcpy(&out, &val, 4);
  return out;
}

/// Dereference pointer to 4 uint8_t as a uint32_t value
static inline uint32_t PunToUint32(const uint8_t* in) {
  uint32_t out;
  memcpy(&out, in, 4);
  return out;
}

/// Dereference pointer to 4 uint8_t as a int32_t value
static inline int32_t PunToInt32(const uint8_t* in) {
  int32_t out;
  memcpy(&out, in, 4);
  return out;
}

/// Dereference pointer to 2 uint8_t as a uint16_t value
static inline uint16_t PunToUint16(const uint8_t* in) {
  uint16_t out;
  memcpy(&out, in, 2);
  return out;
}

/// Dereference pointer to 2 uint8_t as a int16_t value
static inline int16_t PunToInt16(const uint8_t* in) {
  int16_t out;
  memcpy(&out, in, 2);
  return out;
}

#endif