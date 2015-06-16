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
// date: 1/08/2014
// Added support for Slave and Listen modes.
//
// See header file for notes.
//-----------------------------------------------------------------------------

#include "common_peripherals.h"
#include "mBus.h"
#include "error_reporting.h"
#include "string.h" // for memcpy

//FastMode+ -  1 MHz speed (Rise time = 26ns, Fall time = 2ns, Analog Filter OFF)
//FastMode - 400 KHz speed (Rise time = 100ns, Fall time = 10ns, Analog Filter ON)
//Standard - 100 KHz speed (Rise time = 100ns, Fall time = 10ns, Analog Filter ON)
#define I2C_Timing_FastModePlus  0x00C71030
#define I2C_Timing_FastMode      0x6043070E
#define I2C_Timing_Standard      0x60C3283A

#ifdef USING_MBUS1
mBusTypeDef mBus1;
#endif

#ifdef USING_MBUS2
mBusTypeDef mBus2;
#endif


// declared in stm32fxxx_i2c_cpal_hal.c
extern I2C_TypeDef* CPAL_I2C_DEVICE[];


mBusTypeDef* getmBusPtr(uint8_t i2cNum)
{
  #if defined(USING_MBUS1) || defined(USING_MBUS2)
    #if defined(USING_MBUS1) && !defined(USING_MBUS2)
      return &mBus1;
    #elif defined(USING_MBUS2) && !defined(USING_MBUS1)
      return &mBus2;
    #else
      if (i2cNum == 1)
        return &mBus1;
      else //if (i2cNum == 2)
        return &mBus2;
    #endif
  #else
    return pNULL;
  #endif
}

// Return the mBusTypeDef struct pointer associated with the pDevInitStruct given to a callback function.
mBusTypeDef* getmBusPtrFromCallback(CPAL_InitTypeDef* pDevInitStruct) {
  mBusTypeDef* ptr = pNULL;
  if (pDevInitStruct->CPAL_Dev == CPAL_I2C1) {
    #ifdef USING_MBUS1
    ptr = &mBus1;
    #endif
  }
  else if (pDevInitStruct->CPAL_Dev == CPAL_I2C2) {
    #ifdef USING_MBUS2
    ptr = &mBus2;
    #endif
  }
  return ptr;
}

void mBusInitTiming(uint8_t i2cNum, uint32_t timing)
{
  #if defined(USING_MBUS1) || defined(USING_MBUS2)
  mBusTypeDef* mBusCurrent = getmBusPtr(i2cNum);
  if(i2cNum == 1)
  {
    #if defined(USING_MBUS1)
    RCC_I2CCLKConfig(RCC_I2C1CLK_SYSCLK);
    mBusCurrent->CPAL_InitStruct = &I2C1_DevStructure;
    CPAL_I2C_StructInit(mBusCurrent->CPAL_InitStruct);
    mBusCurrent->CPAL_InitStruct->CPAL_Dev = CPAL_I2C1; // what's this for?
    #endif
  }
  else //if(i2cNum == 2)
  {
    #if defined(USING_MBUS2)
    RCC_I2CCLKConfig(RCC_I2C2CLK_SYSCLK);
    mBusCurrent->CPAL_InitStruct = &I2C2_DevStructure;
    CPAL_I2C_StructInit(mBusCurrent->CPAL_InitStruct);
    mBusCurrent->CPAL_InitStruct->CPAL_Dev = CPAL_I2C2; // what's this for?
    #endif
  }
  mBusCurrent->timing = timing;

  // RX Transfer Structure
  mBusCurrent->mBusRx.pbBuffer = pNULL; //buffer pointer data
  mBusCurrent->mBusRx.wNumData = 0;     //length of data
  mBusCurrent->mBusRx.wAddr1   = 0;     //slaveAddr
  mBusCurrent->mBusRx.wAddr2   = 0;     //regAddr

  // TX Transfer Structure
  mBusCurrent->mBusTx.pbBuffer = pNULL; //buffer pointer data
  mBusCurrent->mBusTx.wNumData =  0;    //length of data
  mBusCurrent->mBusTx.wAddr1   = 0;     //slaveAddr
  mBusCurrent->mBusTx.wAddr2   = 0;     //regAddr
      
  mBusCurrent->CPAL_InitStruct->CPAL_Direction = CPAL_DIRECTION_TXRX;
  mBusCurrent->CPAL_InitStruct->CPAL_Mode = CPAL_MODE_MASTER;
  mBusCurrent->CPAL_InitStruct->CPAL_ProgModel = CPAL_PROGMODEL_DMA;
  mBusCurrent->CPAL_InitStruct->pCPAL_TransferTx = &(mBusCurrent->mBusTx);
  mBusCurrent->CPAL_InitStruct->pCPAL_TransferRx = &(mBusCurrent->mBusRx);
  mBusCurrent->CPAL_InitStruct->wCPAL_Options = CPAL_OPT_I2C_AUTOMATIC_END;

  mBusCurrent->CPAL_InitStruct->pCPAL_I2C_Struct->I2C_Timing = timing;
  mBusCurrent->CPAL_InitStruct->pCPAL_I2C_Struct->I2C_AnalogFilter = I2C_AnalogFilter_Enable;
  mBusCurrent->CPAL_InitStruct->pCPAL_I2C_Struct->I2C_DigitalFilter = 0x00;
  mBusCurrent->CPAL_InitStruct->pCPAL_I2C_Struct->I2C_Mode = I2C_Mode_I2C;
  mBusCurrent->CPAL_InitStruct->pCPAL_I2C_Struct->I2C_OwnAddress1 = 0x00;
  mBusCurrent->CPAL_InitStruct->pCPAL_I2C_Struct->I2C_Ack = I2C_Ack_Enable;
  mBusCurrent->CPAL_InitStruct->pCPAL_I2C_Struct->I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;

  CPAL_I2C_Init(mBusCurrent->CPAL_InitStruct);
  #endif
}

