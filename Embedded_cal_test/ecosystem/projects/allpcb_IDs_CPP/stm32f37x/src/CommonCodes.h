/*
 * CommonCodes.h
 *
 *  Copyright (c) 2014 Kumar Robotics. All rights reserved. Proprietary License.
 *
 *	Created on: 2014-07-03
 *      Author: gareth
 */

#ifndef COMMON_CODES_H_
#define COMMON_CODES_H_

/**
 *  @brief Error code mechanism for creating descriptive errors without wasting
 *   uController memory on string storage.
 *
 *  @note Compile with -DBUILD_DESKTOP on desktop, and -DBUILD_ARM on M4.
 */
namespace Error {

//  TEMP: replace me later
#define BUILD_ARM

#if defined(BUILD_ARM)

//  adds an enum entry
#define DEFCODE(a,b) a
enum Code {
  None = 0,
  
#elif defined(BUILD_DESKTOP)

//  declares a string constant
#define DEFCODE(a,b) b
  
const char * const kCommonMessageDefs[] = { "No error",
  
#else
#error BUILD_ARM or BUILD_DESKTOP must be defined to compile this header.
#endif
    
  /*
   *  Place error code definitions below:
   */
  DEFCODE(IMUConnection, "IMU is not connected"),
  DEFCODE(IMUState, "Failed to wake IMU from sleep"),
  DEFCODE(IMUOption, "Failed to configure IMU"),
  DEFCODE(IMURead, "Failed to read from IMU"),
  
  DEFCODE(FilterSingular, "State estimator is numerically unstable"),
  
  DEFCODE(CommOverflow, "Received packets are being dropped")
  
#if defined(BUILD_ARM)
};  //  enum Code
#elif defined(BUILD_DESKTOP)
};  //  kCommonMessageDefs
#endif

};  //  namespace Error

#ifdef DEFCODE
#undef DEFCODE
#endif

#endif  //  COMMON_CODES_H_
