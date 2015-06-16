#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <stdint.h>
#include "communication_interface.h"
#include "system_clock.h"

enum control_state {kStop, kStandby, kQuat, kOpen, kOpenAttitude};

class StateMachine {

  public:

    StateMachine();

    void ReadMsg(CommunicationInterface& com, uint8_t* rx_data, uint8_t rx_length);

    enum control_state get_state();

    uint8_t get_standby_needs_init();
    void clear_standby_needs_init();

  private:

    enum control_state state;

    struct Time com_stamp;
    struct Time pilot_stamp;

    bool standby_needs_init;
};

#endif // STATE_MACHINE_H