void mBusInit(uint8_t i2cNum)
{
  mBusInitTiming(i2cNum, I2C_Timing_FastMode);
}


void mBusSlaveInit(uint8_t i2cNum) {
  #if defined(USING_MBUS1) || defined(USING_MBUS2)
  mBusTypeDef* mBusCurrent = getmBusPtr(i2cNum);
  if(i2cNum == 1)
  {
    #if defined(USING_MBUS1)
    RCC_I2CCLKConfig(RCC_I2C1CLK_SYSCLK);
    mBusCurrent->CPAL_InitStruct = &I2C1_DevStructure;
    CPAL_I2C_StructInit(mBusCurrent->CPAL_InitStruct);
    mBusCurrent->CPAL_InitStruct->CPAL_Dev = CPAL_I2C1; // what's this for?
    #endif
  }
  else //if(i2cNum == 2)
  {
    #if defined(USING_MBUS2)
    RCC_I2CCLKConfig(RCC_I2C2CLK_SYSCLK);
    mBusCurrent->CPAL_InitStruct = &I2C2_DevStructure;
    CPAL_I2C_StructInit(mBusCurrent->CPAL_InitStruct);
    mBusCurrent->CPAL_InitStruct->CPAL_Dev = CPAL_I2C2; // what's this for?
    #endif
  }
    
  // RX Transfer Structure
  mBusCurrent->mBusRx.pbBuffer = pNULL; //buffer pointer data
  mBusCurrent->mBusRx.wNumData = 0;     //length of data
  mBusCurrent->mBusRx.wAddr1   = 0;     //slaveAddr
  mBusCurrent->mBusRx.wAddr2   = 0;     //regAddr

  // TX Transfer Structure
  mBusCurrent->mBusTx.pbBuffer = pNULL; //buffer pointer data
  mBusCurrent->mBusTx.wNumData = 0;     //length of data
  mBusCurrent->mBusTx.wAddr1   = 0;     //slaveAddr
  mBusCurrent->mBusTx.wAddr2   = 0;     //regAddr
      
  mBusCurrent->CPAL_InitStruct->CPAL_Direction   = CPAL_DIRECTION_TXRX;
  mBusCurrent->CPAL_InitStruct->CPAL_Mode        = CPAL_MODE_SLAVE;
  mBusCurrent->CPAL_InitStruct->CPAL_ProgModel   = CPAL_PROGMODEL_INTERRUPT;
  mBusCurrent->CPAL_InitStruct->pCPAL_TransferTx = &(mBusCurrent->mBusTx);
  mBusCurrent->CPAL_InitStruct->pCPAL_TransferRx = &(mBusCurrent->mBusRx);
  mBusCurrent->CPAL_InitStruct->wCPAL_Options    = CPAL_OPT_I2C_AUTOMATIC_END;
  mBusCurrent->CPAL_InitStruct->wCPAL_Options 	|= CPAL_OPT_16BIT_REG;

  mBusCurrent->CPAL_InitStruct->pCPAL_I2C_Struct->I2C_Timing        = I2C_Timing_FastMode;
  mBusCurrent->CPAL_InitStruct->pCPAL_I2C_Struct->I2C_AnalogFilter  = I2C_AnalogFilter_Enable;
  mBusCurrent->CPAL_InitStruct->pCPAL_I2C_Struct->I2C_DigitalFilter = 0x00;
  mBusCurrent->CPAL_InitStruct->pCPAL_I2C_Struct->I2C_Mode          = I2C_Mode_I2C;
  mBusCurrent->CPAL_InitStruct->pCPAL_I2C_Struct->I2C_OwnAddress1   = 0x00;
  mBusCurrent->CPAL_InitStruct->pCPAL_I2C_Struct->I2C_Ack           = I2C_Ack_Enable;
  mBusCurrent->CPAL_InitStruct->pCPAL_I2C_Struct->I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;

  CPAL_I2C_Init(mBusCurrent->CPAL_InitStruct);
  #endif
}


void mBusRestart(uint8_t i2cNum)
{
  mBusTypeDef* mBusCurrent = getmBusPtr(i2cNum);
  mBusInitTiming(i2cNum, mBusCurrent->timing);
}


uint8_t mBusPeriphReady(uint8_t slaveAddr, uint8_t i2cNum)
{
  mBusTypeDef* mBusCurrent = getmBusPtr(i2cNum);
  CPAL_InitTypeDef* mBusStruct = mBusCurrent->CPAL_InitStruct;
	mBusStruct->pCPAL_TransferTx = &(mBusCurrent->mBusTx);     
  mBusStruct->pCPAL_TransferTx->wAddr1 = (uint32_t)slaveAddr;
  if(CPAL_I2C_IsDeviceReady(mBusStruct) == CPAL_FAIL)
  {
    mBusStruct->CPAL_State = CPAL_STATE_READY;
    return ERROR;
  }
  else 
  {
    mBusStruct->CPAL_State = CPAL_STATE_READY;
    return SUCCESS;
  }
}


