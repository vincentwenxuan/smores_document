#include "project_message_types.h"
#include "uav_reporter.h"
#include "uav_reporter_msgs.h"

// persistent memory key values (based on component type and subtypes)
const uint16_t key_send_roll_report             = (uint16_t)256*kTypeUavReporter + kSubtypeSendRollReport;
const uint16_t key_send_yaw_report              = (uint16_t)256*kTypeUavReporter + kSubtypeSendYawReport;
const uint16_t key_send_control_calc            = (uint16_t)256*kTypeUavReporter + kSubtypeSendControlCalc;
const uint16_t key_send_control_est             = (uint16_t)256*kTypeUavReporter + kSubtypeSendControlEst;
const uint16_t key_send_imu_raw_report          = (uint16_t)256*kTypeUavReporter + kSubtypeSendImuRawReport;
const uint16_t key_send_imu_metric_report       = (uint16_t)256*kTypeUavReporter + kSubtypeSendImuMetricReport;
const uint16_t key_send_est_quat_report         = (uint16_t)256*kTypeUavReporter + kSubtypeSendEstQuatReport;
const uint16_t key_send_motor_report            = (uint16_t)256*kTypeUavReporter + kSubtypeSendMotorReport;
const uint16_t key_send_battery_report          = (uint16_t)256*kTypeUavReporter + kSubtypeSendBatteryReport;
const uint16_t key_send_loop_profile            = (uint16_t)256*kTypeUavReporter + kSubtypeSendLoopProfile;
const uint16_t key_send_loop_freq               = (uint16_t)256*kTypeUavReporter + kSubtypeSendLoopFreq;
const uint16_t key_send_estimator               = (uint16_t)256*kTypeUavReporter + kSubtypeSendEstimator;
const uint16_t key_send_efficiency              = (uint16_t)256*kTypeUavReporter + kSubtypeSendEfficiency;
const uint16_t key_send_open_attitude_debug     = (uint16_t)256*kTypeUavReporter + kSubtypeSendOpenAttitudeDebug;


UavReporter::UavReporter( PersistentMemory            &log_mem,
                          Mpu60XXImu                  &log_imu,
                          GyroAccelDriftUkfIo         &log_est,
                          MotorHal                    &log_motor_hal,
                          BatteryMonitor              &log_battery,
                          PulsingCoaxControllerQuat   &log_uav,
                          CoaxOpenAttitudeController  &log_open_attitude,
                          QuatPD                      &log_pd,
                          LoopMonitor                 &log_loop,
                          SlaveMasterTransferStruct   &log_bottom_data):
