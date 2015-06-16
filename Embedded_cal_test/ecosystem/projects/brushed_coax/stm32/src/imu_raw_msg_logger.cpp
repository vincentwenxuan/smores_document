#include "imu_raw_msg_logger.hpp"
#include "common_message_types.h"
#include "CBuf.h"

#define imu_raw_msg_logger_SIZE 8
#define LENGTH_LIMIT  5

extern "C" {
  volatile struct
  {
    uint8_t           m_getIdx;
    uint8_t           m_putIdx;
    MsgImuRaw         m_entry[imu_raw_msg_logger_SIZE];
  } imu_raw_msg_logger;
}

Mpu60XXImu *imu_ptr;

// verify size is a power of 2
#define IMU_RAW_MSG_LOGGER_SIZE_MASK (imu_raw_msg_logger_SIZE - 1)
#if (imu_raw_msg_logger_SIZE & IMU_RAW_MSG_LOGGER_SIZE_MASK)
  #error "Require imu_raw_msg_logger_SIZE to be power of 2."
#endif

// verify length fits in size
#if (LENGTH_LIMIT > imu_raw_msg_logger_SIZE)
  #error "Require LENGTH_LIMIT <= imu_raw_msg_logger_SIZE."
#endif


void imu_raw_msg_logger_init(Mpu60XXImu &imu) {
  CBUF_Init(imu_raw_msg_logger);
  imu_ptr = &imu;
}

void imu_raw_msg_logger_push() {
  // if max entries already stored, discard oldest
  if(CBUF_Len(imu_raw_msg_logger) >= LENGTH_LIMIT) {
    CBUF_AdvancePopIdx(imu_raw_msg_logger);
  }
  // push by copying directly into the buffer
  MsgImuRaw* push_ptr = const_cast<MsgImuRaw*>(CBUF_GetPushEntryPtr(imu_raw_msg_logger));
  imu_ptr->FillMsgImuRaw(push_ptr);
  CBUF_AdvancePushIdx(imu_raw_msg_logger);
}

void imu_raw_msg_logger_send(CommunicationInterface& com) {
  // empty the buffer into the com 
  while(!CBUF_IsEmpty(imu_raw_msg_logger)) {
    // pop by copying directly out of the buffer
    MsgImuRaw* pop_ptr = const_cast<MsgImuRaw*>(CBUF_GetPopEntryPtr(imu_raw_msg_logger));
    com.SendPacket(kTypeImuRaw, (uint8_t*)pop_ptr, sizeof(MsgImuRaw));
    CBUF_AdvancePopIdx(imu_raw_msg_logger);
  }
}