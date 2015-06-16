#include "common_message_types.h"
#include "battery_monitor_msgs.h"
#include "battery_monitor.h"
#include "math_helper.h"

#ifdef USING_BATTERY_MONITOR

const uint16_t key_volts_gain = (uint16_t)256*kTypeBatteryMonitor + kSubtypeVoltsGain;
const uint16_t key_amps_bias =   (uint16_t)256*kTypeBatteryMonitor + kSubtypeAmpsBias;
const uint16_t key_amps_gain =   (uint16_t)256*kTypeBatteryMonitor + kSubtypeAmpsGain;

BatteryMonitor::BatteryMonitor(PersistentMemory& memory) {
  // reserve memory
  mem = &memory;
  mem->AddKey(key_volts_gain);
  mem->AddKey(key_amps_bias);
  mem->AddKey(key_amps_gain);

  float temp;
  temp = 0.00030f;
    mem->Read(key_volts_gain, (uint32_t*)&temp);
    BattItSetVoltsGain(temp);
  temp = -34000.0f;
    mem->Read(key_amps_bias, (uint32_t*)&temp);
    BattItSetAmpsBias(temp);
  temp = 0.00095f;
    mem->Read(key_amps_gain, (uint32_t*)&temp);
    BattItSetAmpsGain(temp);

  InitBatteryMonitorHardware();
}

void BatteryMonitor::SendMsgBatteryReport(CommunicationInterface& com) {
  MsgBatteryReport tx_msg;
  BattItGetBatteryFilt(&tx_msg.volts, &tx_msg.amps, &tx_msg.watts, &tx_msg.joules);
  tx_msg.t = ToSeconds(GetTime());
  com.SendPacket(kTypeBatteryReport, (uint8_t*)&tx_msg, sizeof(MsgBatteryReport));
}

/*
uint8_t BatteryMonitor::ReadRegister(uint16_t reg, uint32_t &value) {
  uint8_t high_byte = reg<<8;
  uint8_t low_byte = reg & 0x00ff;
  if(high_byte == TYPE_BATTERY_MONITOR_VAL) {
    switch(low_byte) {
      case kSubtypeVoltsRaw:
        value = PunToUint32(BattItGetVoltsRaw()); return 1;
      case kSubtypeVolts:
        value = PunToUint32(BattItGetVolts()); return 1;
      case kSubtypeVoltsFilt:
        value = PunToUint32(BattItGetVoltsFilt()); return 1;
      case kSubtypeAmpsRaw:
        value = PunToUint32(BattItGetAmpsRaw()); return 1;
      case kSubtypeAmps:
        value = PunToUint32(BattItGetAmps()); return 1;
      case kSubtypeAmpsFilt:
        value = PunToUint32(BattItGetAmpsFilt()); return 1;
      case kSubtypeWatts:
        value = PunToUint32(BattItGetWatts()); return 1;
      case kSubtypeWattsFilt:
        value = PunToUint32(BattItGetWattsFilt()); return 1;
      case kSubtypeJoules:
        value = PunToUint32(BattItGetJoules()); return 1;
      case kSubtypeVoltsGain:
        value = PunToUint32(BattItGetVoltsGain()); return 1;
      case kSubtypeAmpsBias:
        value = PunToUint32(BattItGetAmpsBias()); return 1;
      case kSubtypeAmpsGain:
        value = PunToUint32(BattItGetAmpsGain()); return 1;
    }
  }
  return 0;
}
*/