#if defined (USING_MBUS_MASTER_MODE) || !defined (USING_MBUS_LISTEN_MODE)
uint8_t mBusWrite(uint8_t slaveAddr, uint8_t regAddr, uint8_t data, uint8_t i2cNum)
{
  mBusTypeDef* mBusCurrent = getmBusPtr(i2cNum);
  CPAL_InitTypeDef* mBusStruct = mBusCurrent->CPAL_InitStruct;
	mBusCurrent->bufferWr[0] = data;
	mBusStruct->pCPAL_TransferTx = &(mBusCurrent->mBusTx); 
  mBusStruct->pCPAL_TransferTx->wNumData = 1;
 	mBusStruct->pCPAL_TransferTx->pbBuffer = mBusCurrent->bufferWr;
  mBusStruct->pCPAL_TransferTx->wAddr1   = (uint32_t)slaveAddr;
  mBusStruct->pCPAL_TransferTx->wAddr2   = (uint32_t)regAddr;

  if (CPAL_I2C_Write(mBusStruct) == CPAL_PASS)
  {
    while((mBusStruct->CPAL_State != CPAL_STATE_READY) && (mBusStruct->CPAL_State != CPAL_STATE_ERROR)){}
    if (mBusStruct->CPAL_State == CPAL_STATE_ERROR)
    {
      mBusStruct->CPAL_State = CPAL_STATE_READY;
      return ERROR;
    }
    return SUCCESS;
  }
  else 
  {
    return ERROR;
  }
}
#endif




#if defined (USING_MBUS_MASTER_MODE) || !defined (USING_MBUS_LISTEN_MODE)
// Write a byte stream to 7-bit slave address 'slave_addr' beginning at 16-bit 
// register address 'reg_addr'. Non-blocking unless previous transaction in 
// ongoing.  Address is high byte first first, data is native.
uint8_t mBusMasterWriteBurst(uint8_t slaveAddr, uint16_t regAddr, uint8_t length, uint8_t *data, uint8_t i2cNum)
{
  mBusTypeDef* mBus = getmBusPtr(i2cNum);
  CPAL_InitTypeDef* mBusStruct = mBus->CPAL_InitStruct;

  // Block If Device is Busy (a previous transaction is ongoing)
  // while (((mBusStruct->CPAL_State & CPAL_STATE_BUSY) != 0)
  //         || (mBusStruct->CPAL_State == CPAL_STATE_READY_TX)
  //         || (mBusStruct->CPAL_State == CPAL_STATE_READY_RX)) {};


  // while(mBus->CPAL_InitStruct->CPAL_State != CPAL_STATE_READY
  //    && mBus->CPAL_InitStruct->CPAL_State != CPAL_STATE_DISABLED
  //    && mBus->CPAL_InitStruct->CPAL_State != CPAL_STATE_ERROR) {};
  // DelayMicroseconds(1);

  // if(mBus->CPAL_InitStruct->CPAL_State == CPAL_STATE_DISABLED
  // || mBus->CPAL_InitStruct->CPAL_State == CPAL_STATE_ERROR) {
  //   mRedON;
  //   mBus->CPAL_InitStruct->CPAL_State = CPAL_STATE_READY;
  //   mBusInit(i2cNum);
  // }

  memcpy(mBus->bufferBurstWr, data, length);
  mBusStruct->pCPAL_TransferTx = &(mBus->mBusTx); 
  mBusStruct->pCPAL_TransferTx->wNumData = length;
  mBusStruct->pCPAL_TransferTx->pbBuffer = mBus->bufferBurstWr;
  mBusStruct->pCPAL_TransferTx->wAddr1   = (uint32_t)slaveAddr;
  mBusStruct->pCPAL_TransferTx->wAddr2   = (uint32_t)regAddr;
  mBusStruct->wCPAL_Options |= CPAL_OPT_16BIT_REG;

  if(CPAL_I2C_Write(mBusStruct) == CPAL_PASS) {
    return SUCCESS;
  }
  else {
    //WarnReport(ERROR_MBUS_MASTER_WRITE_BURST, "CPAL_I2C_Write failed");

    WarnReport(ERROR_MBUS_MASTER_WRITE_BURST, "CPAL_I2C_Write fail, Mode=%u,State=%u,Error=%u",
      mBus->CPAL_InitStruct->CPAL_Mode,
      mBus->CPAL_InitStruct->CPAL_State,
      mBus->CPAL_InitStruct->wCPAL_DevError);

    return ERROR;
  }
}
#endif


#if defined (USING_MBUS_MASTER_MODE) || !defined (USING_MBUS_LISTEN_MODE)
uint8_t mBusMasterReadBurstStart(uint8_t slaveAddr, uint16_t regAddr, uint8_t length, uint8_t i2cNum)
{
  mBusTypeDef* mBus = getmBusPtr(i2cNum);

  if(mBus->CPAL_InitStruct->CPAL_State != CPAL_STATE_READY) {
    WarnReport(ERROR_MBUS_MASTER_READ_BURST_START, "CPAL Not Ready, Mode=%u,State=%u,Error=%u",
      mBus->CPAL_InitStruct->CPAL_Mode,
      mBus->CPAL_InitStruct->CPAL_State,
      mBus->CPAL_InitStruct->wCPAL_DevError);
    //return ERROR;
    mBus->CPAL_InitStruct->CPAL_State = CPAL_STATE_READY;
  }

  mBus->CPAL_InitStruct->pCPAL_TransferRx           = &(mBus->mBusRx); 
  mBus->CPAL_InitStruct->pCPAL_TransferRx->wNumData = length;
  mBus->CPAL_InitStruct->pCPAL_TransferRx->pbBuffer = mBus->bufferBurstRd;
  mBus->CPAL_InitStruct->pCPAL_TransferRx->wAddr1   = (uint32_t)slaveAddr;
  mBus->CPAL_InitStruct->pCPAL_TransferRx->wAddr2   = (uint32_t)regAddr;
  mBus->CPAL_InitStruct->wCPAL_Options              |= CPAL_OPT_16BIT_REG;

  if(CPAL_I2C_Read(mBus->CPAL_InitStruct) == CPAL_PASS) {
    mBus->mBusReadBurstStartFlag=1;
    return SUCCESS;
  }
  else {
    WarnReport(ERROR_MBUS_MASTER_READ_BURST_START, "CPAL_I2C_Read failed");
    return ERROR;
  }
}
#endif