send_roll_report(0),
send_yaw_report(0),
send_control_calc(0),
send_control_est(0),
send_imu_raw_report(0),
send_imu_metric_report(0),
send_est_quat_report(0),
send_motor_report(0),
send_battery_report(0),
send_loop_profile(0),
send_loop_freq(0),
send_estimator(0),
send_efficiency(0),
send_open_attitude_debug(0)
{
  // store pointers to reported devices
  mem           = &log_mem;
  imu           = &log_imu;
  est           = &log_est;
  motor_hal     = &log_motor_hal;
  battery       = &log_battery;
  uav           = &log_uav;
  open_attitude = &log_open_attitude;
  pd            = &log_pd;
  loop          = &log_loop;
  bottom_data = &log_bottom_data;

  // request persistent memory service
  mem->AddKey(key_send_roll_report);
  mem->AddKey(key_send_yaw_report);
  mem->AddKey(key_send_control_calc);
  mem->AddKey(key_send_control_est);
  mem->AddKey(key_send_imu_raw_report);
  mem->AddKey(key_send_imu_metric_report);
  mem->AddKey(key_send_est_quat_report);
  mem->AddKey(key_send_motor_report);
  mem->AddKey(key_send_battery_report);
  mem->AddKey(key_send_loop_profile);
  mem->AddKey(key_send_loop_freq);
  mem->AddKey(key_send_estimator);
  mem->AddKey(key_send_efficiency);
  mem->AddKey(key_send_open_attitude_debug);

  // init from memory
  mem->Read(key_send_roll_report,           (uint32_t*)&send_roll_report);
  mem->Read(key_send_yaw_report,            (uint32_t*)&send_yaw_report);
  mem->Read(key_send_control_calc,          (uint32_t*)&send_control_calc);
  mem->Read(key_send_control_est,           (uint32_t*)&send_control_est);
  mem->Read(key_send_imu_raw_report,        (uint32_t*)&send_imu_raw_report);
  mem->Read(key_send_imu_metric_report,     (uint32_t*)&send_imu_metric_report);
  mem->Read(key_send_est_quat_report,       (uint32_t*)&send_est_quat_report);
  mem->Read(key_send_motor_report,          (uint32_t*)&send_motor_report);
  mem->Read(key_send_battery_report,        (uint32_t*)&send_battery_report);
  mem->Read(key_send_loop_profile,          (uint32_t*)&send_loop_profile);
  mem->Read(key_send_loop_freq,             (uint32_t*)&send_loop_freq);
  mem->Read(key_send_estimator,             (uint32_t*)&send_estimator);
  mem->Read(key_send_efficiency,            (uint32_t*)&send_efficiency);
  mem->Read(key_send_open_attitude_debug,   (uint32_t*)&send_open_attitude_debug);
}


