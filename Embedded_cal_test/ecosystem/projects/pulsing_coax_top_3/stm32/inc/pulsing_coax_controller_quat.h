// The PulsingCoaxControllerQuat object listens for pilot commands and returns 
// motor command values according to a quaternion PD control law.

#ifndef PULSING_COAX_CONTROLLER_QUAT_H
#define PULSING_COAX_CONTROLLER_QUAT_H

#include "common_peripherals.h"
#include "project_message_types.h"

#include "persistent_memory.h"
#include "communication_interface.h"
#include "quat_pd.h"
#include "mpu_60XX_imu.h"
#include "gyro_accel_drift_ukf_io.h"
#include "motor_hal.hpp"
#include "biquad_lpf_dynamic_c.h"

#include <stdint.h>

class PulsingCoaxControllerQuat {

  public:
    // Constructor with memory. Requires call to InitFromMemory() after Freeze()
    PulsingCoaxControllerQuat( PersistentMemory &memory,
                                QuatPD &controller,
                                Mpu60XXImu &inertial_measurement_unit,
                                GyroAccelDriftUkfIo &estimator,
                                MotorHal &motor_hal);

    // Update control law with estimates
    void Update();
    
    ////////////////////////////////////////////////////////////////////////////
    // MESSAGE OPERATIONS
    
    // Read message and act (presently over USB, in future general)
    void ReadMsg(CommunicationInterface& com, uint8_t* rx_data, uint8_t rx_length);

    // DebugControlVar
    //void FillMsgDebugControlVar(MsgDebugControlVar* tx_msg);
    //void SendMsgDebugControlVar(CommunicationInterface& com);    // all control variables

    // PulsingCoaxParam
    void FillMsgPulsingCoaxParam(MsgPulsingCoaxParam* tx_msg);
    void SendMsgPulsingCoaxParam(CommunicationInterface& com);

    // RollReport
    void FillMsgRollReport(MsgRollReport* tx_msg);
    void SendMsgRollReport(CommunicationInterface& com);

    // YawReport
    void FillMsgYawReport(MsgYawReport* tx_msg);
    void SendMsgYawReport(CommunicationInterface& com);

    // ControlCalc
    void FillMsgControlCalc(MsgControlCalc* tx_msg);
    void SendMsgControlCalc(CommunicationInterface& com);

    // AttitudeReport
    //void FillMsgAttitudeReport(MsgAttitudeReport* tx_msg);
    //void SendMsgAttitudeReport(CommunicationInterface& com);
  
    // outputs
    float top_mean;         // volts
    float top_pulse_amp;    // volts
    float top_pulse_phase;  // radians
    float bottom_mean;      // volts

    float u_phase;

    // flash parameters
    float advance_angle;    // radians

  
  private:

    PersistentMemory *mem;
    QuatPD *pd;
    Mpu60XXImu *imu;
    GyroAccelDriftUkfIo *ukf;
    MotorHal *motor_hal;
    
    // flash parameters
    float pulse_thresh;
    float pulse_min;
    float pulse_sat;
    float yaw_sat;
    float thrust_sat;
    uint8_t enable_bottom;

    QuatState des;

    // desired state
    struct Time t;
    float thrust_des;
    float roll_des;
    float pitch_des;
    float yaw_des;
    float yaw_des_rate;

    OrthoControl u;
    struct BiquadLpfDynamicC omega_filter;
};
  
#endif // PULSING_COAX_CONTROLLER_QUAT_H