/*
 * Cedric Destin
 */
/* Includes
 * 
 */
#include "usart.h"
/* Initialized Variables
 * 
 */
/* Usart2Init
 * 
 */
void Usart2Init(void){
  /* 
   */ 
  GPIO_InitTypeDef GPIO_InitStructure; 
  /*This may not be necessary, since it is defined in peripherals.c
   * I am guseeing that redefining it may reset the registers...
   */
  USART_InitTypeDef USART_InitStructure;
  /* USART Structure
   * 
   */
  USART_ClockInitTypeDef USART_ClockInitStructure;
  /* Clock Structure defined in stm32f37x_usart.h
    * uint32_t USART_Clock
      * Specifies if the USART clock is enabled
	* USART_Clock_Disable
	* USART_Clock_Enable
    * uint32_t USART_CPOL
    * uint32_t USART_CPHA
    * uint32_t USART_LastBit
   */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
  /* Gate the APB1 clock
   * Enable USART2
   */
  
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_7);
  
  //RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  /* I am not sure about the Mode
   * 
   */
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  /* Setting Pin 3...
   * 
   */
  USART_ClockStructInit(&USART_ClockInitStructure);
  /* Fills each member of the struct with default
   *
   */
  USART_ClockInit(USART2, &USART_ClockInitStructure);
  /* USART2 clock is set up
   * 
   */
  USART_InitStructure.USART_BaudRate = 9600;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_Mode = USART_Mode_Rx;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  /* Set the USART Struct
   * 
   */
  USART_Init(USART2, &USART_InitStructure);
  USART_Cmd(USART2, ENABLE);
  /* Write Parameter to Registers
   * 
   */
  
}
void Usart2Put(uint8_t ch){
  USART_SendData(USART2, (uint8_t) ch);
  while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET){
    /* Wait until the transmission flag is good
     * 
     */
    mRedON;
    mWaitms(10);
    printf("Sending");
  }
  mRedOFF;
}
uint8_t Usart2Get(void){
    while(USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET){
      /* 
       * Get status of Receive flag
       */
      mRedON;
      //printf("\rCan't Receive\n");
      mWaitms(100);
    }
    mRedOFF;
    return (uint8_t)USART_ReceiveData(USART2);
}