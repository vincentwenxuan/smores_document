// The CoaxOpenController is a fully open loop, feedforward controller for a 
// pulsing mode coaxial uav.  This is not a practical controller for human 
// flight, but enables repeatable open loop tests.

#ifndef COAX_OPEN_CONTROLLER_H
#define COAX_OPEN_CONTROLLER_H

#include "project_message_types.h"
#include "communication_interface.h"
#include "persistent_memory.h"

class CoaxOpenController {
  public:
    CoaxOpenController(PersistentMemory &mem);

    void Update();

    void ReadMsg(CommunicationInterface& com, uint8_t* rx_data, uint8_t rx_length);

    void FillMsgCoaxOpenControllerParam(MsgCoaxOpenControllerParam *tx_msg);
    void SendMsgCoaxOpenControllerParam(CommunicationInterface &com);

    // outputs
    float top_mean;         // volts
    float bottom_mean;      // volts
    float top_pulse_amp;    // volts
    float top_pulse_phase;  // radians

  private:
    PersistentMemory* mem;

    // controller parameters
    float f0_yaw;     // model feedforward polynomial (volts)
    float f1_yaw;     // model feedforward polynomial (volts/volt)
    float f2_yaw;     // model feedforward polynomial (volts/volts^2)
};

#endif // COAX_OPEN_CONTROLLER_H