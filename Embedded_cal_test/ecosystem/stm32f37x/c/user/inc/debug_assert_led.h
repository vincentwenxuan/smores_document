// Turn LED on if and only if a logical test switches from true to false inside 
// a test section of code.
//
// Example:
//
// assert_pre(everythingIsNormal);
// thinkProblemIsHere();
// assert_post(everythingIsNormal);
//
// The LED turns on only if test everythingIsNormal was true before 
// thinkProblemIsHere() and was false after thinkProblemIsHere().

#ifdef __cplusplus
extern "C" {
#endif

// global variable stores assert pre result
uint8_t assert_pre_result = 0;

// call before test section
// evaluates logical test and stores result: pass if true
// LED turns OFF always
void assert_pre(uint8_t test);

// call after test section
// evaluates logical test: pass if true
// LED turns ON iff pre was false and post is true
void assert_post(uint8_t test);

#ifdef __cplusplus
}
#endif // __cplusplus