#if defined (USING_MBUS_MASTER_MODE) || !defined (USING_MBUS_LISTEN_MODE)
uint8_t mBusRead(uint8_t slaveAddr, uint8_t regAddr, uint8_t i2cNum)
{
  mBusTypeDef* mBusCurrent = getmBusPtr(i2cNum);
  CPAL_InitTypeDef* mBusStruct = mBusCurrent->CPAL_InitStruct;
	mBusCurrent->bufferRd[0] = 0;
	mBusStruct->pCPAL_TransferRx = &mBusCurrent->mBusRx; 
  mBusStruct->pCPAL_TransferRx->wNumData = 1;
 	mBusStruct->pCPAL_TransferRx->pbBuffer = mBusCurrent->bufferRd;
  mBusStruct->pCPAL_TransferRx->wAddr1   = (uint32_t)slaveAddr;
  mBusStruct->pCPAL_TransferRx->wAddr2   = (uint32_t)regAddr;

  if(CPAL_I2C_Read(mBusStruct) == CPAL_PASS)
  {
    while((mBusStruct->CPAL_State != CPAL_STATE_READY) && (mBusStruct->CPAL_State != CPAL_STATE_ERROR)){}
	  if(mBusStruct->CPAL_State == CPAL_STATE_ERROR)
    {
      mBusStruct->CPAL_State = CPAL_STATE_READY;
      return ERROR;
    }
    return mBusCurrent->bufferRd[0];
  }
  else
  {
    return ERROR;
  }	
}
#endif


#if defined (USING_MBUS_MASTER_MODE) || !defined (USING_MBUS_LISTEN_MODE)
uint8_t mBusWriteBurst(uint8_t slaveAddr, uint8_t regAddr, uint8_t length, uint8_t *data, uint8_t i2cNum)
{
  mBusTypeDef* mBusCurrent = getmBusPtr(i2cNum);
  CPAL_InitTypeDef* mBusStruct = mBusCurrent->CPAL_InitStruct;
	uint8_t i;
	for(i=0;i<length;i++)
	{
		mBusCurrent->bufferBurstWr[i] = data[i];	
	}
	mBusStruct->pCPAL_TransferTx = &(mBusCurrent->mBusTx); 
  mBusStruct->pCPAL_TransferTx->wNumData = length;
 	mBusStruct->pCPAL_TransferTx->pbBuffer = mBusCurrent->bufferBurstWr;
  mBusStruct->pCPAL_TransferTx->wAddr1   = (uint32_t)slaveAddr;
  mBusStruct->pCPAL_TransferTx->wAddr2   = (uint32_t)regAddr;

  if(CPAL_I2C_Write(mBusStruct) == CPAL_PASS)
  {
    while((mBusStruct->CPAL_State != CPAL_STATE_READY) && (mBusStruct->CPAL_State != CPAL_STATE_ERROR)){}
    if (mBusStruct->CPAL_State == CPAL_STATE_ERROR)
    {
      mBusStruct->CPAL_State = CPAL_STATE_READY;
      return ERROR;
    }
    return SUCCESS;
  }
  else 
  {
    return ERROR;
  }
}
#endif


#if defined (USING_MBUS_MASTER_MODE) || !defined (USING_MBUS_LISTEN_MODE)
uint8_t mBusReadBurst(uint8_t slaveAddr, uint8_t regAddr, uint8_t length, uint8_t *data, uint8_t i2cNum)
{
  mBusTypeDef* mBusCurrent = getmBusPtr(i2cNum);
  CPAL_InitTypeDef* mBusStruct = mBusCurrent->CPAL_InitStruct;
  mBusStruct->pCPAL_TransferRx = &(mBusCurrent->mBusRx); 
  mBusStruct->pCPAL_TransferRx->wNumData = length;
  mBusStruct->pCPAL_TransferRx->pbBuffer = mBusCurrent->bufferBurstRd;
  mBusStruct->pCPAL_TransferRx->wAddr1   = (uint32_t)slaveAddr;
  mBusStruct->pCPAL_TransferRx->wAddr2   = (uint32_t)regAddr;

  if(CPAL_I2C_Read(mBusStruct) == CPAL_PASS)
  {
    while((mBusStruct->CPAL_State != CPAL_STATE_READY) && (mBusStruct->CPAL_State != CPAL_STATE_ERROR)){}
    if(mBusStruct->CPAL_State == CPAL_STATE_ERROR)
    {
      mBusStruct->CPAL_State = CPAL_STATE_READY;
      return ERROR;
    }
    for(uint8_t i=0;i<length;i++)
    {
      data[i] = mBusCurrent->bufferBurstRd[i]; 
    }
    return SUCCESS;
  }
  else
  {
    return ERROR;
  }
}
#endif