void UavReporter::ReadMsg(CommunicationInterface &com, uint8_t *rx_data, uint8_t rx_length)
 {
  uint8_t type = rx_data[0];
  if(type == kTypeQuatPilot || type == kTypeQuatFullObsPilot || type == kTypeOpenPilot || type == kTypeOpenAttitudePilot || type == kTypeUavReporterRequest)
  {
    if(send_roll_report) {                            // 41 bytes
      uav->SendMsgRollReport(com);
    }

    if(send_yaw_report) {
      uav->SendMsgYawReport(com);
    }

    if(send_control_calc) {
      uav->SendMsgControlCalc(com);
    }

    if(send_control_est) {
      SendMsgControlEst(com);
    }

    if(send_imu_raw_report) {
      imu->SendRaw(com);
    }

    if(send_imu_metric_report) {
      imu->SendMetric(com);
    }

    if(send_est_quat_report) {
      est->SendEstQuat(com);
    }

    if(send_motor_report) {
      motor_hal->SendMsgMotorHalReport(com);
    }

    if(send_battery_report) {
      SendMsgUavReporterPower(com);
    }

    if(send_loop_profile) {
      loop->SendProfile(com);
    }

    if(send_loop_freq) {
      loop->SendFrequency(com);
    }

    if(send_estimator) {
      SendMsgUavReporterEstimator(com);
    }

    if(send_efficiency) {
      SendMsgEfficiency(com);
    }

    if(send_open_attitude_debug) {
      open_attitude->SendMsgDebug(com);
    }
  } // end if pilot message types


  else if(type == kTypeUavReporter && rx_length >= 3)
  {
    uint8_t subtype = rx_data[1];
    uint8_t direction = rx_data[2];

    if(direction == GET_CMD) {
      uint8_t tx_msg[6];
      tx_msg[0] = subtype;
      tx_msg[1] = RESPONSE_CMD;
      switch(subtype) {
        case kSubtypeSendRollReport:
          tx_msg[2] = send_roll_report;
          com.SendPacket(kTypeUavReporter, tx_msg, 3); break;
        case kSubtypeSendYawReport:
          tx_msg[2] = send_yaw_report;
          com.SendPacket(kTypeUavReporter, tx_msg, 3); break;
        case kSubtypeSendControlCalc:
          tx_msg[2] = send_control_calc;
          com.SendPacket(kTypeUavReporter, tx_msg, 3); break;
        case kSubtypeSendControlEst:
          tx_msg[2] = send_control_est;
          com.SendPacket(kTypeUavReporter, tx_msg, 3); break;
        case kSubtypeSendImuRawReport:
          tx_msg[2] = send_imu_raw_report;
          com.SendPacket(kTypeUavReporter, tx_msg, 3); break;
        case kSubtypeSendImuMetricReport:
          tx_msg[2] = send_imu_metric_report;
          com.SendPacket(kTypeUavReporter, tx_msg, 3); break;
        case kSubtypeSendEstQuatReport:
          tx_msg[2] = send_est_quat_report;
          com.SendPacket(kTypeUavReporter, tx_msg, 3); break;
        case kSubtypeSendMotorReport:
          tx_msg[2] = send_motor_report;
          com.SendPacket(kTypeUavReporter, tx_msg, 3); break;
        case kSubtypeSendBatteryReport:
          tx_msg[2] = send_battery_report;
          com.SendPacket(kTypeUavReporter, tx_msg, 3); break;
        case kSubtypeSendLoopProfile:
          tx_msg[2] = send_loop_profile;
          com.SendPacket(kTypeUavReporter, tx_msg, 3); break;
        case kSubtypeSendLoopFreq:
          tx_msg[2] = send_loop_freq;
          com.SendPacket(kTypeUavReporter, tx_msg, 3); break;
        case kSubtypeSendEstimator:
          tx_msg[2] = send_estimator;
          com.SendPacket(kTypeUavReporter, tx_msg, 3); break;
        case kSubtypeSendEfficiency:
          tx_msg[2] = send_efficiency;
          com.SendPacket(kTypeUavReporter, tx_msg, 3); break;
        case kSubtypeSendOpenAttitudeDebug:
          tx_msg[2] = send_open_attitude_debug;
          com.SendPacket(kTypeUavReporter, tx_msg, 3); break;
      }
    } // end if get

    else if(direction == SET_CMD) {
      switch(subtype) {
        case kSubtypeSendRollReport:
          send_roll_report = rx_data[3]; break;
        case kSubtypeSendYawReport:
          send_yaw_report = rx_data[3]; break;
        case kSubtypeSendControlCalc:
          send_control_calc = rx_data[3]; break;
        case kSubtypeSendControlEst:
          send_control_est = rx_data[3]; break;
        case kSubtypeSendImuRawReport:
          send_imu_raw_report = rx_data[3]; break;
        case kSubtypeSendImuMetricReport:
          send_imu_metric_report = rx_data[3]; break;
        case kSubtypeSendEstQuatReport:
          send_est_quat_report = rx_data[3]; break;
        case kSubtypeSendMotorReport:
          send_motor_report = rx_data[3]; break;
        case kSubtypeSendBatteryReport:
          send_battery_report = rx_data[3]; break;
        case kSubtypeSendLoopProfile:
          send_loop_profile = rx_data[3]; break;
        case kSubtypeSendLoopFreq:
          send_loop_freq = rx_data[3]; break;
        case kSubtypeSendEstimator:
          send_estimator = rx_data[3]; break;
        case kSubtypeSendEfficiency:
          send_efficiency = rx_data[3]; break;
        case kSubtypeSendOpenAttitudeDebug:
          send_open_attitude_debug = rx_data[3]; break;
      }
    } // end if set

    else if(direction == SAVE_CMD) {
      switch(subtype) {
        case kSubtypeSendRollReport:
          mem->Write(key_send_roll_report, send_roll_report); break;
        case kSubtypeSendYawReport:
          mem->Write(key_send_yaw_report, send_yaw_report); break;
        case kSubtypeSendControlCalc:
          mem->Write(key_send_control_calc, send_control_calc); break;
        case kSubtypeSendControlEst:
          mem->Write(key_send_control_est, send_control_est); break;
        case kSubtypeSendImuRawReport:
          mem->Write(key_send_imu_raw_report, send_imu_raw_report); break;
        case kSubtypeSendImuMetricReport:
          mem->Write(key_send_imu_metric_report, send_imu_metric_report); break;
        case kSubtypeSendEstQuatReport:
          mem->Write(key_send_est_quat_report, send_est_quat_report); break;
        case kSubtypeSendMotorReport:
          mem->Write(key_send_motor_report, send_motor_report); break;
        case kSubtypeSendBatteryReport:
          mem->Write(key_send_battery_report, send_battery_report); break;
        case kSubtypeSendLoopProfile:
          mem->Write(key_send_loop_profile, send_loop_profile); break;
        case kSubtypeSendLoopFreq:
          mem->Write(key_send_loop_freq, send_loop_freq); break;
        case kSubtypeSendEstimator:
          mem->Write(key_send_estimator, send_estimator); break;
        case kSubtypeSendEfficiency:
          mem->Write(key_send_efficiency, send_efficiency); break;
        case kSubtypeSendOpenAttitudeDebug:
          mem->Write(key_send_open_attitude_debug, send_open_attitude_debug); break;
      }
    } // end if save
  } // end if kTypeUavReporter


  else if(type == kTypeRequestMessage) {
    MsgRequestMessage *rx_req = (MsgRequestMessage*) &rx_data[1];
    if(rx_req->msg_type == kTypeUavReporterParam) {
      SendMsgUavReporterParam(com);
    }
  } // end if kTypeRequestMessage

}


