/**
  *****************************************************************************************
  * @file    SPI.c 
  * @author  Chaitanya Bhargava (cbhar)
  * @version V1.1.0
  * @date    06-June-2014
  * @brief   This file will initialze the SPI pheripheral on the M4
  *****************************************************************************************
*/
#include "SPI.h"
#include "SPIplatform_config.h"
#include "mGeneral.h"
//**************************************************************************************
//
//!  SPI_config
//!
//!  @param    none
//!   
//!  @return   none
//!
//!  @brief    This will set up the SPI peripheral based on the settings in the 
//!            SPIplatform_config.h. For now it will set up SPI3 which is used by the wifi
//!            in SMORES and hence this should not be used since its set up already in 
//!            stm32_CC3000.c
//
//**************************************************************************************
void SPI_config(void)
{
	SPI_InitTypeDef SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	// Enable the SPI clock
	RCC_APB2PeriphClockCmd(SPI_RCC, ENABLE);
	// SPI Pin Definition
	GPIO_PinAFConfig(SPI_PORT, SPI_SCK_PIN_SOURCE, SPI_AF);    
	GPIO_PinAFConfig(SPI_PORT, SPI_MISO_PIN_SOURCE, SPI_AF); 
	GPIO_PinAFConfig(SPI_PORT, SPI_MOSI_PIN_SOURCE, SPI_AF);
		
	GPIO_InitStructure.GPIO_Pin = SPI_SCK_PIN | SPI_MISO_PIN | SPI_MOSI_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(SPI_PORT,&GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = SPI_NSS_PIN;  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(SPI_PORT,&GPIO_InitStructure);

	/*Configure the SPI*/
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_MODE;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_SCALING;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB; 
    SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI_NUM, &SPI_InitStructure);
    SPI_RxFIFOThresholdConfig(SPI_NUM, SPI_RxFIFOThreshold_QF);
    SPI_Cmd(SPI_NUM, ENABLE);
}