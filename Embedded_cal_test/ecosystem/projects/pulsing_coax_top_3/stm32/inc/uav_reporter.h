#ifndef UAV_REPORTER_H
#define UAV_REPORTER_H

#include "communication_interface.h"
#include "persistent_memory.h"
#include "mpu_60XX_imu.h"
#include "gyro_accel_drift_ukf_io.h"
#include "brushless_controller.h"
#include "motor_hal.hpp"
#include "battery_monitor.h"
#include "pulsing_coax_controller_quat.h"
#include "coax_open_attitude_controller.hpp"
#include "quat_pd.h"
#include "loop_monitor.h"
#include "struct_definitions.h"
#include "stdint.h"

class UavReporter {
  public:

    UavReporter(PersistentMemory            &mem,
                Mpu60XXImu                  &imu,
                GyroAccelDriftUkfIo         &est,
                MotorHal                    &motor_hal,
                BatteryMonitor              &battery,
                PulsingCoaxControllerQuat   &uav,
                CoaxOpenAttitudeController  &open_attitude,
                QuatPD                      &pd,
                LoopMonitor                 &loop,
                SlaveMasterTransferStruct   &log_bottom_data);

    void ReadMsg(CommunicationInterface &com, uint8_t *rx_data, uint8_t rx_length);

  private:
    PersistentMemory            *mem;
    Mpu60XXImu                  *imu;
    GyroAccelDriftUkfIo         *est;
    MotorHal                    *motor_hal;
    BatteryMonitor              *battery;
    PulsingCoaxControllerQuat   *uav;
    CoaxOpenAttitudeController  *open_attitude;
    QuatPD                      *pd;
    LoopMonitor                 *loop;
    SlaveMasterTransferStruct   *bottom_data;

    uint8_t send_roll_report;
    uint8_t send_yaw_report;

    uint8_t send_control_calc;
    uint8_t send_control_est;

    uint8_t send_imu_raw_report;
    uint8_t send_imu_metric_report;
    uint8_t send_est_quat_report;

    uint8_t send_motor_report;
    uint8_t send_battery_report;

    uint8_t send_loop_profile;
    uint8_t send_loop_freq;

    uint8_t send_estimator;

    uint8_t send_efficiency;

    uint8_t send_open_attitude_debug;

    void SendMsgUavReporterParam(CommunicationInterface &com);
    void FillMsgUavReporterParam(MsgUavReporterParam *tx_msg);

    void SendMsgUavReporterPower(CommunicationInterface &com);
    void FillMsgUavReporterPower(MsgUavReporterPower *tx_msg);

    void SendMsgUavReporterEstimator(CommunicationInterface &com);
    void FillMsgUavReporterEstimator(MsgUavReporterEstimator *tx_msg);

    void FillMsgControlEst(MsgControlEst* tx_msg);
    void SendMsgControlEst(CommunicationInterface& com);

    void SendMsgEfficiency(CommunicationInterface& com);
    void FillMsgEfficiency(MsgEfficiency* tx_msg);
};

#endif // UAV_REPORTER_H