// The CoaxOpenAttitudeController is a closed loop yaw controller augmented 
// with open loop attitude commands.  This is not a practical controller for 
// heads up flight of unstable vehicles.  However, it is appropriate for 
// vehicles with passively stable attitude dynamics, and it enables 
// somewhat repeatable open loop tests.

#ifndef COAX_OPEN_ATTITUDE_CONTROLLER_H
#define COAX_OPEN_ATTITUDE_CONTROLLER_H

#include "project_message_types.h"
#include "communication_interface.h"
#include "persistent_memory.h"

#include "gyro_accel_drift_ukf_io.h"

#include "quat_state.h"
#include "biquad_lpf_dynamic_c.h"
#include "quat_control_math.hpp"

class CoaxOpenAttitudeController {
  public:
    CoaxOpenAttitudeController( PersistentMemory &mem,
                                GyroAccelDriftUkfIo &estimator);

    void Update();

    void ReadMsg(CommunicationInterface& com, uint8_t* rx_data, uint8_t rx_length);

    void FillMsgCoaxOpenAttitudeControllerParam(MsgCoaxOpenAttitudeControllerParam *tx_msg);
    void SendMsgCoaxOpenAttitudeControllerParam(CommunicationInterface &com);

    void SendMsgDebug(CommunicationInterface& com);
    void FillMsgDebug(MsgCoaxOpenAttitudeControllerDebug* tx_msg);

    // outputs
    float top_mean;         // volts
    float bottom_mean;      // volts
    float top_pulse_amp;    // volts
    float top_pulse_phase;  // radians

  private:
    PersistentMemory* mem;
    GyroAccelDriftUkfIo *ukf;

    QuatControlMath<float> controller;

    struct BiquadLpfDynamicC wz_filter;

    // controller parameters
    float f0_yaw;     // model feedforward polynomial (volts)
    float f1_yaw;     // model feedforward polynomial (volts/volt)
    float f2_yaw;     // model feedforward polynomial (volts/volts^2)
    float wz_f0;
    float pulse_sat;
    float yaw_sat;
    float thrust_sat;
    float advance_angle;

    // control state
    float uz_f;
    float pilot_thrust;
    float pilot_pulse_phase;
    float pilot_yaw;
    struct Time t;
};

#endif // COAX_OPEN_ATTITUDE_CONTROLLER_H