#if defined (USING_MBUS_MASTER_MODE) || !defined (USING_MBUS_LISTEN_MODE)
uint8_t mBusWriteNoAdd(uint8_t slaveAddr, uint8_t data, uint8_t i2cNum)
{
  mBusTypeDef* mBusCurrent = getmBusPtr(i2cNum);
  CPAL_InitTypeDef* mBusStruct = mBusCurrent->CPAL_InitStruct;
  mBusCurrent->bufferWr[0] = data;
  mBusStruct->wCPAL_Options |= CPAL_OPT_NO_MEM_ADDR;
  mBusStruct->pCPAL_TransferTx = &(mBusCurrent->mBusTx); 
  mBusStruct->pCPAL_TransferTx->wNumData = 1;
  mBusStruct->pCPAL_TransferTx->pbBuffer = mBusCurrent->bufferWr;
  mBusStruct->pCPAL_TransferTx->wAddr1   = (uint32_t)slaveAddr;

  if(CPAL_I2C_Write(mBusStruct) == CPAL_PASS)
  {
    while((mBusStruct->CPAL_State != CPAL_STATE_READY) && (mBusStruct->CPAL_State != CPAL_STATE_ERROR)){}
    if (mBusStruct->CPAL_State == CPAL_STATE_ERROR)
    {
      mBusStruct->CPAL_State = CPAL_STATE_READY;
      mBusStruct->wCPAL_Options &= ~CPAL_OPT_NO_MEM_ADDR;
      return ERROR;
    }
    mBusStruct->wCPAL_Options &= ~CPAL_OPT_NO_MEM_ADDR;
    return SUCCESS;
  }
  else 
  {
    mBusStruct->wCPAL_Options &= ~CPAL_OPT_NO_MEM_ADDR;
    return ERROR;
  }
}
#endif


#if defined (USING_MBUS_MASTER_MODE) || !defined (USING_MBUS_LISTEN_MODE)
uint8_t mBusReadNoAdd(uint8_t slaveAddr, uint8_t i2cNum)
{
  mBusTypeDef* mBusCurrent = getmBusPtr(i2cNum);
  CPAL_InitTypeDef* mBusStruct = mBusCurrent->CPAL_InitStruct;
  mBusCurrent->bufferRd[0] = 0;
  mBusStruct->wCPAL_Options |= CPAL_OPT_NO_MEM_ADDR;
  mBusStruct->pCPAL_TransferRx = &(mBusCurrent->mBusRx); 
  mBusStruct->pCPAL_TransferRx->wNumData = 1;
  mBusStruct->pCPAL_TransferRx->pbBuffer = mBusCurrent->bufferRd;
  mBusStruct->pCPAL_TransferRx->wAddr1   = (uint32_t)slaveAddr;

  if(CPAL_I2C_Read(mBusStruct) == CPAL_PASS)
  {
    while((mBusStruct->CPAL_State != CPAL_STATE_READY) && (mBusStruct->CPAL_State != CPAL_STATE_ERROR)){}
    if(mBusStruct->CPAL_State == CPAL_STATE_ERROR)
    {
      mBusStruct->CPAL_State = CPAL_STATE_READY;
      mBusStruct->wCPAL_Options &= ~CPAL_OPT_NO_MEM_ADDR;
      return ERROR;
    }
    mBusStruct->wCPAL_Options &= ~CPAL_OPT_NO_MEM_ADDR;
    return mBusCurrent->bufferRd[0];
  }
  else
  {
    mBusStruct->wCPAL_Options &= ~CPAL_OPT_NO_MEM_ADDR;
    return ERROR;
  } 
}
#endif


#if defined (USING_MBUS_MASTER_MODE) || !defined (USING_MBUS_LISTEN_MODE)
uint8_t mBusWriteBurstNoAdd(uint8_t slaveAddr, uint8_t length, uint8_t *data, uint8_t i2cNum)
{
  mBusTypeDef* mBusCurrent = getmBusPtr(i2cNum);
  CPAL_InitTypeDef* mBusStruct = mBusCurrent->CPAL_InitStruct;
  uint8_t i;
  for(i=0;i<length;i++)
  {
    mBusCurrent->bufferBurstWr[i] = data[i]; 
  }
  mBusStruct->wCPAL_Options |= CPAL_OPT_NO_MEM_ADDR;
  mBusStruct->pCPAL_TransferTx = &(mBusCurrent->mBusTx); 
  mBusStruct->pCPAL_TransferTx->wNumData = length;
  mBusStruct->pCPAL_TransferTx->pbBuffer = mBusCurrent->bufferBurstWr;
  mBusStruct->pCPAL_TransferTx->wAddr1   = (uint32_t)slaveAddr;

  if(CPAL_I2C_Write(mBusStruct) == CPAL_PASS)
  {
    while((mBusStruct->CPAL_State != CPAL_STATE_READY) && (mBusStruct->CPAL_State != CPAL_STATE_ERROR)){}
    if (mBusStruct->CPAL_State == CPAL_STATE_ERROR)
    {
      mBusStruct->CPAL_State = CPAL_STATE_READY;
      mBusStruct->wCPAL_Options &= ~CPAL_OPT_NO_MEM_ADDR;
      return ERROR;
    }
    mBusStruct->wCPAL_Options &= ~CPAL_OPT_NO_MEM_ADDR;
    return SUCCESS;
  }
  else 
  {
    mBusStruct->wCPAL_Options &= ~CPAL_OPT_NO_MEM_ADDR;
    return ERROR;
  }
}
#endif


