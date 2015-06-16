#include <stdint.h>
#include "common_peripherals.h"

// global variable stores assert pre result
uint8_t assert_pre_result = 0;

// call before test section
// evaluates logical test and stores result: pass if true
// LED turns OFF always
void assert_pre(uint8_t test) {
  assert_pre_result = test;
  mGreenOFF;
}

// call after test section
// evaluates logical test: pass if true
// LED turns ON iff pre was false and post is true
void assert_post(uint8_t test) {
  if(!assert_pre_result && test)
    mGreenON;
}