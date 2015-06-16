#include "vector_control.h"
//#include "system_stm32f37x.h"
#include "arm_math.h"
#include "common_peripherals.h"
#include "space_vector_helper.h"

#ifndef PI
#define PI					3.141592653589793f
#endif
#define PI_BY_3     1.047197551196598f
#define INV_PI_BY_3 0.954929658551372f
#define PI_BY_2     1.570796326794897f
#define INV_PI_BY_2 0.636619772367581f
					
const int ref_vectors6[6][3] = {{0,1,1},{0,0,1},{1,0,1},{1,0,0},{1,1,0},{0,1,0}};  // phase vectors		
const int ref_vectors4[4][3] = {{0,0,1},{1,0,0},{1,1,0},{0,1,1}};  // phase vectors				

// input mech_angle in radians, arbitrary range
// output elec_angle in radians, [-2*PI, 2*PI]
float MechToElecAngle(float mech_angle, uint16_t motor_poles)
{
  float elec_angle = mech_angle*motor_poles;
  int16_t divs = (float)elec_angle*0.159154943091895f;// /(2.0f*PI); // truncate towards zero
  return elec_angle - 6.283185307179586f*divs; //2.0f*PI
}

// input elec_ang in radians, arbitrary range
void CalcSpaceVector(float elec_ang, float amplitude, float *pwms, enum Shape sv_shape){
  // start 3.2us
	float theta_da, theta_db;
	float t1, t2, t0;											// Time slots for 3 vectors.
	int section,  next_section;

  //start 700ns
  // wrap input angle to [0, 2*PI]
  while(elec_ang >= 2*PI){
    elec_ang = elec_ang - 2.0f*PI;
  };
  while(elec_ang < 0.0f){
    elec_ang = elec_ang + 2.0f*PI;
  };
  //end 700ns

  // start 2.8us
  /*
  if(elec_ang >= PI)
  {
    if(elec_ang >= 4.0f*PI_BY_3)
    {
      if( elec_ang >= 5.0f*PI_BY_3)
      {
        section = 5;
        next_section = 0;
      }
      else // elec_ang >= 4PI/3 and elec_ang < 5PI/3
      {
        section = 4;
        next_section = 5;
      }
    }
    else //elec_ang < 4PI/3 and elec_ang >= PI
    {
      section = 3;
      next_section = 4;
    }
  }
  else //elec_ang < PI
  {
    if(elec_ang >= PI_BY_3)
    {
      if(elec_ang >= 2.0f*PI_BY_3)
      {
        section = 2;
        next_section = 3;
      }
      else //elec_ang < 2PI/3 and elec_ang > PI/3
      {
        section = 1;
        next_section = 2;
      }
    }
    else //elec_ang < PI/3
    {
      section = 0;
      next_section = 1;
    }
  }
  */
  // end 2.8us

  switch(sv_shape)
  {
  case kTrapezoidal:
	// trapezoidal
    // start < 100ns (trapezoidal)
    // start <100ns
    section = elec_ang * INV_PI_BY_3; //perhaps change to ifs
    next_section = section + 1;
    if(next_section == 6) {
      next_section = 0;
    }
    // end <100ns
    
    theta_da = elec_ang - ((float)section)*PI_BY_3;	//Theta, the electrical angle between current angle and section boundary, from 0 to pi/3 
    // scale factor (in simplest form)
		//theta_db = sin(theta_da)/sin((PI/3)+theta_da);                  // 81-108us
    //theta_db = arm_sin_f32(theta_da)/arm_sin_f32(PI_BY_3+theta_da); // 4.6us
    theta_db = SpaceVectorHelper(theta_da);                         // <100 ns (how is this possible?)

    ///40ns
		t1 = theta_db*amplitude; 						// Time for using vector 1
		t2 = (1.0f-theta_db)*amplitude;   			// Time for using vector 2
		t0 = (1.0f-amplitude)*0.5f;				    		// Time for 0 vector.
    ///end 40ns
    
    // start 95ns
    pwms[0] = (t0 + t1*ref_vectors6[next_section][0]) + t2*ref_vectors6[section][0];
    pwms[1] = (t0 + t1*ref_vectors6[next_section][1]) + t2*ref_vectors6[section][1];
    pwms[2] = (t0 + t1*ref_vectors6[next_section][2]) + t2*ref_vectors6[section][2];
    // end 95ns
    break;
  // end < 100ns (trapezoidal)
	case kSinusoidal:
	// sinusoidal
    // start <100ns
    section = elec_ang * INV_PI_BY_3; //perhaps change to ifs
    next_section = section + 1;
    if(next_section == 6) {
      next_section = 0;
    }
    // end <100ns
  	theta_da = elec_ang - ((float)section)*PI_BY_3;	//Theta, the electrical angle between current angle and section boundary, from 0 to pi/3 
		// Maximum possible torque assumed
    theta_db = PI_BY_3 - theta_da;										//pi/3-theta.
		///55-76us
    //t2 = sin(theta_da)*amplitude;
		//t1 = sin(theta_db)*amplitude;
    ///end 55-76us
    t1 = arm_sin_f32(theta_da)*amplitude;
		t2 = arm_sin_f32(theta_db)*amplitude;
		t0 = (1.0f-t1-t2)*0.5f;							// Time for 0 vector.
    
    // start 95ns
    pwms[0] = (t0 + t1*ref_vectors6[next_section][0]) + t2*ref_vectors6[section][0];
    pwms[1] = (t0 + t1*ref_vectors6[next_section][1]) + t2*ref_vectors6[section][1];
    pwms[2] = (t0 + t1*ref_vectors6[next_section][2]) + t2*ref_vectors6[section][2];
    // end 95ns
    break;
  case kQuadrature:
  // quadrature
    section = elec_ang * INV_PI_BY_2; //perhaps change to ifs
    next_section = section + 1;
    if(next_section == 4) {
      next_section = 0;
    }
    
    theta_da = elec_ang - ((float)section)*PI_BY_2;
    theta_db = PI_BY_2 - theta_da;
    amplitude = amplitude*0.70710678118f;
    t1 = arm_sin_f32(theta_da)*amplitude;
    t2 = arm_sin_f32(theta_db)*amplitude;
    t0 = (1.0f-t1-t2)*0.5f;
    
    pwms[0] = (t0 + t1*ref_vectors4[next_section][0]) + t2*ref_vectors4[section][0]; //cos
    pwms[1] = (t0 + t1*ref_vectors4[next_section][1]) + t2*ref_vectors4[section][1]; //common
    pwms[2] = (t0 + t1*ref_vectors4[next_section][2]) + t2*ref_vectors4[section][2]; //sin
    break;
  default:
    t1 = 0;
    t2 = 0;
    t0 = 0;
  }

  // end 3.2us
}