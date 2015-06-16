#ifndef EXCEPTIONON_HANDLER_IT_H
#define EXCEPTIONON_HANDLER_IT_H

#include "common_peripherals.h"
#include STM32FXXX_H

#ifdef __cplusplus
 extern "C" {
#endif 

void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);
//void SysTick_Handler(void);
//void USB_HP_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif // EXCEPTIONON_HANDLER_IT_H