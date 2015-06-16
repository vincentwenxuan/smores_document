/**
  ******************************************************************************
  * @file    STM32F3xx_EEPROM_Emulation/inc/eeprom.h
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    02-October-2012
  * @brief   Main program body.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  * Modified 11/22/2013 by Jimmy Paulos
  * Modlab, University of Pennsylvania
  *
  ******************************************************************************
  */ 
  
#ifndef DYNAMIC_EEPROM_H
#define DYNAMIC_EEPROM_H

#include "common_peripherals.h"
#include STM32FXXX_H
#include STM32FXXX_FLASH_H

#include <stdint.h>

#ifdef USING_DYNAMIC_EEPROM

#ifdef __cplusplus
extern "C" {
#endif

uint16_t DynamicEE_Init(uint16_t* eeprom_addresses, uint16_t eeprom_number_values);
uint16_t DynamicEE_ReadVariable(uint16_t VirtAddress, uint32_t* Data);
uint16_t DynamicEE_WriteVariable(uint16_t* eeprom_addresses, uint16_t eeprom_number_values, uint16_t VirtAddress, uint32_t Data);
FLASH_Status DynamicEE_Format(void);


#ifdef __cplusplus
}
#endif

#endif // DYNAMIC_EEPROM_H

#endif // USING_DYNAMIC_EEPROM
