/**
  *****************************************************************************************
  * @file    I2C_V2.c 
  * @author  Chaitanya Bhargava (cbhar)
  * @version V1.1.0
  * @date    06-June-2014
  * @brief   This file contains all the functions to do basic i2c 2 wire communication.
  *          The functions and structs are from the stm32 standard peripheral lib.
  *          In case of any communication error the yellow LED will turn on. 
  *****************************************************************************************
*/

//*****************************************************************************
//
//! \addtogroup NonCPAL_i2c_api
//! @{
//
//*****************************************************************************


#include "I2C_V2platform_config.h"
#include "I2C_V2.h"
#include "mGeneral.h"
#define OWN_ADDR 	0x00 // Master of Puppets
#define mIMU_ADDR  (0x69 & 0x7f) << 1 //mIMU address. Note how the bits need to be shifted.
//-------------------------PRIVATE VARIABLES AND FUNCTIONS-----------------------//
volatile uint8_t i2cnackflag = 0;
uint8_t nack_check(uint8_t SLAVE_ADDR);
//---------------------Initialise I2C----------------------------//
//**************************************************************************************
//
//!  I2C_config
//!
//!  @param    none
//!   
//!  @return   none
//!
//!  @brief    This functions sets up the I2C pheripheral based on the values defined in
//!            I2C_V2platform_config.h Note the I2C_Timing value determines the speed, high
//!            time, low time of the I2C lines and need to be configured properly
//!            This is a simpler plain configuration of I2C which does not use CPAL
//
//**************************************************************************************
void I2C_config()
{
  I2C_DeInit(I2C_NUM);
	/* Enable the I2C peripheral clock*/
	RCC_APB1PeriphClockCmd(I2C_RCC,ENABLE);
	/* All GPIO clocks already enabled*/

	/*Connect pins to i2c alternate function*/
	GPIO_PinAFConfig(I2C_PORT,I2C_SCK_PIN_SOURCE,I2C_AF);//PB8 i2c sck
	GPIO_PinAFConfig(I2C_PORT,I2C_SDA_PIN_SOURCE,I2C_AF);//PB9 i2c sda

	/*Configure the desired pins to Aternate function*/
	GPIO_InitTypeDef GPIO_InitStructure;
	/* Configure pins as AF pushpull */
  	GPIO_InitStructure.GPIO_Pin = I2C_SCK_PIN | I2C_SDA_PIN;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  	GPIO_Init(I2C_PORT, &GPIO_InitStructure); 	

  	/* Set up the I2C configuration*/
  	I2C_InitTypeDef i2cInitStructure;
  	i2cInitStructure.I2C_Timing = I2C_TIMING; // I2c speed 100Khz
  	i2cInitStructure.I2C_AnalogFilter = I2C_AnalogFilter_Enable ; 
  	i2cInitStructure.I2C_DigitalFilter = 0x00;
  	i2cInitStructure.I2C_Mode = I2C_Mode_I2C;
  	i2cInitStructure.I2C_OwnAddress1 = OWN_ADDR;
  	i2cInitStructure.I2C_Ack = I2C_Ack_Disable;
  	i2cInitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  	I2C_Init(I2C_NUM,&i2cInitStructure);
    I2C_Cmd(I2C_NUM,ENABLE);
  	/* Enable the NVIC for the corresponding I2c*/
    /* I2C1 Channel configuration (INTERRUPT)*/
   NVIC_InitTypeDef NVIC_InitStructure;
   
    NVIC_InitStructure.NVIC_IRQChannel = I2C1_EV_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure); 
    /*Configure interrupt to be called if no acknowledge recieved*/
    I2C_ITConfig(I2C1,I2C_IT_NACKI,ENABLE);
}
//----------------------Functions to write or read----------------------//
//**************************************************************************************
//
//!  I2C_write_1byte
//!
//!  @param          SLAVE_ADDR   The 7-bit address of the slave
//!  @param          reg          The 8-bit register to write to
//!  @param          data         The 8-bit data to send
//!            
//!            
//!  @return   0                         if write failed
//!
//!  @return   1                         if write was a success
//!
//!  @brief    Writes 1 byte over the I2C bus to the addressed slave
//!        
//        
//**************************************************************************************
uint8_t I2C_write_1byte(uint8_t SLAVE_ADDR,uint8_t reg,uint8_t data)
{
  //I2C_SlaveAddressConfig ( I2C_NUM,SLAVE_ADDR);
  //I2C_AcknowledgeConfig(I2C_NUM, DISABLE);
  I2C_TransferHandling ( I2C_NUM,SLAVE_ADDR,3,I2C_AutoEnd_Mode,I2C_Generate_Start_Write);
  I2C_GenerateSTART(I2C_NUM,ENABLE);
  //Start and send address with write bit
  I2C_NUM->TXDR = reg;
  while(I2C_GetFlagStatus(I2C_NUM,I2C_FLAG_TXE)==RESET){
    if (nack_check(SLAVE_ADDR))
      return 0;
  }
  //Send the register to write to
  I2C_NUM->TXDR = data;
  while(I2C_GetFlagStatus(I2C_NUM,I2C_FLAG_TXE)==RESET){
    if (nack_check(SLAVE_ADDR))
      return 0;
  }
  //Send the data
  I2C_GenerateSTOP ( I2C_NUM,ENABLE);
  return 1;
}
//**************************************************************************************
//
//!  I2C_write
//!
//!  @param          SLAVE_ADDR   The 7-bit address of the slave
//!  @param          reg          The 8-bit register to start writing to
//!  @param          data         The pointer to the data
//!  @param          len          length of the data to be sent
//!            
//!  @return   0                         if write failed
//!
//!  @return   1                         if write was a success
//!
//!  @brief    Writes len number of bytes to the addressed slave
//!        
//        
//**************************************************************************************
uint8_t I2C_write(uint8_t SLAVE_ADDR,uint8_t reg,uint8_t *data, int len) //write multiple bytes
{
  //I2C_SlaveAddressConfig ( I2C_NUM,SLAVE_ADDR);
  //I2C_AcknowledgeConfig(I2C_NUM, DISABLE);
  I2C_TransferHandling ( I2C_NUM,SLAVE_ADDR,len+1,I2C_AutoEnd_Mode,I2C_Generate_Start_Write);
  I2C_GenerateSTART(I2C_NUM,ENABLE);
  //Start and send address with write bit
  I2C_NUM->TXDR = reg;
  while(I2C_GetFlagStatus(I2C_NUM,I2C_FLAG_TXE)==RESET){
    if (nack_check(SLAVE_ADDR))
      return 0;
  }
  //Send the register to write to
  for (int i = 0;i<len;i++)
  {
  I2C_NUM->TXDR = data[i];
  while(I2C_GetFlagStatus(I2C_NUM,I2C_FLAG_TXE)==RESET){
    if (nack_check(SLAVE_ADDR))
      return 0;
   }
  }
  //Send the data
  I2C_GenerateSTOP ( I2C_NUM,ENABLE);
  return 1;
}
//**************************************************************************************
//
//!  I2C_read_1byte
//!
//!  @param    SLAVE_ADDR   The 7-bit address of the slave
//!  @param    reg          The 8-bit register to read from
//!            
//!            
//!            
//!  @return   0                         if read failed
//!
//!  @return   rxbuf                     8-bit data read from the slave
//!
//!  @brief    Reads 1 byte over the I2C bus to the addressed slave
//!        
//        
//**************************************************************************************
uint8_t I2C_read_1byte(uint8_t SLAVE_ADDR, uint8_t reg)
{
  uint8_t rxbuf;
  //I2C_SlaveAddressConfig ( I2C_NUM,SLAVE_ADDR);
  //I2C_AcknowledgeConfig(I2C_NUM, ENABLE);// MOST IMPORTANT!!! 
  I2C_TransferHandling ( I2C_NUM,SLAVE_ADDR,2,I2C_SoftEnd_Mode,I2C_Generate_Start_Write);
  I2C_GenerateSTART(I2C_NUM,ENABLE);
  //Start and send address with write bit
  I2C_NUM->TXDR = reg;
  while(I2C_GetFlagStatus(I2C_NUM,I2C_FLAG_TXE)==RESET){
    if (nack_check(SLAVE_ADDR))
      return 0;
  }
  //Send the register to read from
  I2C_GenerateSTOP ( I2C_NUM,ENABLE);
  //Stop I2C
  I2C_TransferHandling ( I2C_NUM,SLAVE_ADDR,1,I2C_SoftEnd_Mode,I2C_Generate_Start_Read);
  I2C_GenerateSTART(I2C_NUM,ENABLE);
  //Restart and send adderss with read bit
  rxbuf = I2C_NUM->RXDR;
  while(I2C_GetFlagStatus(I2C_NUM,I2C_FLAG_RXNE)==RESET){
    if (nack_check(SLAVE_ADDR))
      return 0;
  }                     // weird
  //Read the data
  I2C_GenerateSTOP ( I2C_NUM,ENABLE);
  return rxbuf;
}
//**************************************************************************************
//
//!  I2C_read
//!
//!  @param    SLAVE_ADDR   The 7-bit address of the slave
//!  @param    reg          The 8-bit register to start reading from
//!  @param    rxbuf        The pointer to the buffer where recieved data is to be stored
//!  @param    len          length of the data to recieve
//!            
//!  @return   0                         if read failed
//!
//!  @return   rxbuf                     pointer the the buffer where the data is stored
//!
//!  @brief    Reads len number of bytes from the addressed slave
//!        
//        
//**************************************************************************************
uint8_t * I2C_read(uint8_t SLAVE_ADDR, uint8_t reg, uint8_t *rxbuf,int len) //Read multiple bytes
{
  //I2C_SlaveAddressConfig ( I2C_NUM,SLAVE_ADDR);
  //I2C_AcknowledgeConfig(I2C_NUM, ENABLE);// MOST IMPORTANT!!! 
  I2C_TransferHandling ( I2C_NUM,SLAVE_ADDR,2,I2C_SoftEnd_Mode,I2C_Generate_Start_Write);
  I2C_GenerateSTART(I2C_NUM,ENABLE);
  //Start and send address with write bit
  I2C_NUM->TXDR = reg;
  while(I2C_GetFlagStatus(I2C_NUM,I2C_FLAG_TXE)==RESET){
    if (nack_check(SLAVE_ADDR))
      return 0;
  }
  //Send the register to read from
  I2C_GenerateSTOP ( I2C_NUM,ENABLE);
  //Stop I2C
  I2C_TransferHandling ( I2C_NUM,SLAVE_ADDR,len,I2C_SoftEnd_Mode,I2C_Generate_Start_Read);
  I2C_GenerateSTART(I2C_NUM,ENABLE);
  //Restart and send adderss with read bit
  for (int i = 0;i<len;i++)
  {
  rxbuf[i] = I2C_NUM->RXDR;
  while(I2C_GetFlagStatus(I2C_NUM,I2C_FLAG_RXNE)==RESET){
    if (nack_check(SLAVE_ADDR))
      return 0;
    }
  }                     // weird
  //Read the data
  I2C_GenerateSTOP ( I2C_NUM,ENABLE);
  return rxbuf;
}
//--------------------I2C INTERRUPT HANDLER-----------------------//
void I2C1_EV_IRQHandler()
{
  i2cnackflag = 1;
   I2C_ITConfig(I2C1,I2C_IT_NACKI,DISABLE);
}
//-----------------------private function-------------------------//
uint8_t nack_check(uint8_t SLAVE_ADDR)
{
  if (i2cnackflag)
    {
      mYellowTOGGLE;
      I2C_TransferHandling ( I2C_NUM,SLAVE_ADDR,0,I2C_SoftEnd_Mode,I2C_Generate_Stop);
      I2C_ITConfig(I2C1,I2C_IT_NACKI,ENABLE);
      i2cnackflag = 0;
      return 1;
    }
  else
      return 0;
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//
//*****************************************************************************