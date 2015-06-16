/**
  ******************************************************************************
  * @file    STM32F3xx_EEPROM_Emulation/src/eeprom.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    02-October-2012
  * @brief   This file provides all the EEPROM emulation firmware functions.
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

/* Includes ------------------------------------------------------------------*/
#include "dynamic_eeprom.h"
#include "common_peripherals.h"

#ifdef USING_DYNAMIC_EEPROM

/* Define the STM32F3xx Flash page size depending on the used STM32 device */
#define PAGE_SIZE  (uint16_t)0x800  /* Page size = 2KByte */

/* EEPROM start address in Flash */
#define EEPROM_START_ADDRESS    ((uint32_t)0x08000000 + (FLASH_SIZE/2-2)*PAGE_SIZE) // EEPROM emulation start address

/* Pages 0 and 1 base and end addresses */
#define PAGE0_BASE_ADDRESS      ((uint32_t)(EEPROM_START_ADDRESS + 0x000))
#define PAGE0_END_ADDRESS       ((uint32_t)(EEPROM_START_ADDRESS + (PAGE_SIZE - 1)))

#define PAGE1_BASE_ADDRESS      ((uint32_t)(EEPROM_START_ADDRESS + PAGE_SIZE))
#define PAGE1_END_ADDRESS       ((uint32_t)(EEPROM_START_ADDRESS + (2 * PAGE_SIZE - 1)))

/* Used Flash pages for EEPROM emulation */
#define PAGE0                   ((uint16_t)0x0000)
#define PAGE1                   ((uint16_t)0x0001)

/* No valid page define */
#define NO_VALID_PAGE           ((uint16_t)0x00AB)

/* Page status definitions */
#define ERASED                  ((uint16_t)0xFFFF)     /* PAGE is empty */
#define RECEIVE_DATA            ((uint16_t)0xEEEE)     /* PAGE is marked to receive data */
#define VALID_PAGE              ((uint16_t)0x0000)     /* PAGE containing valid data */

/* Valid pages in read and write defines */
#define READ_FROM_VALID_PAGE    ((uint8_t)0x00)
#define WRITE_IN_VALID_PAGE     ((uint8_t)0x01)

/* Page full define */
#define PAGE_FULL               ((uint8_t)0x80)



/** @addtogroup EEPROM_Emulation
  * @{
  */ 

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static uint16_t DynamicEE_VerifyPageFullWriteVariable(uint16_t VirtAddress, uint32_t Data);
static uint16_t DynamicEE_PageTransfer(uint16_t* eeprom_addresses, uint16_t eeprom_number_values, uint16_t VirtAddress, uint32_t Data);
static uint16_t DynamicEE_FindValidPage(uint8_t Operation);

FLASH_Status ErasePageInterruptDisabled(uint32_t Page_Address) {
  __disable_irq();  // disable interrupts here
  FLASH_Status FlashStatus = FLASH_ErasePage(Page_Address);
  __enable_irq();    // enable interrupts here
  return FlashStatus;
}

/**
  * @brief  Restore the pages to a known good state in case of page's status
  *   corruption after a power loss.
  * @param  None.
  * @retval - Flash error code: on write Flash error
  *         - FLASH_COMPLETE: on success
  */