#if defined (USING_MBUS_MASTER_MODE) || !defined (USING_MBUS_LISTEN_MODE)
uint8_t mBusReadBurstNoAdd(uint8_t slaveAddr, uint8_t length, uint8_t *data, uint8_t i2cNum)
{
  mBusTypeDef* mBusCurrent = getmBusPtr(i2cNum);
  CPAL_InitTypeDef* mBusStruct = mBusCurrent->CPAL_InitStruct;
  mBusStruct->wCPAL_Options |= CPAL_OPT_NO_MEM_ADDR;
  mBusStruct->pCPAL_TransferRx = &(mBusCurrent->mBusRx); 
  mBusStruct->pCPAL_TransferRx->wNumData = length;
  mBusStruct->pCPAL_TransferRx->pbBuffer = mBusCurrent->bufferBurstRd;
  mBusStruct->pCPAL_TransferRx->wAddr1   = (uint32_t)slaveAddr;

  if(CPAL_I2C_Read(mBusStruct) == CPAL_PASS)
  {
    while((mBusStruct->CPAL_State != CPAL_STATE_READY) && (mBusStruct->CPAL_State != CPAL_STATE_ERROR)){}
    if(mBusStruct->CPAL_State == CPAL_STATE_ERROR)
    {
      mBusStruct->CPAL_State = CPAL_STATE_READY;
      mBusStruct->wCPAL_Options &= ~CPAL_OPT_NO_MEM_ADDR;
      return ERROR;
    }
    mBusStruct->wCPAL_Options &= ~CPAL_OPT_NO_MEM_ADDR;
    for(uint8_t i=0;i<length;i++)
    {
      data[i] = mBusCurrent->bufferBurstRd[i]; 
    }
    return SUCCESS;
  }
  else
  {
    mBusStruct->wCPAL_Options &= ~CPAL_OPT_NO_MEM_ADDR;
    return ERROR;
  }
}
#endif


#if defined (USING_MBUS_MASTER_MODE) || !defined (USING_MBUS_LISTEN_MODE)
uint8_t mBusWriteBurstNB(uint8_t slaveAddr, uint8_t regAddr, uint8_t length, uint8_t *data, uint8_t i2cNum)
{
  mBusTypeDef* mBusCurrent = getmBusPtr(i2cNum);
  CPAL_InitTypeDef* mBusStruct = mBusCurrent->CPAL_InitStruct;
  uint8_t i;
  for(i=0;i<length;i++)
  {
    mBusCurrent->bufferBurstWr[i] = data[i]; 
  }
  mBusStruct->pCPAL_TransferTx = &(mBusCurrent->mBusTx); 
  mBusStruct->pCPAL_TransferTx->wNumData = length;
  mBusStruct->pCPAL_TransferTx->pbBuffer = mBusCurrent->bufferBurstWr;
  mBusStruct->pCPAL_TransferTx->wAddr1   = (uint32_t)slaveAddr;
  mBusStruct->pCPAL_TransferTx->wAddr2   = (uint32_t)regAddr;

  if(CPAL_I2C_Write(mBusStruct) == CPAL_PASS)
  {
    return SUCCESS;
  }
  else 
  {
    return ERROR;
  }
}
#endif


#if defined (USING_MBUS_MASTER_MODE) || !defined (USING_MBUS_LISTEN_MODE)
uint8_t mBusReadBurstStartNB(uint8_t slaveAddr, uint8_t regAddr, uint8_t length, uint8_t i2cNum)
{
  mBusTypeDef* mBusCurrent = getmBusPtr(i2cNum);
  CPAL_InitTypeDef* mBusStruct = mBusCurrent->CPAL_InitStruct;
  if(mBusStruct->CPAL_State != CPAL_STATE_READY){return ERROR;}
	mBusStruct->pCPAL_TransferRx = &(mBusCurrent->mBusRx); 
  mBusStruct->pCPAL_TransferRx->wNumData = length;
 	mBusStruct->pCPAL_TransferRx->pbBuffer = mBusCurrent->bufferBurstRd;
  mBusStruct->pCPAL_TransferRx->wAddr1   = (uint32_t)slaveAddr;
  mBusStruct->pCPAL_TransferRx->wAddr2   = (uint32_t)regAddr;

  	if(CPAL_I2C_Read(mBusStruct) == CPAL_PASS)
  	{
      mBusCurrent->mBusReadBurstStartFlag=1;
  		return SUCCESS;
  	}
  	else 
  	{
  		return ERROR;
  	}
}
#endif


#if defined (USING_MBUS_MASTER_MODE) || !defined (USING_MBUS_LISTEN_MODE)
uint8_t mBusReadBurstDataNB(uint8_t length, uint8_t* data, uint8_t i2cNum)
{
  mBusTypeDef* mBusCurrent = getmBusPtr(i2cNum);
	if(mBusCurrent->mBusReadBurstReady)
	{
		for(uint8_t i=0;i<length;i++)
    {
      data[i] = mBusCurrent->bufferBurstRd[i]; 
    }
		mBusCurrent->mBusReadBurstReady = 0;
    mBusCurrent->mBusReadBurstStartFlag=0;
		return SUCCESS;
	}
	else
	{
		return ERROR;
	}
}
#endif


uint32_t CPAL_TIMEOUT_UserCallback(CPAL_InitTypeDef* pDevInitStruct)
{
  WarnReport(ERROR_CPAL_TIMEOUT, "MBUS CPAL_TIMEOUT,Mode=%u,State=%u,Error=%u",
    pDevInitStruct->CPAL_Mode,
    pDevInitStruct->CPAL_State,
    pDevInitStruct->wCPAL_DevError);

  // if in slave mode
  if(pDevInitStruct->CPAL_Mode == CPAL_MODE_SLAVE) {
    pDevInitStruct->CPAL_State = CPAL_STATE_READY;
    #if defined (CPAL_I2C_LISTEN_MODE) && defined (CPAL_I2C_SLAVE_MODE)
    CPAL_I2C_Listen(pDevInitStruct);
    #endif
  }
  // otherwise in master mode
  else {
    pDevInitStruct->CPAL_State = CPAL_STATE_READY;
    //if (pDevInitStruct->CPAL_Dev == CPAL_I2C1)
    //  mBusRestart(1);
    //else if (pDevInitStruct->CPAL_Dev == CPAL_I2C2)
    //  mBusRestart(2);
    //}
  }

	return CPAL_PASS;
}


