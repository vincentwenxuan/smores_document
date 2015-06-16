#ifndef PID_H
#define PID_H

#include <stdint.h>

class Pid {
private: 
  bool set_point_changed;
  float set_point; // Set point
  float set_point_dot; // Set point derivative
  float last_process_variable;
  float error_integral;
  float Kp;
  float Ki;
  float Kd;
  float integral_clamp;
  float feed_forward; // Feed forward
public:
  Pid(); // Constructor

  /// Takes in a process variable and returns the manipulated variable.
  /// Kp, Ki, Kd, feed_forward, set_point, and set_point_dot should all
  /// be set (or left as default) before calling this function.
  float PidCompute(float);

  /// Takes in a process variable and it's externally computed derivative 
  /// and returns the manipulated variable.
  /// Kp, Ki, Kd, feed_forward, set_point, and set_point_dot should all
  /// be set (or left as default) before calling this function.
  float PidCompute(float, float);
  
  /// Takes in a process variable and returns the manipulated variable.
  /// Kp, Ki, Kd, feed_forward, set_point, and set_point_dot should all
  /// be set (or left as default) before calling this function.
  /// This version will wrap error between (-pi,pi)
  float PidComputeRadMod(float);

  /// Takes in a process variable and it's externally computed derivative 
  /// and returns the manipulated variable.
  /// Kp, Ki, Kd, feed_forward, set_point, and set_point_dot should all
  /// be set (or left as default) before calling this function.
  /// This version will wrap error between (-pi,pi)
  float PidComputeRadMod(float, float);
  
  /*************SETTERS*************/
  int8_t set_set_point(float);
  
  int8_t set_set_point_dot(float);
  
  int8_t set_Kp(float);
  
  int8_t set_Ki(float);
  
  int8_t set_Kd(float);
  
  int8_t set_integral_clamp(float);
  
  int8_t set_feed_forward(float);
  
  float get_set_point();
  
  float get_set_point_dot();
  
  float get_Kp();
  
  float get_Ki();
  
  float get_Kd();
  
  float get_integral_clamp();
  
  float get_feed_forward();
  
  
  
  }; // class Pid

#endif /* PID_H */