uint16_t DynamicEE_Init(uint16_t* eeprom_addresses, uint16_t eeprom_number_values)
{
  uint16_t PageStatus0 = 6, PageStatus1 = 6;
  uint16_t VarIdx = 0;
  uint16_t EepromStatus = 0, ReadStatus = 0;
  int16_t x = -1;
  uint16_t  FlashStatus;

  FLASH_Unlock(); //Unlock flash
  /* Get Page0 status */
  PageStatus0 = (*(__IO uint16_t*)PAGE0_BASE_ADDRESS);
  /* Get Page1 status */
  PageStatus1 = (*(__IO uint16_t*)PAGE1_BASE_ADDRESS);

  /* Check for invalid header states and repair if necessary */
  switch (PageStatus0)
  {
    case ERASED:
      if (PageStatus1 == VALID_PAGE) /* Page0 erased, Page1 valid */
      {
        /* Erase Page0 */
        FlashStatus = ErasePageInterruptDisabled(PAGE0_BASE_ADDRESS);
        /* If erase operation was failed, a Flash error code is returned */
        if (FlashStatus != FLASH_COMPLETE)
        {
          FLASH_Lock();
          return FlashStatus;
        }
      }
      else if (PageStatus1 == RECEIVE_DATA) /* Page0 erased, Page1 receive */
      {
        /* Erase Page0 */
        FlashStatus = ErasePageInterruptDisabled(PAGE0_BASE_ADDRESS);
        /* If erase operation was failed, a Flash error code is returned */
        if (FlashStatus != FLASH_COMPLETE)
        {
          FLASH_Lock();
          return FlashStatus;
        }
        /* Mark Page1 as valid */
        FlashStatus = FLASH_ProgramHalfWord(PAGE1_BASE_ADDRESS, VALID_PAGE);
        /* If program operation was failed, a Flash error code is returned */
        if (FlashStatus != FLASH_COMPLETE)
        {
          FLASH_Lock();
          return FlashStatus;
        }
      }
      else /* First EEPROM access (Page0&1 are erased) or invalid state -> format EEPROM */
      {
        /* Erase both Page0 and Page1 and set Page0 as valid page */
        FlashStatus = DynamicEE_Format();
        /* If erase/program operation was failed, a Flash error code is returned */
        if (FlashStatus != FLASH_COMPLETE)
        {
          FLASH_Lock();
          return FlashStatus;
        }
      }
      break;

    case RECEIVE_DATA:
      if (PageStatus1 == VALID_PAGE) /* Page0 receive, Page1 valid */
      {
        /* Transfer data from Page1 to Page0 */
        for (VarIdx = 0; VarIdx < eeprom_number_values; VarIdx++)
        {
          if (( *(__IO uint16_t*)(PAGE0_BASE_ADDRESS + 6)) == eeprom_addresses[VarIdx])
          {
            x = VarIdx;
          }
          if (VarIdx != x)
          {
            /* Read the last variables' updates */
            uint32_t DataVar = 0;
            ReadStatus = DynamicEE_ReadVariable(eeprom_addresses[VarIdx], &DataVar);
            /* In case variable corresponding to the virtual address was found */
            if (ReadStatus != 0x1)
            {
              /* Transfer the variable to the Page0 */
              EepromStatus = DynamicEE_VerifyPageFullWriteVariable(eeprom_addresses[VarIdx], DataVar);
              /* If program operation was failed, a Flash error code is returned */
              if (EepromStatus != FLASH_COMPLETE)
              {
                FLASH_Lock();
                return EepromStatus;
              }
            }
          }
        }
        /* Mark Page0 as valid */
        FlashStatus = FLASH_ProgramHalfWord(PAGE0_BASE_ADDRESS, VALID_PAGE);
        /* If program operation was failed, a Flash error code is returned */
        if (FlashStatus != FLASH_COMPLETE)
        {
          FLASH_Lock();
          return FlashStatus;
        }
        /* Erase Page1 */
        FlashStatus = ErasePageInterruptDisabled(PAGE1_BASE_ADDRESS);
        /* If erase operation was failed, a Flash error code is returned */
        if (FlashStatus != FLASH_COMPLETE)
        {
          FLASH_Lock();
          return FlashStatus;
        }
      }
      else if (PageStatus1 == ERASED) /* Page0 receive, Page1 erased */
      {
        /* Erase Page1 */
        FlashStatus = ErasePageInterruptDisabled(PAGE1_BASE_ADDRESS);
        /* If erase operation was failed, a Flash error code is returned */
        if (FlashStatus != FLASH_COMPLETE)
        {
          FLASH_Lock();
          return FlashStatus;
        }
        /* Mark Page0 as valid */
        FlashStatus = FLASH_ProgramHalfWord(PAGE0_BASE_ADDRESS, VALID_PAGE);
        /* If program operation was failed, a Flash error code is returned */
        if (FlashStatus != FLASH_COMPLETE)
        {
          FLASH_Lock();
          return FlashStatus;
        }
      }
      else /* Invalid state -> format eeprom */
      {
        /* Erase both Page0 and Page1 and set Page0 as valid page */
        FlashStatus = DynamicEE_Format();
        /* If erase/program operation was failed, a Flash error code is returned */
        if (FlashStatus != FLASH_COMPLETE)
        {
          FLASH_Lock();
          return FlashStatus;
        }
      }
      break;

    case VALID_PAGE:
      if (PageStatus1 == VALID_PAGE) /* Invalid state -> format eeprom */
      {
        /* Erase both Page0 and Page1 and set Page0 as valid page */
        FlashStatus = DynamicEE_Format();
        /* If erase/program operation was failed, a Flash error code is returned */
        if (FlashStatus != FLASH_COMPLETE)
        {
          FLASH_Lock();
          return FlashStatus;
        }
      }
      else if (PageStatus1 == ERASED) /* Page0 valid, Page1 erased */
      {
        /* Erase Page1 */
        FlashStatus = ErasePageInterruptDisabled(PAGE1_BASE_ADDRESS);
        /* If erase operation was failed, a Flash error code is returned */
        if (FlashStatus != FLASH_COMPLETE)
        {
          FLASH_Lock();
          return FlashStatus;
        }
      }
      else /* Page0 valid, Page1 receive */
      {
        /* Transfer data from Page0 to Page1 */
        for (VarIdx = 0; VarIdx < eeprom_number_values; VarIdx++)
        {
          if ((*(__IO uint16_t*)(PAGE1_BASE_ADDRESS + 6)) == eeprom_addresses[VarIdx])
          {
            x = VarIdx;
          }
          if (VarIdx != x)
          {
            uint32_t DataVar = 0;
            /* Read the last variables' updates */
            ReadStatus = DynamicEE_ReadVariable(eeprom_addresses[VarIdx], &DataVar);
            /* In case variable corresponding to the virtual address was found */
            if (ReadStatus != 0x1)
            {
              /* Transfer the variable to the Page1 */
              EepromStatus = DynamicEE_VerifyPageFullWriteVariable(eeprom_addresses[VarIdx], DataVar);
              /* If program operation was failed, a Flash error code is returned */
              if (EepromStatus != FLASH_COMPLETE)
              {
                FLASH_Lock();
                return EepromStatus;
              }
            }
          }
        }
        /* Mark Page1 as valid */
        FlashStatus = FLASH_ProgramHalfWord(PAGE1_BASE_ADDRESS, VALID_PAGE);
        /* If program operation was failed, a Flash error code is returned */
        if (FlashStatus != FLASH_COMPLETE)
        {
          FLASH_Lock();
          return FlashStatus;
        }
        /* Erase Page0 */
        FlashStatus = ErasePageInterruptDisabled(PAGE0_BASE_ADDRESS);
        /* If erase operation was failed, a Flash error code is returned */
        if (FlashStatus != FLASH_COMPLETE)
        {
          FLASH_Lock();
          return FlashStatus;
        }
      }
      break;

    default:  /* Any other state -> format eeprom */
      /* Erase both Page0 and Page1 and set Page0 as valid page */
      FlashStatus = DynamicEE_Format();
      /* If erase/program operation was failed, a Flash error code is returned */
      if (FlashStatus != FLASH_COMPLETE)
      {
        FLASH_Lock();
        return FlashStatus;
      }
      break;
  }
  FLASH_Lock();
  return FLASH_COMPLETE;
}

