This file focus on the auto-calibration method. To make the codes work, we added and changed some commands in the main functions, both in atmega and stm32.
Some head files and c/cpp files also changed or added.

1. common_macros.h are added to store some macros which was defined in main functions. So pay attention about the multi-definition before merging.

2. TWI_slave.h is changed to prevent multi-definition.

3. Wiper_encoder.c is changed to make sure we can request the raw data without position/velocity calculation.

4. atm_cali.c and .h are added to do the auto calibraion in atmega part. You can just add one command in main code: cali_mode();   The atm_cali.c will do the rest part.

5. stm_cali.cpp and .h are added to do the the auto calibration in stm32 part. You can just add one command in main code: cali_cmd(left_wheel_motor, right_wheel_motor, right_pan_tilt_motor, left_pan_tilt_motor); The stm_cali.cpp will do the rest part.

6. USART.c and .h are deleted to prevent the memory overflow in atmega. Remember to put them back when new chips are applied.

7. smores_wiper_encoder_test is used to show the 4 calibrated values (adc0_0, adc0_180, adc1_0, adc1_180)

8. smores_wiper_encoder_calibration is the old version. (We found out the new version is not working at all. We cannot read the data from usb.)

9. We put all the things to the smores_autocali (atmega part) and smores_autocalibration (stm32 part).

10. We change the project_message_types.h, add the autocalibration command type



P.S. resolution calculation is wrong! (FIXED)
