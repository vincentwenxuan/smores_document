#ifndef BATTERY_MONITOR_H
#define BATTERY_MONITOR_H

#include "communication_interface.h"
#include "persistent_memory.h"
#include "battery_monitor_it.h"
#include "stdint.h"

#ifdef USING_BATTERY_MONITOR

class BatteryMonitor {
  public:

    BatteryMonitor(PersistentMemory& memory);

    void ReadMsg(CommunicationInterface& com, uint8_t* rx_data, uint8_t rx_length);

    //uint8_t ReadRegister(uint16_t reg, uint32_t &value);

    void SendMsgBatteryReport(CommunicationInterface& com);

    float GetVoltsFilt()  { return BattItGetVoltsFilt(); };
    float GetAmpsFilt()   { return BattItGetAmpsFilt();  };
    float GetWattsFilt()  { return BattItGetWattsFilt(); };
    float GetJoules()     { return BattItGetJoules(); };

  private:
    PersistentMemory* mem;
};

#endif // USING_BATTERY_MONITOR

#endif // BATTERY_MONITOR_H