/**
  * @brief  Returns the last stored variable data, if found, which correspond to
  *   the passed virtual address
  * @param  VirtAddress: Variable virtual address
  * @param  Data: Global variable contains the read variable value
  * @retval Success or error status:
  *           - 0: if variable was found
  *           - 1: if the variable was not found
  *           - NO_VALID_PAGE: if no valid page was found.
  */
uint16_t DynamicEE_ReadVariable(uint16_t VirtAddress, uint32_t* Data)
{
  uint16_t ValidPage = PAGE0;
  uint16_t AddressValue = 0x0000;
  uint16_t ReadStatus = 1;
  uint32_t Address = EEPROM_START_ADDRESS, PageStartAddress = EEPROM_START_ADDRESS;

  /* Get active Page for read operation */
  ValidPage = DynamicEE_FindValidPage(READ_FROM_VALID_PAGE);

  /* Check if there is no valid page */
  if (ValidPage == NO_VALID_PAGE)
  {
    return  NO_VALID_PAGE;
  }

  /* Get the valid Page start Address */
  PageStartAddress = (uint32_t)(EEPROM_START_ADDRESS + (uint32_t)(ValidPage * PAGE_SIZE));

  /* Get the valid Page end Address */
  Address = (uint32_t)((EEPROM_START_ADDRESS - 4) + (uint32_t)((1 + ValidPage) * PAGE_SIZE));

  /* Check each active page address starting from end */
  while (Address > (PageStartAddress + 2))
  {
    /* Get the current location content to be compared with virtual address */
    AddressValue = (*(__IO uint16_t*)Address);

    /* Compare the read address with the virtual address */
    if (AddressValue == VirtAddress)
    {
      /* Get content of Address-4 which is variable value */
      *Data = (*(__IO uint32_t*)(Address - 4));

      /* In case variable value is read, reset ReadStatus flag */
      ReadStatus = 0;

      break;
    }
    else
    {
      /* Next address location */
      Address = Address - 6;
    }
  }

  /* Return ReadStatus value: (0: variable exist, 1: variable doesn't exist) */
  return ReadStatus;
}