/*=========== Transfer UserCallback ===========*/

// only required in slave mode
void CPAL_I2C_TXTC_UserCallback(CPAL_InitTypeDef* pDevInitStruct) {
  // get mBus pointer, or return on failure
  mBusTypeDef* mBus = getmBusPtrFromCallback(pDevInitStruct);
  if(mBus == pNULL) { return; }

  // restart listen if in slave mode
  #if defined (CPAL_I2C_LISTEN_MODE) && defined (CPAL_I2C_SLAVE_MODE)
  if(mBus->CPAL_InitStruct->CPAL_Mode == CPAL_MODE_SLAVE) {
    if (pDevInitStruct->CPAL_Dev == CPAL_I2C1) {
      mBusSlaveInit(1);   // this is a hack fix to some sort of buffering issue
      mBusSlaveListen(1);
    }
    else if (pDevInitStruct->CPAL_Dev == CPAL_I2C2) {
      mBusSlaveInit(2);   // this is a hack fix to some sort of buffering issue
      mBusSlaveListen(2);
    }
  }
  #endif
}

// only required in slave mode
void CPAL_I2C_TX_UserCallback(CPAL_InitTypeDef* pDevInitStruct) {
  // get mBus pointer, or return on failure
  mBusTypeDef* mBus = getmBusPtrFromCallback(pDevInitStruct);
  if(mBus == pNULL) { return; }

  #if defined (CPAL_I2C_LISTEN_MODE) && defined (CPAL_I2C_SLAVE_MODE)
  if(mBus->CPAL_InitStruct->CPAL_Mode == CPAL_MODE_SLAVE) {
    mBus->byte_count++;
    if(mBus->byte_count == 5) {
      // advance register address and prepare next value to send
      mBus->reg_addr++;
      mBus->byte_count = 1;
      uint32_t value = SlaveTx_UserCallback(mBus->reg_addr);
      memcpy(mBus->bufferBurstWr, (uint8_t*)&value, 4);
      mBus->CPAL_InitStruct->pCPAL_TransferTx->pbBuffer = mBus->bufferBurstWr;          // data in
      mBus->CPAL_InitStruct->pCPAL_TransferTx->wNumData = MBUS_BUFF_SIZE;       // length out
    }
  }
  #endif
}

// only required in slave mode
void CPAL_I2C_RXTC_UserCallback(CPAL_InitTypeDef* pDevInitStruct) {  
  // get mBus pointer, or return on failure
  mBusTypeDef* mBus = getmBusPtrFromCallback(pDevInitStruct);
  if(mBus == pNULL) { return; }

  if(mBus->mBusReadBurstStartFlag == 1) {
    mBus->mBusReadBurstReady = 1;
  }

  // restart listen if in slave mode
  #if defined (CPAL_I2C_LISTEN_MODE) && defined (CPAL_I2C_SLAVE_MODE)
  if(mBus->CPAL_InitStruct->CPAL_Mode == CPAL_MODE_SLAVE) {
    if (pDevInitStruct->CPAL_Dev == CPAL_I2C1)
      mBusSlaveListen(1);
    else if (pDevInitStruct->CPAL_Dev == CPAL_I2C2)
      mBusSlaveListen(2);
  }
  #endif
}

// only required in slave mode
// Called after reception of each byte.  Generates slave callbacks with 
// received register address and received 32-bit value.
void CPAL_I2C_RX_UserCallback(CPAL_InitTypeDef* pDevInitStruct) {
  // get mBus pointer, or return on failure
  mBusTypeDef* mBus = getmBusPtrFromCallback(pDevInitStruct);
  if(mBus == pNULL) { return; }

  #if defined (CPAL_I2C_LISTEN_MODE) && defined (CPAL_I2C_SLAVE_MODE)
  if(mBus->CPAL_InitStruct->CPAL_Mode == CPAL_MODE_SLAVE) {
    mBus->byte_count++;
    if(!mBus->has_reg_addr && mBus->byte_count == 2) {
      mBus->reg_addr = ((uint16_t)mBus->bufferBurstRd[0]<<8) + mBus->bufferBurstRd[1];
      mBus->has_reg_addr = 1;
    }
    else if(mBus->has_reg_addr && (mBus->byte_count-2)%4 == 0) {
      uint32_t value;
      memcpy(&value, &(mBus->bufferBurstRd[mBus->byte_count-4]), 4);
      SlaveRx_UserCallback(mBus->reg_addr, value);
      mBus->reg_addr++;
    }
  }
  #endif
}

//void CPAL_I2C_TX_UserCallback(CPAL_InitTypeDef* pDevInitStruct) {}
//void CPAL_I2C_DMATXTC_UserCallback(CPAL_InitTypeDef* pDevInitStruct) {}
//void CPAL_I2C_DMATXHT_UserCallback(CPAL_InitTypeDef* pDevInitStruct) {}
//void CPAL_I2C_DMATXTE_UserCallback(CPAL_InitTypeDef* pDevInitStruct) {}