void BatteryMonitor::ReadMsg(CommunicationInterface& com, uint8_t* rx_data, uint8_t rx_length) {
  uint8_t type = rx_data[0];

  if(type == kTypeRequestMessage) {
    MsgRequestMessage *data_in = (MsgRequestMessage*) &rx_data[1];
    uint8_t msg_type = data_in->msg_type;

    if(msg_type == kTypeBatteryReport) {
      SendMsgBatteryReport(com);
    }
  }
  else if (type == kTypeBatteryMonitor && rx_length >= 3) {

    uint8_t subtype = rx_data[1];
    uint8_t direction = rx_data[2];
    float ftemp = 0;
    int32_t itemp = 0;

    if(direction == GET_CMD) {
      uint8_t tx_msg[6];
      tx_msg[0] = subtype;
      tx_msg[1] = 3;
      switch(subtype) {
        case kSubtypeVoltsRaw:
          itemp = BattItGetVoltsRaw();
          memcpy(&tx_msg[2], &itemp, 4);
          com.SendPacket(kTypeBatteryMonitor, tx_msg, 6); break;
        case kSubtypeVolts:
          ftemp = BattItGetVolts();
          memcpy(&tx_msg[2], &ftemp, 4);
          com.SendPacket(kTypeBatteryMonitor, tx_msg, 6); break;
        case kSubtypeVoltsFilt:
          ftemp = BattItGetVoltsFilt();
          memcpy(&tx_msg[2], &ftemp, 4);
          com.SendPacket(kTypeBatteryMonitor, tx_msg, 6); break;
        case kSubtypeAmpsRaw:
          itemp = BattItGetAmpsRaw();
          memcpy(&tx_msg[2], &itemp, 4);
          com.SendPacket(kTypeBatteryMonitor, tx_msg, 6); break;
        case kSubtypeAmps:
          ftemp = BattItGetAmps();
          memcpy(&tx_msg[2], &ftemp, 4);
          com.SendPacket(kTypeBatteryMonitor, tx_msg, 6); break;
        case kSubtypeAmpsFilt:
          ftemp = BattItGetAmpsFilt();
          memcpy(&tx_msg[2], &ftemp, 4);
          com.SendPacket(kTypeBatteryMonitor, tx_msg, 6); break;
        case kSubtypeWatts:
          ftemp = BattItGetWatts();
          memcpy(&tx_msg[2], &ftemp, 4);
          com.SendPacket(kTypeBatteryMonitor, tx_msg, 6); break;
        case kSubtypeWattsFilt:
          ftemp = BattItGetWattsFilt();
          memcpy(&tx_msg[2], &ftemp, 4);
          com.SendPacket(kTypeBatteryMonitor, tx_msg, 6); break;
        case kSubtypeJoules:
          ftemp = BattItGetJoules();
          memcpy(&tx_msg[2], &ftemp, 4);
          com.SendPacket(kTypeBatteryMonitor, tx_msg, 6); break;
        case kSubtypeVoltsGain:
          ftemp = BattItGetVoltsGain();
          memcpy(&tx_msg[2], &ftemp, 4);
          com.SendPacket(kTypeBatteryMonitor, tx_msg, 6); break;
        case kSubtypeAmpsBias:
          ftemp = BattItGetAmpsBias();
          memcpy(&tx_msg[2], &ftemp, 4);
          com.SendPacket(kTypeBatteryMonitor, tx_msg, 6); break;
        case kSubtypeAmpsGain:
          ftemp = BattItGetAmpsGain();
          memcpy(&tx_msg[2], &ftemp, 4);
          com.SendPacket(kTypeBatteryMonitor, tx_msg, 6); break;
      }
    }
    else if(direction == SET_CMD) {
      switch(subtype) {
        case kSubtypeVoltsGain:
          ftemp = PunToFloat(&rx_data[3]);
          BattItSetVoltsGain(ftemp);        break;
        case kSubtypeAmpsBias:
          ftemp = PunToFloat(&rx_data[3]);
          BattItSetAmpsBias(ftemp);         break;
        case kSubtypeAmpsGain:
          ftemp = PunToFloat(&rx_data[3]);
          BattItSetAmpsGain(ftemp);         break;
        case kSubtypeVoltsCalibrate:
          ftemp = PunToFloat(&rx_data[3]);
          BattItCalculateVoltsGain(ftemp);  break;
        case kSubtypeResetJoules:
          BattItResetJoules();              break;
      }
    }
    else if(direction == SAVE_CMD) {
      switch(subtype) {
        case kSubtypeVoltsGain:
          ftemp = BattItGetVoltsGain();
          mem->Write(key_volts_gain, PunToUint32(ftemp)); break;
        case kSubtypeAmpsBias:
          ftemp = BattItGetAmpsBias();
          mem->Write(key_amps_bias, PunToUint32(ftemp));   break;
        case kSubtypeAmpsGain:
          ftemp = BattItGetAmpsGain();
          mem->Write(key_amps_gain, PunToUint32(ftemp));   break;
      }
    }
  }
}

#endif // USING_BATTERY_MONITOR