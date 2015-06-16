#include "communication_interface.h"
#include "mpu_60XX_imu.h"

void imu_raw_msg_logger_init(Mpu60XXImu &imu);

void imu_raw_msg_logger_push();

void imu_raw_msg_logger_send(CommunicationInterface& com);