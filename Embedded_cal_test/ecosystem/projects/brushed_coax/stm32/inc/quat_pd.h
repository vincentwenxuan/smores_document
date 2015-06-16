#ifndef QUAT_PD_H
#define QUAT_PD_H

#include "common_peripherals.h"
#include "persistent_memory.h"
#include "project_message_types.h"
#include "quat_state.h"
#include "biquad_lpf_dynamic_c.h"
#include "quat_control_math.hpp"

#include <stdint.h>

typedef struct {
  float thrust;
  float roll;
  float pitch;
  float yaw;
} OrthoControl;

class QuatPD {

  public:

    // Constructor with memory
    QuatPD(PersistentMemory &mem);

    // Calculate Control Law
    void Calculate(float dt, float thrust, const QuatState& des, const QuatState& est, float omega, OrthoControl& u);

    // Read message and act
    void ReadMsg(CommunicationInterface& com, uint8_t* rx_data, uint8_t rx_length);

    // ControlGains
    void FillMsgQuatPDParam(MsgQuatPDParam *tx_msg);
    void SendMsgQuatPDParam(CommunicationInterface &com);

    // filtered angular rates
    float w_filt[3];

    float uz_f;
    float ux_g;
    float uy_g;
    QuatControlMath<float> controller;

  private:
  
    PersistentMemory* mem;
    
    // controller parameters
    float f0_yaw;     // model feedforward polynomial (volts)
    float f1_yaw;     // model feedforward polynomial (volts/volt)
    float f2_yaw;     // model feedforward polynomial (volts/volts^2)

    float wxy_f0;     // angular rate filter corner frequency
    float wz_f0;

    float g;

    struct BiquadLpfDynamicC wx_filt;
    struct BiquadLpfDynamicC wy_filt;
    struct BiquadLpfDynamicC wz_filt;
};
  
#endif // QUAT_PD_H