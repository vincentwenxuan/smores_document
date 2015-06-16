#ifndef TIM1_DRIVE_H
#define TIM1_DRIVE_H

#ifdef __cplusplus
extern "C" {
#endif

extern float cmd_volts_mean_1;
extern float cmd_volts_amplitude_1;
extern float cmd_phase_1;

extern float cmd_volts_mean_2;
extern float cmd_volts_amplitude_2;
extern float cmd_phase_2;

void tim1_init();
void tim1_set_supply_volts(float volts);

//void tim1_set_duty1(float duty);
void tim1_set_position_callback_1(float (*func)());
void tim1_set_cmd_volts_mean_1(float volts);
void tim1_set_cmd_volts_amplitude_1(float volts);
void tim1_set_cmd_phase_1(float radians);

//void tim1_set_duty2(float duty);
void tim1_set_position_callback_2(float (*func)());
void tim1_set_cmd_volts_mean_2(float volts);
void tim1_set_cmd_volts_amplitude_2(float volts);
void tim1_set_cmd_phase_2(float radians);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TIM1_DRIVE_H