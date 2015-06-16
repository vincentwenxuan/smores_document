#ifndef BRUSHLESS_CONTROLLER_H
#define	BRUSHLESS_CONTROLLER_H

#include "persistent_memory.h"
#include "common_peripherals.h"
#include "common_message_types.h"
#include "communication_interface.h"
#include "pid.h"
#include "vector_control.h"

#include <stdint.h>

#if (defined(MOTOR_TIMER) || (defined(SYNC_TIMER_LOW) && defined(SYNC_TIMER_HIGH))) && defined(ENCODER_QEP_TIMER) 


float DefaultCommuteCallback(float);

class BrushlessController {
  public:
    /* This callback is called during encoder commutation.  The float that is 
     * returned is addedto the pwm (-1 to 1).  The input argument is the mechanical 
     * angle in radians.  It is up to the user to set this callback to their function.
     */
    float (*CommuteCallback)(float mech_angle);

    
    /*******************************************************************************
     * Initialization
     ******************************************************************************/
    BrushlessController();
    
    #ifdef USING_PERSISTENT_MEMORY
    BrushlessController(PersistentMemory& mem_);
    #endif
    
    /*******************************************************************************
     * Parsing
     ******************************************************************************/
    /* This is the class's readMsg function, which accepts the current data and length,
    and if it cares about a message, pops the message */
    void ReadMsg(CommunicationInterface& com, uint8_t* data, uint8_t length);
    
    /*******************************************************************************
     * Memory Management
     ******************************************************************************/
    /* Writes specific data to flash.  Use MotorMessage enums to specify which value to write, or use 255 to write all.  No CMDs (command values) are written to flash. 
    */
    #ifdef USING_PERSISTENT_MEMORY
    void WriteFlash(uint8_t variable_number);
    #endif
    
    /* Writes all data to flash.  This is the same as WriteFlash(255).
    */
    #ifdef USING_PERSISTENT_MEMORY
    void WriteAllFlash();
    #endif
    
    /* Reads specific data from flash.  Use MotorMessage enums to specify which value to read, or use 255 to write all.  No CMDs (command values) can be read from flash. 
    */
    #ifdef USING_PERSISTENT_MEMORY
    void ReadFlash(uint8_t variable_number);
    #endif
    
    /* Reads all data from flash into ram.  This is the same as ReadFlash(255).
    */
    #ifdef USING_PERSISTENT_MEMORY
    void ReadAllFlash();
    #endif

    /*******************************************************************************
     * Controller
     ******************************************************************************/

     /*Commutes the motor by reading the location and adjusting the phase PWMs appropriately
      The desired settings must be set (control type, phase/amplitude, etc.)
      This should be called AT LEAST 12 times per electrical cycle (12*# pole pairs*mechanical speed in hz)
     */
    void BrushlessControllerCommute();

    /*Executes the current controller, may it be position, speed, or current
    */
    void BrushlessControllerControl();

    /* Calibrates the angle of the position sensor to a known motor electrical angle.
    pwm_duty (0 to 1) is the pwm duty to be applied to the motor 
    (Voltage = Supply Voltage * pwm_duty).
    time_ms is the time in milliseconds that the controller holds the motor postion.
    Use caution!  This will dump current into your motor in hopes that it makes it to
    the 0 torque location.  Too little current will give a poor calibration while too 
    much current will result in a fire.  Too small of time doesn't give the motor enough
    time to settle into a postion while too much time can increase the chances of fire.
    */
    void CalibrateMotorAngle(float pwm_duty, uint32_t time_ms);

    /*******************************************************************************
     * Get/Set
     ******************************************************************************/
    // Get current motor mechanical angle, absolute with respect to the sensor.
    float get_mechanical_angle();

    /* Sets the PWM and checks that it's between 0-1, sets the controller mode to PWM mode
    */
    void set_pwm_cmd(float pwm);
    
    /* Sets the voltage applied to a motor after checking that the supply can deliver
    */
    void set_voltage_cmd(float voltage);
    
    /* Sets the current to be applied to the motor and puts the controller in current mode. 
    The second parameter indicates if the driver should use regen (regen = 1) or if it 
    should actively apply a curent (0) if the motor direction is opposite of the current
    direction.
    */ 
    //void set_current_cmd(float current, uint8_t regen);
    
    /* Sets the torque to be applied by the motor and puts the controller in torque mode.
    A Kv must be previously set.
    */
    //void set_torque_cmd(float torque);
    
    /* Sets the speed target and puts the controller in speed control mode
    */
    void set_speed_cmd(float speed);
    
    /* Sets the speed target and final position target and puts the controller in speed pos control mode
    */
    void set_speed_position_cmd(float speed, float position);
    
    /* Sets the position target and puts the controller in position control mode
    */
    void set_position_cmd(float position);

    /* Sets the electrical angle and pwm and puts the controller in kElecAngle mode
    */
    void set_elec_angle_cmd(float pwm, float angle);

    /* Sets the supply voltage for all voltage calculations
    */
    void set_voltage_supply(float voltage);
    
    /* Sets the motor-encoder calibration angle in radians (-2*pi to 2*pi)
    */
    void set_motor_encoder_calibration_angle(float angle);
    