/**
  * @brief  Writes/upadtes variable data in EEPROM.
  * @param  VirtAddress: Variable virtual address
  * @param  Data: 16 bit data to be written
  * @retval Success or error status:
  *           - FLASH_COMPLETE: on success
  *           - PAGE_FULL: if valid page is full
  *           - NO_VALID_PAGE: if no valid page was found
  *           - Flash error code: on write Flash error
  */
uint16_t DynamicEE_WriteVariable(uint16_t* eeprom_addresses, uint16_t eeprom_number_values, uint16_t VirtAddress, uint32_t Data)
{
  uint16_t Status = 0;

  /* Write the variable virtual address and value in the EEPROM */
  Status = DynamicEE_VerifyPageFullWriteVariable(VirtAddress, Data);

  /* In case the EEPROM active page is full */
  if (Status == PAGE_FULL)
  {
    /* Perform Page transfer */
    Status = DynamicEE_PageTransfer(eeprom_addresses, eeprom_number_values, VirtAddress, Data);
  }

  /* Return last operation status */
  return Status;
}

/**
  * @brief  Erases PAGE0 and PAGE1 and writes VALID_PAGE header to PAGE0
  * @param  None
  * @retval Status of the last operation (Flash write or erase) done during
  *         EEPROM formating
  */
FLASH_Status DynamicEE_Format(void)
{
  FLASH_Status FlashStatus = FLASH_COMPLETE;

  FLASH_Unlock();
  
  /* Erase Page0 */
  FlashStatus = ErasePageInterruptDisabled(PAGE0_BASE_ADDRESS);

  /* If erase operation was failed, a Flash error code is returned */
  if (FlashStatus != FLASH_COMPLETE)
  {
    FLASH_Lock();
    return FlashStatus;
  }

  /* Set Page0 as valid page: Write VALID_PAGE at Page0 base address */
  FlashStatus = FLASH_ProgramHalfWord(PAGE0_BASE_ADDRESS, VALID_PAGE);

  /* If program operation was failed, a Flash error code is returned */
  if (FlashStatus != FLASH_COMPLETE)
  {
    FLASH_Lock();
    return FlashStatus;
  }

  /* Erase Page1 */
  FlashStatus = ErasePageInterruptDisabled(PAGE1_BASE_ADDRESS);
  
  FLASH_Lock();
  
  /* Return Page1 erase operation status */
  return FlashStatus;
}

/**
  * @brief  Find valid Page for write or read operation
  * @param  Operation: operation to achieve on the valid page.
  *   This parameter can be one of the following values:
  *     @arg READ_FROM_VALID_PAGE: read operation from valid page
  *     @arg WRITE_IN_VALID_PAGE: write operation from valid page
  * @retval Valid page number (PAGE0 or PAGE1) or NO_VALID_PAGE in case
  *   of no valid page was found
  */
