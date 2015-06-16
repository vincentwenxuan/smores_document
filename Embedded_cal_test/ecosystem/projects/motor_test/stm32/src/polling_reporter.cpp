#include "polling_reporter.h"
#include "project_message_types.h"
#include "math_helper.h"

// persistent memory key values (based on component type and subtypes)
const uint16_t key_stream_freq                = (uint16_t)256*kTypePollingReporter + kSubtypeStreamFreq;
const uint16_t key_stream_enabled             = (uint16_t)256*kTypePollingReporter + kSubtypeStreamEnabled;
const uint16_t key_type_trigger               = (uint16_t)256*kTypePollingReporter + kSubtypeTypeTrigger;
const uint16_t key_send_motor_report          = (uint16_t)256*kTypePollingReporter + kSubtypeSendMotorReport;
const uint16_t key_send_battery_report        = (uint16_t)256*kTypePollingReporter + kSubtypeSendBatteryReport;
const uint16_t key_send_test_stand            = (uint16_t)256*kTypePollingReporter + kSubtypeSendTestStand;

PollingReporter::PollingReporter( PersistentMemory            &log_mem,
                                  PulsingController           &log_motor,
                                  BatteryMonitor              &log_battery):
stream_freq(500.0),
stream_enabled(0),
type_trigger(0),
send_motor_report(0),
send_battery_report(0),
send_test_stand(0)
{
  // store pointers to reported devices
  mem = &log_mem;
  motor = &log_motor;
  battery = &log_battery;

  // request persistent memory service
  mem->AddKey(key_stream_freq);
  mem->AddKey(key_stream_enabled);
  mem->AddKey(key_type_trigger);
  mem->AddKey(key_send_motor_report);
  mem->AddKey(key_send_battery_report);
  mem->AddKey(key_send_test_stand);

  // load parameters from memory
  mem->Read(key_stream_freq,                (uint32_t*)&stream_freq);
  mem->Read(key_stream_enabled,             (uint32_t*)&stream_enabled);
  mem->Read(key_type_trigger,               (uint32_t*)&type_trigger);
  mem->Read(key_send_motor_report,          (uint32_t*)&send_motor_report);
  mem->Read(key_send_battery_report,        (uint32_t*)&send_battery_report);
  mem->Read(key_send_test_stand,            (uint32_t*)&send_test_stand);
}