////////////////////////////////////////////////////////////////////////////////

void UavReporter::SendMsgUavReporterParam(CommunicationInterface &com) {
  MsgUavReporterParam tx_msg;
  FillMsgUavReporterParam(&tx_msg);
  com.SendPacket(kTypeUavReporterParam, (uint8_t*)&tx_msg, sizeof(MsgUavReporterParam));
}


void UavReporter::FillMsgUavReporterParam(MsgUavReporterParam *tx_msg) {
  tx_msg->roll                = send_roll_report;
  tx_msg->yaw                 = send_yaw_report;
  tx_msg->control_calc        = send_control_calc;
  tx_msg->control_est         = send_control_est;
  tx_msg->imu_raw             = send_imu_raw_report;
  tx_msg->imu_metric          = send_imu_metric_report;
  tx_msg->est_quat            = send_est_quat_report;
  tx_msg->motor               = send_motor_report;
  tx_msg->battery             = send_battery_report;
  tx_msg->loop_profile        = send_loop_profile;
  tx_msg->loop_freq           = send_loop_freq;
  tx_msg->estimator           = send_estimator;
  tx_msg->efficiency          = send_efficiency;
  tx_msg->open_attitude_debug = send_open_attitude_debug;
}


////////////////////////////////////////////////////////////////////////////////

void UavReporter::SendMsgUavReporterPower(CommunicationInterface &com) {
  MsgUavReporterPower tx_msg;
  FillMsgUavReporterPower(&tx_msg);
  com.SendPacket(kTypeUavReporterPower, (uint8_t*)&tx_msg, sizeof(MsgUavReporterPower));
}


void UavReporter::FillMsgUavReporterPower(MsgUavReporterPower *tx_msg) {
  tx_msg->t             = GetSeconds();
  tx_msg->top_volts     = battery->GetVoltsFilt();
  tx_msg->top_amps      = battery->GetAmpsFilt();
  tx_msg->top_joules    = battery->GetJoules();
  tx_msg->bottom_volts  = bottom_data->volts;
  tx_msg->bottom_amps   = bottom_data->amps;
  tx_msg->bottom_joules = bottom_data->joules;
}


////////////////////////////////////////////////////////////////////////////////

void UavReporter::SendMsgUavReporterEstimator(CommunicationInterface &com) {
  MsgUavReporterEstimator tx_msg;
  FillMsgUavReporterEstimator(&tx_msg);
  com.SendPacket(kTypeUavReporterEstimator, (uint8_t*)&tx_msg, sizeof(MsgUavReporterEstimator));
}