static uint16_t DynamicEE_FindValidPage(uint8_t Operation)
{
  uint16_t PageStatus0 = 6, PageStatus1 = 6;

  /* Get Page0 actual status */
  PageStatus0 = (*(__IO uint16_t*)PAGE0_BASE_ADDRESS);

  /* Get Page1 actual status */
  PageStatus1 = (*(__IO uint16_t*)PAGE1_BASE_ADDRESS);

  /* Write or read operation */
  switch (Operation)
  {
    case WRITE_IN_VALID_PAGE:   /* ---- Write operation ---- */
      if (PageStatus1 == VALID_PAGE)
      {
        /* Page0 receiving data */
        if (PageStatus0 == RECEIVE_DATA)
        {
          return PAGE0;         /* Page0 valid */
        }
        else
        {
          return PAGE1;         /* Page1 valid */
        }
      }
      else if (PageStatus0 == VALID_PAGE)
      {
        /* Page1 receiving data */
        if (PageStatus1 == RECEIVE_DATA)
        {
          return PAGE1;         /* Page1 valid */
        }
        else
        {
          return PAGE0;         /* Page0 valid */
        }
      }
      else
      {
        return NO_VALID_PAGE;   /* No valid Page */
      }

    case READ_FROM_VALID_PAGE:  /* ---- Read operation ---- */
      if (PageStatus0 == VALID_PAGE)
      {
        return PAGE0;           /* Page0 valid */
      }
      else if (PageStatus1 == VALID_PAGE)
      {
        return PAGE1;           /* Page1 valid */
      }
      else
      {
        return NO_VALID_PAGE ;  /* No valid Page */
      }

    default:
      return PAGE0;             /* Page0 valid */
  }
}

/**
  * @brief  Verify if active page is full and Writes variable in EEPROM.
  * @param  VirtAddress: 16 bit virtual address of the variable
  * @param  Data: 16 bit data to be written as variable value
  * @retval Success or error status:
  *           - FLASH_COMPLETE: on success
  *           - PAGE_FULL: if valid page is full
  *           - NO_VALID_PAGE: if no valid page was found
  *           - Flash error code: on write Flash error
  */
static uint16_t DynamicEE_VerifyPageFullWriteVariable(uint16_t VirtAddress, uint32_t Data)
{
  FLASH_Status FlashStatus = FLASH_COMPLETE;
  uint16_t ValidPage = PAGE0;
  uint32_t Address = EEPROM_START_ADDRESS, PageEndAddress = EEPROM_START_ADDRESS+PAGE_SIZE-1;

  /* Get valid Page for write operation */
  ValidPage = DynamicEE_FindValidPage(WRITE_IN_VALID_PAGE);

  /* Check if there is no valid page */
  if (ValidPage == NO_VALID_PAGE)
  {
    return  NO_VALID_PAGE;
  }

  FLASH_Unlock();
  
  /* Get the valid Page start Address */
  Address = (uint32_t)(EEPROM_START_ADDRESS + (uint32_t)(ValidPage * PAGE_SIZE));

  /* Get the valid Page end Address */
  PageEndAddress = (uint32_t)((EEPROM_START_ADDRESS - 2) + (uint32_t)((1 + ValidPage) * PAGE_SIZE));

  /* Check each active page address starting from begining */
  while (Address < PageEndAddress)
  {
    /* Verify if Address and Address+4 contents are 0xFFFFFFFF */
    if ((*(__IO uint32_t*)Address) == 0xFFFFFFFF && (*(__IO uint16_t*)Address) == 0xFFFF)
    {
      /* Set variable data */
      FlashStatus = FLASH_ProgramWord(Address, Data); 
      /* If program operation was failed, a Flash error code is returned */
      if (FlashStatus != FLASH_COMPLETE)
      {
        FLASH_Lock();
        return FlashStatus;
      }
      /* Set variable virtual address */
      FlashStatus = FLASH_ProgramHalfWord(Address + 4, VirtAddress);
      FLASH_Lock();
      /* Return program operation status */
      return FlashStatus;
    }
    else
    {
      /* Next address location */
      Address = Address + 6;
    }
  }
  FLASH_Lock();
  /* Return PAGE_FULL in case the valid page is full */
  return PAGE_FULL;
}