void PollingReporter::ReadMsg(CommunicationInterface &com, uint8_t *rx_data, uint8_t rx_length)
 {
  uint8_t type = rx_data[0];

  if(type == type_trigger || type == kTypePoll)
  {
    if(send_motor_report) {
      motor->SendState(com);
    }
    if(send_battery_report) {
      battery->SendMsgBatteryReport(com);
    }
    if(send_test_stand) {
      SendMsgTestStand(com);
    }
  } // end if pilot message types


  if(type == kTypePollingReporter && rx_length >= 3)
  {
    uint8_t subtype = rx_data[1];
    uint8_t direction = rx_data[2];

    if(direction == GET_CMD) {
      uint8_t tx_msg[6];
      tx_msg[0] = subtype;
      tx_msg[1] = RESPONSE_CMD;
      switch(subtype) {
        case kSubtypeStreamFreq:
          memcpy(&tx_msg[2], &stream_freq, 4);
          com.SendPacket(kTypePollingReporter, tx_msg, 6); break;
        case kSubtypeStreamEnabled:
          tx_msg[2] = stream_enabled;
          com.SendPacket(kTypePollingReporter, tx_msg, 3); break;
        case kSubtypeTypeTrigger:
          tx_msg[2] = type_trigger;
          com.SendPacket(kTypePollingReporter, tx_msg, 3); break;
        case kSubtypeSendMotorReport:
          tx_msg[2] = send_motor_report;
          com.SendPacket(kTypePollingReporter, tx_msg, 3); break;
        case kSubtypeSendBatteryReport:
          tx_msg[2] = send_battery_report;
          com.SendPacket(kTypePollingReporter, tx_msg, 3); break;
        case kSubtypeSendTestStand:
          tx_msg[2] = send_test_stand;
          com.SendPacket(kTypePollingReporter, tx_msg, 3); break;
      }
    } // end if get

    else if(direction == SET_CMD) {
      switch(subtype) {
        case kSubtypeStreamFreq:
          stream_freq         = PunToFloat(&rx_data[3]); break;
        case kSubtypeStreamEnabled:
          stream_enabled      = rx_data[3]; break;
        case kSubtypeTypeTrigger:
          type_trigger        = rx_data[3]; break;
        case kSubtypeSendMotorReport:
          send_motor_report   = rx_data[3]; break;
        case kSubtypeSendBatteryReport:
          send_battery_report = rx_data[3]; break;
        case kSubtypeSendTestStand:
          send_test_stand     = rx_data[3]; break;
      }
    } // end if set

    else if(direction == SAVE_CMD) {
      switch(subtype) {
        case kSubtypeStreamFreq:
          mem->Write(key_stream_freq,         PunToUint32(stream_freq)); break;
        case kSubtypeStreamEnabled:
          mem->Write(key_stream_enabled,      stream_enabled); break;
        case kSubtypeTypeTrigger:
          mem->Write(key_type_trigger,        type_trigger); break;
        case kSubtypeSendMotorReport:
          mem->Write(key_send_motor_report,   send_motor_report); break;
        case kSubtypeSendBatteryReport:
          mem->Write(key_send_battery_report, send_battery_report); break;
        case kSubtypeSendTestStand:
          mem->Write(key_send_test_stand,     send_test_stand); break;
      }
    } // end if save
  } // end if kTypePollingReporter


  else if(type == kTypeRequestMessage) {
    MsgRequestMessage *rx_req = (MsgRequestMessage*) &rx_data[1];
    if(rx_req->msg_type == kTypePollingReporterParam) {
      SendMsgPollingReporterParam(com);
    }
  } // end if kTypeRequestMessage

}

void PollingReporter::Stream(CommunicationInterface &com)
{
  if(stream_enabled) {
    float period = 1/stream_freq;
    float t = GetSeconds();
    if(t > time_prev_report + period) {
      time_prev_report = t;
      if(send_motor_report) {
        motor->SendState(com);
      }
      if(send_battery_report) {
        battery->SendMsgBatteryReport(com);
      }
      if(send_test_stand) {
        SendMsgTestStand(com);
      }
    }
  }
}

void PollingReporter::SendMsgPollingReporterParam(CommunicationInterface &com) {
  MsgPollingReporterParam tx_msg;
  FillMsgPollingReporterParam(&tx_msg);
  com.SendPacket(kTypePollingReporterParam, (uint8_t*)&tx_msg, sizeof(MsgPollingReporterParam));
}

void PollingReporter::FillMsgPollingReporterParam(MsgPollingReporterParam *tx_msg) {
  tx_msg->stream_freq     = stream_freq;
  tx_msg->stream_enabled  = stream_enabled;
  tx_msg->type_trigger    = type_trigger;
  tx_msg->motor           = send_motor_report;
  tx_msg->battery         = send_battery_report;
  tx_msg->test_stand      = send_test_stand;
}

void PollingReporter::SendMsgTestStand(CommunicationInterface &com) {
  MsgTestStand tx_msg;
  FillMsgTestStand(&tx_msg);
  com.SendPacket(kTypeTestStand, (uint8_t*)&tx_msg, sizeof(MsgTestStand));
}

void PollingReporter::FillMsgTestStand(MsgTestStand *tx_msg) {
  tx_msg->t         = GetSeconds();
  tx_msg->volts     = battery->GetVoltsFilt();
  tx_msg->amps      = battery->GetAmpsFilt();
  tx_msg->watts     = battery->GetWattsFilt();
  tx_msg->joules    = battery->GetJoules();
  tx_msg->pwm       = motor->get_pwm();
  tx_msg->speed     = motor->get_speed();
  tx_msg->position  = motor->get_mechanical_angle();
}