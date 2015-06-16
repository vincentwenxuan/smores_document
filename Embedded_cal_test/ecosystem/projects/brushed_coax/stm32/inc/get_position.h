#ifndef GET_POSITION_H
#define GET_POSITION_H

#ifdef __cplusplus
extern "C" {
#endif

void InitPositionSensor(void);  // initialize inputs and interrupts
float get_position(void);       // rad
float get_velocity(void);       // rad/s

#ifdef __cplusplus
}
#endif // __cplusplus

#endif