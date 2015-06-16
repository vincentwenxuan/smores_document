#include "pid.h"
#include "float.h"
#include "mGeneral.h"
#include "math_helper.h"

Pid::Pid()
{
  Kp = 0;
  Ki = 0;
  Kd = 0;
  integral_clamp = FLT_MAX;
  
  set_point = 0;
  set_point_dot = 0;
  feed_forward = 0;
  error_integral = 0;
  last_process_variable = 0;
  
  set_point_changed = true;
}

float Pid::PidCompute(float process_variable)
{
  float process_variable_dot;
  process_variable_dot = process_variable - last_process_variable;
  last_process_variable = process_variable;
  return PidCompute(process_variable, process_variable_dot);
}

float Pid::PidCompute(float process_variable, float process_variable_dot)
{
  float manipulated_variable, error, error_dot;
  error = set_point - process_variable;
  error_dot = set_point_dot - process_variable_dot;
  error_integral = SatFloat(error_integral + Ki*error, -integral_clamp, integral_clamp);
  manipulated_variable = Kp*error + error_integral + Kd*error_dot;
  last_process_variable = process_variable; // this gets done twice if this is called from the other PidCompute.  Oh well.
  return manipulated_variable;
}

float Pid::PidComputeRadMod(float process_variable)
{
  float process_variable_dot;
  process_variable_dot = WrapRad(process_variable - last_process_variable);
  last_process_variable = process_variable;
  return PidComputeRadMod(process_variable, process_variable_dot);
}

float Pid::PidComputeRadMod(float process_variable, float process_variable_dot)
{
  float manipulated_variable, error, error_dot;
  error = WrapRad(set_point - process_variable);
  error_dot = set_point_dot - process_variable_dot;
  error_integral = SatFloat(error_integral + Ki*error, -integral_clamp, integral_clamp);
  manipulated_variable = Kp*error + error_integral + Kd*error_dot;
  last_process_variable = process_variable; // this gets done twice if this is called from the other PidCompute.  Oh well.
  return manipulated_variable;
}

int8_t Pid::set_set_point(float in)
{
  set_point = in;
  set_point_changed = true;
  return SUCCESS;
}
  
int8_t Pid::set_set_point_dot(float in)
{
  set_point_dot = in;
  return SUCCESS;
}

int8_t Pid::set_Kp(float in)
{
  if(in >= 0)
  {
    Kp = in;
    return SUCCESS;
  }
  return ERROR;
}

int8_t Pid::set_Ki(float in)
{
  if(in >= 0)
  {
    Ki = in;
    return SUCCESS;
  }
  return ERROR;
}

int8_t Pid::set_Kd(float in)
{
  if(in >= 0)
  {
    Kd = in;
    return SUCCESS;
  }
  return ERROR;
}

int8_t Pid::set_integral_clamp(float in)
{
  integral_clamp = in;
  return SUCCESS;
}

int8_t Pid::set_feed_forward(float in)
{
  feed_forward = in;
  return SUCCESS;
}

float Pid::get_set_point(){return set_point;}
  
float Pid::get_set_point_dot(){return set_point_dot;}

float Pid::get_Kp(){return Kp;}

float Pid::get_Ki(){return Ki;}

float Pid::get_Kd(){return Kd;}

float Pid::get_integral_clamp(){return integral_clamp;}

float Pid::get_feed_forward(){return feed_forward;}