    /* Sets the motor data.  motor_pole_pairs_ (0 to Pole Pairs (int16_t)) | motor_Kv (0 to RPM/V) | motor_R_ (0 to ohms) | motor_I0_ (0 to No Load Amps) | motor_V0_ (0 to No Load Volts)
    */
    void set_motor_data(uint16_t motor_pole_pairs_, float motor_Kv, float motor_R_, float motor_I0_, float motor_V0_, uint8_t motor_bemf_shape_);
    
    // Set 0 or 1 to change permutation of three motor wires.
    void set_permute_wires(uint8_t permute);
    
    /* Sets the lead rate. Lead_Per_Velocity (0 to rad/(rad/s))
    */
    void set_motor_electrical_lead_rate(float lead_rate);
    
    /* Sets the gains for current control.  All gains are constrained to be positive.
    */
    void set_current_ctrl(float kp, float ki, float kd);
    
    /* Sets the gains for speed control.  All gains are constrained to be positive.
    */
    void set_speed_ctrl(float kp, float ki, float kd);
    
    /* Sets the gains for position control.  All gains are constrained to be positive.
    */
    void set_pos_ctrl(float kp, float ki, float kd);
    
    /* Sets the pwm count so that the commutation frequency is the specified frequency
    */
    void set_commutation_hz(uint32_t commutation_hz);
    
    // Gets the commutation frequency
    uint32_t get_commutation_hz();
    
    // commanded pwm [-1,1]
    float get_pwm_cmd();   

    // calculated controller output [-1,1]
    float get_pwm();        

    // commanded voltage
    float get_voltage_cmd();
    
    // measured motor speed (rad/s)
    float get_speed();
    
    // inverse of voltage supply
    float get_voltage_supply_inverse();
    
    // state message
    void SendState(CommunicationInterface& com, GetSetEnum);

    // helper function for responses to 'get' access messages
    void ReplyByte(CommunicationInterface& com, uint8_t subtype, uint8_t* data_in, uint8_t length);
    
    // Commute function for shutting off the motor by ensuring all outputs are low
    void BrushlessControllerOffCommute();
    
    // Commute function for controllers that use the encoder. Pass in pwm_duty_cmd as a per unit [-1.0f to 1.0f].
    void BrushlessControllerEncoderCommute(float pwm_duty_cmd);
    
    // Commute funciton for an electrical angle. Pass in a pwm_duty_cmd as a per unit [-1.0f to 1.0f] and electrical_angle_cmd [0.0f to 2.0f*PI)
    void BrushlessControllerElectricalAngleCommute(float pwm_duty_cmd,float electrical_angle_cmd);
    
    // Creates and sends a state response.
    void SendState(CommunicationInterface& com);
    
  private:
    typedef enum{kOffCtrl=-3, kElecAngleCtrl=-2, kCalibrationCtrl=-1, kNoCtrl=0, kPwmCtrl=1, kVoltageCtrl=2, kCurrentCtrl=3, kTorqueCtrl=4, kSpeedCtrl=5, kSpeedPosCtrl=6, kPosCtrl=7} MotorControl;
        
    //Commands
    float pwm_duty_cmd_; // per unit (-1 to 1) pwm that is the input to the commutation funciton
    float voltage_cmd_; // voltage setpoint to calculate pwm_duty_cmd_
    
    // PWM note keeping
    uint16_t max_pwm_; // the maximum timer value (timer periond in clock counts)
    float max_pwm_float_; // the float version of max_pwm_, pre-converted for speed
    
    // Control
    volatile MotorControl control_mode_; // motor control mode state, for the motor control state machine
    struct Time calibration_end_time_;

    // Observed state
    float voltage_supply_; // The supply voltage.  This value is externally supplied
    float voltage_supply_inv_; // The inverse of the supply voltage, kept for speed
    struct Time prev_control_time_; // time stamp of previous speed update
    
    // Calculated state
    float pwm_duty_final_;
    float current_motor_speed_;      // present motor velocity (rad/s)
    float motor_lead_elec_angle_;    // calculated amount to lead electrical position (rad)
    
    // Motor Drive Properties
    float motor_electrical_lead_rate_;       // radians per (rad/s)
    float motor_encoder_calibration_angle_;  // negative of position at calibration (rad)
    uint16_t motor_pole_pairs_;              // number of motor pole pairs (# magnets/2)
    Shape motor_bemf_shape_;                 // The back emf shape, trapezoidal, sinusoidal, or quadrature
    // float motor_Kte_;                     // motor torque constant
    // float motor_R_;
    // float motor_V0_;
    // float motor_I0_;
    uint8_t permute_wires_;                  // Swap two wires to reverse motor direction
    float speed_filter_alpha_; // speed filter coefficient in interval [0,1]
    
    // Controller PID Instances
    // Pid current_ctrl_pid_;
    Pid speed_ctrl_pid_;
    Pid pos_ctrl_pid_;
    
    
    void InitBrushlessController(void);
    
    #ifdef USING_PERSISTENT_MEMORY
    PersistentMemory* mem_;
    #endif

  }; // class BrushlessController

#endif // #if (defined(MOTOR_TIMER) || (defined(SYNC_TIMER_LOW) && defined(SYNC_TIMER_HIGH))) && defined(ENCODER_QEP_TIMER) 
#endif // BRUSHLESS_CONTROLLER_H