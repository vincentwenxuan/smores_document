#include "project_message_types.h"
#include "state_machine.h"
#include "system_clock.h"

const float com_duration    = 1.00; // seconds
const float pilot_duration  = 0.25; // seconds

StateMachine::StateMachine():
state(kStop),
standby_needs_init(false)
{
  com_stamp.sec   = 0;
  com_stamp.nsec  = 0;

  pilot_stamp.sec   = 0;
  pilot_stamp.nsec  = 0;
}


void StateMachine::ReadMsg(CommunicationInterface& com, uint8_t* rx_data, uint8_t rx_length) {
  (void)com;        // intentionally unused
  (void)rx_length;  // intentionally unused

  uint8_t type = rx_data[0];

  // message received, reset communications timeout
  com_stamp = GetTime();

  // transition based on current state
  if(state == kStop) {
    if(type == kTypeQuatPilot || type == kTypeQuatFullObsPilot) {
      pilot_stamp = GetTime();
      state = kQuat;
    }
    else if(type == kTypeOpenPilot) {
      pilot_stamp = GetTime();
      state = kOpen;
    }
    else if(type == kTypeOpenAttitudePilot) {
      pilot_stamp = GetTime();
      state = kOpenAttitude;
    }
    else {
      standby_needs_init = false;
      state = kStandby;
    }
  }

  else if(state == kStandby) {
    if(type == kTypeQuatPilot || type == kTypeQuatFullObsPilot) {
      pilot_stamp = GetTime();
      state = kQuat;
    }
    else if(type == kTypeOpenPilot) {
      pilot_stamp = GetTime();
      state = kOpen;
    }
    else if(type == kTypeOpenAttitudePilot) {
      pilot_stamp = GetTime();
      state = kOpenAttitude;
    }
    else if(type == kTypeKill) {
      state = kStop;
    }
  }

  else if(state == kQuat || state == kOpen || state == kOpenAttitude) {
    if(type == kTypeQuatPilot || type == kTypeQuatFullObsPilot) {
      pilot_stamp = GetTime();
      state = kQuat;
    }
    else if(type == kTypeOpenPilot) {
      pilot_stamp = GetTime();
      state = kOpen;
    }
    else if(type == kTypeOpenAttitudePilot) {
      pilot_stamp = GetTime();
      state = kOpenAttitude;
    }
    else if(type == kTypeKill) {
      state = kStop;
    }
    else if (ToSeconds(SubtractTime(GetTime(), pilot_stamp)) > pilot_duration) {
      standby_needs_init = true;
      state = kStandby;
    }
  }
}


enum control_state StateMachine::get_state() {
  if(ToSeconds(SubtractTime(GetTime(), com_stamp)) > com_duration) {
    state = kStop;
  }
  else if (   (state == kQuat || state == kOpen || state == kOpenAttitude)
           && (ToSeconds(SubtractTime(GetTime(), pilot_stamp)) > pilot_duration) ) {
    standby_needs_init = true;
    state = kStandby;
  }
  return state;
}


uint8_t StateMachine::get_standby_needs_init() {
  return standby_needs_init;
}
void StateMachine::clear_standby_needs_init() {
  standby_needs_init = false;
}