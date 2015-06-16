#ifndef POLLING_REPORTER_H
#define POLLING_REPORTER_H

// general requirements for polling reporter
#include "project_message_types.h"
#include "communication_interface.h"
#include "persistent_memory.h"
#include "stdint.h"

// components reported on
#include "pulsing_controller.h"
#include "battery_monitor.h"

class PollingReporter {
  public:

    PollingReporter(PersistentMemory            &mem,
                    PulsingController           &motor,
                    BatteryMonitor              &battery);

    void ReadMsg(CommunicationInterface &com, uint8_t *rx_data, uint8_t rx_length);

    void Stream(CommunicationInterface &com);

  private:
    PersistentMemory            *mem;
    PulsingController           *motor;
    BatteryMonitor              *battery;

    float time_prev_report;

    float   stream_freq;
    uint8_t stream_enabled;
    uint8_t type_trigger;
    uint8_t send_motor_report;
    uint8_t send_battery_report;
    uint8_t send_test_stand;

    void SendMsgPollingReporterParam(CommunicationInterface &com);
    void FillMsgPollingReporterParam(MsgPollingReporterParam *tx_msg);

    void SendMsgTestStand(CommunicationInterface &com);
    void FillMsgTestStand(MsgTestStand *tx_msg);
};

#endif // POLLING_REPORTER_H