// Called when the Transfer Complete interrupt occurs for the RX DMA channel
void CPAL_I2C_DMARXTC_UserCallback(CPAL_InitTypeDef* pDevInitStruct)
{
  #if defined(USING_MBUS1) || defined(USING_MBUS2)

  mBusTypeDef* mBusCurrent;
  if (pDevInitStruct->CPAL_Dev == CPAL_I2C1) {
    #ifdef USING_MBUS1
    mBusCurrent = &mBus1;
    #endif
  }
  else if (pDevInitStruct->CPAL_Dev == CPAL_I2C2) {
    #ifdef USING_MBUS2
    mBusCurrent = &mBus2;
    #endif
  }
  else {
    return;
  }

  if(mBusCurrent->mBusReadBurstStartFlag == 1) {
    mBusCurrent->mBusReadBurstReady = 1;
  }

  #endif
}


//void CPAL_I2C_DMARXHT_UserCallback(CPAL_InitTypeDef* pDevInitStruct) {}
//void CPAL_I2C_DMARXTE_UserCallback(CPAL_InitTypeDef* pDevInitStruct) {}


void CPAL_I2C_ERR_UserCallback(CPAL_DevTypeDef pDevInstance, uint32_t DeviceError)
{
  // generate warning message
  mBusTypeDef* mBus;
  if (pDevInstance == CPAL_I2C1)
    mBus = getmBusPtr(1);
  else if (pDevInstance == CPAL_I2C2)
    mBus = getmBusPtr(2);
  else
    return;
  WarnReport(ERROR_CPAL_I2C_ERR, "MBUS CPAL_I2C_ERR,Mode=%u,State=%u,Error=%u",
    mBus->CPAL_InitStruct->CPAL_Mode,
    mBus->CPAL_InitStruct->CPAL_State,
    mBus->CPAL_InitStruct->wCPAL_DevError);

  // reset after error
  if (pDevInstance == CPAL_I2C1)
    mBusRestart(1);
  else if (pDevInstance == CPAL_I2C2)
    mBusRestart(2);
}


/// @brief  User callback that manage slave read operation.
/// @param  pDevInitStruct 
/// @retval None
#ifdef CPAL_I2C_SLAVE_MODE
void CPAL_I2C_SLAVE_READ_UserCallback(CPAL_InitTypeDef* pDevInitStruct) {
  // get mBus pointer, or return on failure
  mBusTypeDef* mBus = getmBusPtrFromCallback(pDevInitStruct);
  if(mBus == pNULL) { return; }

  mBus->has_reg_addr = 0;
  mBus->reg_addr = 0;
  mBus->byte_count = 0;

  // configure transfer parameters
  mBus->CPAL_InitStruct->pCPAL_TransferRx = &(mBus->mBusRx);
  mBus->CPAL_InitStruct->pCPAL_TransferRx->pbBuffer = mBus->bufferBurstRd;            // data in
  mBus->CPAL_InitStruct->pCPAL_TransferRx->wNumData = MBUS_BUFF_SIZE;//mBus->next_length_slave_read; // length in (?)
  mBus->CPAL_InitStruct->pCPAL_TransferRx->wAddr1   = 0;                              // unused in slave mode
  mBus->CPAL_InitStruct->pCPAL_TransferRx->wAddr2   = 0;                              // unused in slave mode
  
  // configure interrupt-driven read
  __CPAL_I2C_HAL_ENABLE_SLAVE_RXIT(mBus->CPAL_InitStruct->CPAL_Dev);
}
#endif


/// @brief  User callback that manage slave write operation.
/// @param  pDevInitStruct 
/// @retval None
#ifdef CPAL_I2C_SLAVE_MODE
void CPAL_I2C_SLAVE_WRITE_UserCallback(CPAL_InitTypeDef* pDevInitStruct) {  
  // get mBus pointer, or return on failure
  mBusTypeDef* mBus = getmBusPtrFromCallback(pDevInitStruct);
  if(mBus == pNULL) { return; }

  // prepare first value to send
  uint32_t value = SlaveTx_UserCallback(mBus->reg_addr);
  mBus->byte_count = 0;
  memcpy(mBus->bufferBurstWr, (uint8_t*)&value, 4);

  // configure transfer parameters
  mBus->CPAL_InitStruct->pCPAL_TransferTx = &(mBus->mBusTx);
  mBus->CPAL_InitStruct->pCPAL_TransferTx->pbBuffer = mBus->bufferBurstWr;  // data out
  mBus->CPAL_InitStruct->pCPAL_TransferTx->wNumData = MBUS_BUFF_SIZE;       // length out
  mBus->CPAL_InitStruct->pCPAL_TransferTx->wAddr1 = 0;                      // unused in slave mode
  mBus->CPAL_InitStruct->pCPAL_TransferTx->wAddr2 = 0;                      // unused in slave mode

  // configure interrupt-driven write
  __CPAL_I2C_HAL_ENABLE_SLAVE_TXIT(mBus->CPAL_InitStruct->CPAL_Dev);
}
#endif


#if defined (CPAL_I2C_LISTEN_MODE) && defined (CPAL_I2C_SLAVE_MODE)
uint8_t mBusSlaveListen(uint8_t i2cNum) {
  mBusTypeDef* mBus = getmBusPtr(i2cNum);

  if(mBus->CPAL_InitStruct->CPAL_State == CPAL_STATE_READY
      || mBus->CPAL_InitStruct->CPAL_State == CPAL_STATE_DISABLED)
  {
      mBus->CPAL_InitStruct->CPAL_State = CPAL_STATE_READY;
      CPAL_I2C_Listen(mBus->CPAL_InitStruct);
  }
  return 0;
}
#endif