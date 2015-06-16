#ifndef SYSTEM_CLOCK_H
#define SYSTEM_CLOCK_H

// Provides timestamp and delay utilities based a single high speed timer.
// Representations are seconds and nanoseconds, but the true resolution depends 
// on the low level timer configuration.  A typical resolution is 125 ns,
// derived from a 72 MHz clock prescaled by 9.  To avoid drift, the prescale is 
// configured to achieve ticks with an integral duration in nanoseconds.
//
// Both times and durations are described by a Time struct containing seconds 
// and nanoseconds.  Positive increments always indicate future times.
// 
// For example,
//   ( 3 s,   0 ns) is after ( 2 s,  0 ns)
//   ( 3 s,  50 ns) is after ( 3 s, 40 ns)
//   (-3 s,  50 ns) is after (-3 s, 40 ns)
//
// Provided functions always output results where 0 <= ns < 1e9.  They tolerate 
// other inputs at increased computational expense.

// TODO: Do "FromXXX" functions handle negative times correctly?  Probably not.

#include "common_peripherals.h"
#include STM32FXXX_TIM_H      // needed for inline timer access
#include <stdint.h>

#ifdef USING_SYSTEM_CLOCK

#ifdef __cplusplus
extern "C" {
#endif

////////////////////////////////////////////////////////////////////////////////
// Exact Time Structure

struct Time
{
  int32_t sec;  // nanoseconds
  int32_t nsec; // seconds
};

////////////////////////////////////////////////////////////////////////////////
/// Initialize clock counter and timer hardware.

void InitSystemClock();


////////////////////////////////////////////////////////////////////////////////
// Exact Time

struct Time GetTime();


////////////////////////////////////////////////////////////////////////////////
// Exact Time Arithmetic

// Format time with 0 <= ns < 1e9
inline struct Time MakeProper(struct Time t) {
  while(t.nsec >= 1000000000) {
    t.sec += 1;
    t.nsec -= 1000000000;
  }
  while(t.nsec < 0) {
    t.sec -= 1;
    t.nsec += 1000000000;
  }
  return t;
}

// AddTime(t2, t1) = t2 + t1;
inline struct Time AddTime(struct Time t2, struct Time t1) {
  t1 = MakeProper(t1);  // prevents overflow during arithmetic
  t2 = MakeProper(t2);
  t2.sec += t1.sec;
  t2.nsec += t1.nsec;
  return MakeProper(t2);
}

// SubtractTime(t2, t1) = t2 - t1;
inline struct Time SubtractTime(struct Time t2, struct Time t1) {
  t1.sec = -t1.sec;
  t1.nsec = -t1.nsec;
  return AddTime(t2, t1);
}

// GreaterThan(t2, t1) returns true if t2 > t1
inline uint8_t GreaterThan(struct Time t2, struct Time t1) {
  t1 = SubtractTime(t1, t2);
  return (t1.sec < 0);
}

// Requires arguments with |.nsec| < 1e9 (proper times are always ok)
inline void DelayTime(struct Time duration) {
  struct Time stop = AddTime(GetTime(), duration);
  while(GreaterThan(stop, GetTime())) {}
}


////////////////////////////////////////////////////////////////////////////////
// Exact Time Conversions

inline struct Time FromNanoseconds(int32_t ns) {
  struct Time t;
  t.sec = ns/1000000000;
  t.nsec = (ns - t.sec*1000000000);
  return t;
}

inline struct Time FromMicroseconds(int32_t us) {
  struct Time t;
  t.sec = us/1000000;
  t.nsec = (us - t.sec*1000000)*1000;
  return t;
}

inline struct Time FromMilliseconds(int32_t ms) {
  struct Time t;
  t.sec = ms/1000;
  t.nsec = (ms - t.sec*1000)*1000000;
  return t;
}

inline struct Time FromSeconds(float s) {
  struct Time t;
  t.sec = (int32_t)s;
  t.nsec = (s - (float)t.sec)*1000000000;
  return t;
}

////////////////////////////////////////////////////////////////////////////////
// Approximate Time Conversions

// Convert positive time or duration to microseconds. Value wraps normally as a 
// uint32_t integer, ie wraps approximately every 71 minutes.  Negative times 
// or durations return 0.
inline uint32_t ToMicroseconds(struct Time t) {
  t = MakeProper(t);
  if(t.sec >= 0) {
    return (uint32_t)((uint64_t)t.sec*1000000U + (uint32_t)t.nsec/1000U);
  }
  else {
    return 0;
  }
}

inline float ToSeconds(struct Time t) {
  t = MakeProper(t);
  return t.sec + t.nsec*1.0e-9f;
}

////////////////////////////////////////////////////////////////////////////////
// Approximate Blocking Delays

inline void DelaySeconds(float s) {
  DelayTime(FromSeconds(s));
}

inline void DelayMilliseconds(uint32_t ms) {
  DelayTime(FromMilliseconds(ms));
}

inline void DelayMicroseconds(uint32_t us) {
  DelayTime(FromMicroseconds(us));
}
















////////////////////////////////////////////////////////////////////////////////
// Approximate Time
// CANDIDATE FOR FUTURE REMOVAL
//
// DO NOT USE THESE FUNCTIONS TO CALCULATE TIME DIFFERENCES,
// THEY ALL SUFFER FROM VARIABLE PRECISION OR LIMITED RANGE.
//
// To calculate an accurate time interval between two timestamps:
//   struct Time t1    = GetTime();
//   struct Time t2    = GetTime();
//   struct Time diff  = SubtractTime(t2, t1);
//   float seconds         = ToSeconds(diff);        // variable resolution
//   uint32_t microseconds = ToMicroseconds(diff);   // fixed resolution, requires positive interval < 2^32 microseconds
//
// To calculate time elapsed since t1, use
//   float seconds = ToSeconds(SubtractTime(GetTime(), t1));

// Get approximate time since initialization in seconds (s)
// Do not use GetSeconds() for calculating time differences due to incumbent 
// variable precision issues.
inline float GetSeconds() {
  return ToSeconds(GetTime());
}

// Get approximate time since initialization in microseconds (us)
// Do not use GetMicroseconds() for calculating time differences due to 
// incument overflow issues.
// Value wraps normally as a uint32_t integer, ie wraps approximately 
// every 71 minutes.
inline uint32_t GetMicroseconds() {
  return ToMicroseconds(GetTime());
}


#ifdef __cplusplus
}
#endif // __cplusplus

#endif // USING_SYSTEM_CLOCK

#endif // SYSTEM_CLOCK_H