/**
  * @brief  Transfers last updated variables data from the full Page to
  *   an empty one.
  * @param  VirtAddress: 16 bit virtual address of the variable
  * @param  Data: 16 bit data to be written as variable value
  * @retval Success or error status:
  *           - FLASH_COMPLETE: on success
  *           - PAGE_FULL: if valid page is full
  *           - NO_VALID_PAGE: if no valid page was found
  *           - Flash error code: on write Flash error
  */
static uint16_t DynamicEE_PageTransfer(uint16_t* eeprom_addresses, uint16_t eeprom_number_values, uint16_t VirtAddress, uint32_t Data)
{
  FLASH_Status FlashStatus = FLASH_COMPLETE;
  uint32_t NewPageAddress = PAGE1_BASE_ADDRESS, OldPageAddress = PAGE0_BASE_ADDRESS;
  uint16_t ValidPage = PAGE0, VarIdx = 0;
  uint16_t EepromStatus = 0, ReadStatus = 0;

  /* Get active Page for read operation */
  ValidPage = DynamicEE_FindValidPage(READ_FROM_VALID_PAGE);

  if (ValidPage == PAGE1)       /* Page1 valid */
  {
    /* New page address where variable will be moved to */
    NewPageAddress = PAGE0_BASE_ADDRESS;

    /* Old page address where variable will be taken from */
    OldPageAddress = PAGE1_BASE_ADDRESS;
  }
  else if (ValidPage == PAGE0)  /* Page0 valid */
  {
    /* New page address where variable will be moved to */
    NewPageAddress = PAGE1_BASE_ADDRESS;

    /* Old page address where variable will be taken from */
    OldPageAddress = PAGE0_BASE_ADDRESS;
  }
  else
  {
    return NO_VALID_PAGE;       /* No valid Page */
  }
  FLASH_Unlock();
  
  /* Set the new Page status to RECEIVE_DATA status */
  FlashStatus = FLASH_ProgramHalfWord(NewPageAddress, RECEIVE_DATA);
  /* If program operation was failed, a Flash error code is returned */
  if (FlashStatus != FLASH_COMPLETE)
  {
    FLASH_Lock();
    return FlashStatus;
  }

  /* Write the variable passed as parameter in the new active page */
  EepromStatus = DynamicEE_VerifyPageFullWriteVariable(VirtAddress, Data);
  /* If program operation was failed, a Flash error code is returned */
  if (EepromStatus != FLASH_COMPLETE)
  {
    FLASH_Lock();
    return EepromStatus;
  }

  /* Transfer process: transfer variables from old to the new active page */
  for (VarIdx = 0; VarIdx < eeprom_number_values; VarIdx++)
  {
    if (eeprom_addresses[VarIdx] != VirtAddress)  /* Check each variable except the one passed as parameter */
    {
      uint32_t DataVar = 0;
      /* Read the other last variable updates */
      ReadStatus = DynamicEE_ReadVariable(eeprom_addresses[VarIdx], &DataVar);
      /* In case variable corresponding to the virtual address was found */
      if (ReadStatus != 0x1)
      {
        /* Transfer the variable to the new active page */
        EepromStatus = DynamicEE_VerifyPageFullWriteVariable(eeprom_addresses[VarIdx], DataVar);
        /* If program operation was failed, a Flash error code is returned */
        if (EepromStatus != FLASH_COMPLETE)
        {
          FLASH_Lock();
          return EepromStatus;
        }
      }
    }
  }

  /* Erase the old Page: Set old Page status to ERASED status */
  FlashStatus = ErasePageInterruptDisabled(OldPageAddress);
  /* If erase operation was failed, a Flash error code is returned */
  if (FlashStatus != FLASH_COMPLETE)
  {
    FLASH_Lock();
    return FlashStatus;
  }

  /* Set new Page status to VALID_PAGE status */
  FlashStatus = FLASH_ProgramHalfWord(NewPageAddress, VALID_PAGE);
  /* If program operation was failed, a Flash error code is returned */
  if (FlashStatus != FLASH_COMPLETE)
  {
    FLASH_Lock();
    return FlashStatus;
  }
  FLASH_Lock();
  /* Return last operation flash status */
  return FlashStatus;
}


/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

#endif // USING_DYNAMIC_EEPROM
