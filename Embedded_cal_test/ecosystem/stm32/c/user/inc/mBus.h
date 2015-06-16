//-----------------------------------------------------------------------------
// MAEVARM M4 STM32F373 mBus
// version: 1.0
// date: March 22, 2013
// author: Neel Shah (neels@seas.upenn.edu)
//
// edited: Matthew Piccoli (piccoli@seas.upenn.edu)
// date: 5/27/2013
// Adding support for multiple mBuses
//
// edited: Jimmy Paulos (jpaulos@seas.upenn.edu)
// date: 5/06/2014
// Added support for Slave and Listen modes.
//
// Project Status:
//    Single Master and Dual Master modes are well tested.  Single Listen mode 
//    is functional in a unit test and support 32-bit pseudo-register access.
//
// Future Work:
//    Protect rapid successive calls to nonblocking functions from interfering 
//    with eachother.  Make better use of CPAL state machine, instead of grafting 
//    on extra state.
//-----------------------------------------------------------------------------

#ifndef __m_bus
#define __m_bus

#include "common_peripherals.h"
#include STM32FXXX_I2C_CPAL_H
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MBUS_BUFF_SIZE 256

// Multiple mBusTypeDef structs will be instatiated to maintain the state for 
// multiple I2C devices (I2C1 and I2C2).
typedef struct
{
  CPAL_InitTypeDef* CPAL_InitStruct; // pointer to either I2C1_DevStructure or I2C2_DevStructure
  CPAL_TransferTypeDef mBusRx;
  CPAL_TransferTypeDef mBusTx;
  uint8_t bufferWr[2];
  uint8_t bufferRd[2];
  uint8_t bufferBurstWr[MBUS_BUFF_SIZE];
  uint8_t bufferBurstRd[MBUS_BUFF_SIZE];
  __IO uint8_t mBusReadBurstReady;
  __IO uint8_t mBusReadBurstStartFlag;
  uint8_t prev_length_slave_read;
  uint8_t next_length_slave_read;
  uint8_t has_reg_addr;
  uint16_t reg_addr;
  uint8_t byte_count;
  uint32_t timing;
} mBusTypeDef;

mBusTypeDef* getmBusPtr(uint8_t i2cNum);
void mBusInit(uint8_t i2cNum);
void mBusInitTiming(uint8_t i2cNum, uint32_t timing);
void mBusRestart(uint8_t i2cNum);

uint8_t mBusPeriphReady(uint8_t slaveAddr, uint8_t i2cNum);

#if defined (USING_MBUS_MASTER_MODE) || !defined (USING_MBUS_LISTEN_MODE)
  uint8_t mBusWrite(uint8_t slaveAddr, uint8_t regAddr, uint8_t data, uint8_t i2cNum);
  uint8_t mBusRead (uint8_t slaveAddr, uint8_t regAddr, uint8_t i2cNum);

  uint8_t mBusWriteBurst(uint8_t slaveAddr, uint8_t regAddr, uint8_t length, uint8_t* data, uint8_t i2cNum);
  uint8_t mBusReadBurst (uint8_t slaveAddr, uint8_t regAddr, uint8_t length, uint8_t* data, uint8_t i2cNum);

  uint8_t mBusWriteNoAdd(uint8_t slaveAddr, uint8_t data, uint8_t i2cNum);
  uint8_t mBusReadNoAdd (uint8_t slaveAddr, uint8_t i2cNum);

  uint8_t mBusWriteBurstNoAdd(uint8_t slaveAddr, uint8_t length, uint8_t* data, uint8_t i2cNum);
  uint8_t mBusReadBurstNoAdd (uint8_t slaveAddr, uint8_t length, uint8_t* data, uint8_t i2cNum);

  //Non Blocking functions, user takes care of waiting and other stuff 
  uint8_t mBusWriteBurstNB(uint8_t slaveAddr, uint8_t regAddr, uint8_t length, uint8_t* data, uint8_t i2cNum);
  uint8_t mBusReadBurstStartNB(uint8_t slaveAddr, uint8_t regAddr, uint8_t length, uint8_t i2cNum);
  uint8_t mBusReadBurstDataNB (uint8_t length, uint8_t* data, uint8_t i2cNum);

  uint8_t mBusMasterWrite32(uint8_t slaveAddr, uint16_t regAddr, uint32_t value, uint8_t i2cNum);
  uint8_t mBusMasterWriteBurst(uint8_t slaveAddr, uint16_t regAddr, uint8_t length, uint8_t *data, uint8_t i2cNum);
  uint8_t mBusMasterReadBurstStart(uint8_t slaveAddr, uint16_t regAddr, uint8_t length, uint8_t i2cNum);

#endif


#if defined USING_MBUS_LISTEN_MODE
  extern void SlaveRx_UserCallback(uint16_t reg_addr, uint32_t value);
  extern uint32_t SlaveTx_UserCallback(uint16_t reg_addr);

  void mBusSlaveInit(uint8_t i2cNum);
  uint8_t mBusSlaveListen(uint8_t i2cNum);
#endif



void CPAL_I2C_ERR_UserCallback(CPAL_DevTypeDef pDevInstance, uint32_t DeviceError);
uint32_t CPAL_TIMEOUT_UserCallback(CPAL_InitTypeDef* pDevInitStruct);
void CPAL_I2C_TXTC_UserCallback(CPAL_InitTypeDef* pDevInitStruct);
void CPAL_I2C_RXTC_UserCallback(CPAL_InitTypeDef* pDevInitStruct);
//void CPAL_I2C_TX_UserCallback(CPAL_InitTypeDef* pDevInitStruct);
//void CPAL_I2C_RX_UserCallback(CPAL_InitTypeDef* pDevInitStruct);
//void CPAL_I2C_DMATXTC_UserCallback(CPAL_InitTypeDef* pDevInitStruct);
//void CPAL_I2C_DMATXHT_UserCallback(CPAL_InitTypeDef* pDevInitStruct);
//void CPAL_I2C_DMATXTE_UserCallback(CPAL_InitTypeDef* pDevInitStruct);
void CPAL_I2C_DMARXTC_UserCallback(CPAL_InitTypeDef* pDevInitStruct);
//void CPAL_I2C_DMARXHT_UserCallback(CPAL_InitTypeDef* pDevInitStruct);
//void CPAL_I2C_DMARXTE_UserCallback(CPAL_InitTypeDef* pDevInitStruct);

#ifdef CPAL_I2C_SLAVE_MODE
  void CPAL_I2C_SLAVE_READ_UserCallback(CPAL_InitTypeDef* pDevInitStruct); // this
  void CPAL_I2C_SLAVE_WRITE_UserCallback(CPAL_InitTypeDef* pDevInitStruct); // this
#endif

#ifdef __cplusplus
}
#endif // __cplusplus

#endif