void UavReporter::FillMsgUavReporterEstimator(MsgUavReporterEstimator *tx_msg) {
  Quaternionf q;  q = est->ukf.get_q();
  Vector3f w;     w = est->ukf.get_w();

  tx_msg->t             = ToSeconds(imu->time);
  tx_msg->wx_meas       = imu->w[0];
  tx_msg->wy_meas       = imu->w[1];
  tx_msg->wz_meas       = imu->w[2];
  tx_msg->ax_meas       = imu->a[0];
  tx_msg->ay_meas       = imu->a[1];
  tx_msg->az_meas       = imu->a[2];
  tx_msg->wx_est        = w(0);
  tx_msg->wy_est        = w(1);
  tx_msg->wz_est        = w(2);
  tx_msg->qw_est        = q.w();
  tx_msg->qx_est        = q.x();
  tx_msg->qy_est        = q.y();
  tx_msg->qz_est        = q.z();
}


////////////////////////////////////////////////////////////////////////////////

void UavReporter::SendMsgControlEst(CommunicationInterface& com) {
  MsgControlEst tx_msg;
  FillMsgControlEst(&tx_msg);
  com.SendPacket(kTypeControlEst, (uint8_t*)&tx_msg, sizeof(MsgControlEst));
}

void UavReporter::FillMsgControlEst(MsgControlEst* tx_msg) {
  Quaternionf q;  q = est->ukf.get_q();
  Vector3f w;     w = est->ukf.get_w();

  tx_msg->t       = GetSeconds();
  tx_msg->wx_est  = w(0);
  tx_msg->wy_est  = w(1);
  tx_msg->wz_est  = w(2);
  tx_msg->qw_est        = q.w();
  tx_msg->qx_est        = q.x();
  tx_msg->qy_est        = q.y();
  tx_msg->qz_est        = q.z();
  tx_msg->wx_filt   = pd->w_filt[0];
  tx_msg->wy_filt   = pd->w_filt[1];
  tx_msg->wz_filt   = pd->w_filt[2];
  tx_msg->top_mean      = uav->top_mean;
  tx_msg->bottom_mean   = uav->bottom_mean;
  tx_msg->top_volts     = battery->GetVoltsFilt();
}


////////////////////////////////////////////////////////////////////////////////

void UavReporter::SendMsgEfficiency(CommunicationInterface& com) {
  MsgEfficiency tx_msg;
  FillMsgEfficiency(&tx_msg);
  com.SendPacket(kTypeEfficiency, (uint8_t*)&tx_msg, sizeof(MsgEfficiency));
}

void UavReporter::FillMsgEfficiency(MsgEfficiency* tx_msg) {
  Quaternionf q;  q = est->ukf.get_q();

  tx_msg->t             = GetSeconds();
  tx_msg->top_volts     = battery->GetVoltsFilt();
  tx_msg->top_amps      = battery->GetAmpsFilt();
  tx_msg->top_joules    = battery->GetJoules();
  tx_msg->top_mean      = motor_hal->get_top_cmd_volts();
  tx_msg->top_speed     = motor_hal->get_top_speed();
  tx_msg->bottom_volts  = bottom_data->volts;
  tx_msg->bottom_amps   = bottom_data->amps;
  tx_msg->bottom_joules = bottom_data->joules;
  tx_msg->bottom_mean   = motor_hal->get_bottom_cmd_volts();
  tx_msg->bottom_speed  = motor_hal->get_bottom_speed();

  // send quaternion with an implied positive w component
  if(q.w() >= 0) {
    tx_msg->qx_est  = q.x();
    tx_msg->qy_est  = q.y();
    tx_msg->qz_est  = q.z();
  }
  else {
    tx_msg->qx_est  = -q.x();
    tx_msg->qy_est  = -q.y();
    tx_msg->qz_est  = -q.z();
  }
}