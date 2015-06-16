#ifndef PULSING_CONTROLLER_H
#define	PULSING_CONTROLLER_H

#include "common_peripherals.h"
#include "persistent_memory.h"
#include "communication_interface.h"
#include "brushless_controller.h"

#if defined(SYNC_TIMER_LOW) && defined(SYNC_TIMER_HIGH) && defined(ENCODER_QEP_TIMER) 

#include <stdint.h>

float PulsingCommuteCallback(float);

class PulsingController: public BrushlessController {

  public:

    PulsingController(PersistentMemory& mem);

    void ReadMsg(CommunicationInterface& com, uint8_t* data, uint8_t length);

    void set_cmd_pulse_phase(float phase);
    float get_cmd_pulse_phase();

    void set_cmd_volts_pulse_amp(float amp);
    float get_cmd_volts_pulse_amp();

    void SendState(CommunicationInterface& com);

  private:
    
    PersistentMemory* mem;

    float cmd_pulse_phase;  // commanded sin pulse phase
    float cmd_volts_pulse_amp;    // commanded sin pulse amplitude
    
}; // class PulsingController

#endif // #if defined(SYNC_TIMER_LOW) && defined(SYNC_TIMER_HIGH) && defined(ENCODER_QEP_TIMER)
#endif // PULSING_CONTROLLER_H