#include "mGeneral.h"
#include "I2C_V2.h"
#include "SPI.h"
#include "USARTdebug.h"
#include "tim_pwm_out.h"
#include "lib_adc.h"
#define SLAVE_ADDR  (0x40 & 0x7f) << 1
#define SLAVE_ADDR1  (0x41 & 0x7f) << 1
#define SLAVE_ADDR2  (0x42 & 0x7f) << 1
__IO uint16_t  ADC1ConvertedValue = 0;

int main( void )
{
    uint8_t d = 0x34,rxbuf = 0X23;
    mInit();
    // I2C_config();
    SPI_config();
    debug_setup();
    ADC_A_config();
    GPIO_InitTypeDef  GPIO_InitStructure;

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; //M4 DIRECTION
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15; //M3 DIRECTION
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
    TIMA_config(100, 20,10); // timer1 channel 1 PA8
    TIMB_config(100, 20,10); // timer2 channel 3 PA9
    TIMC_config(100, 20,10); // timer15 channel 1 PB14
    TIMD_config(100, 20,10); // timer2 channel 2 PA1 // first 2 numbers
    //                          // in this function are not doing anything   
    while(1)
    {
        GPIO_ResetBits(GPIOA, GPIO_Pin_0);
        GPIO_ResetBits(GPIOB, GPIO_Pin_15);
        // debug_write_ch(0XAB);
        mBlueTOGGLE;
        mGreenTOGGLE;
        mRedTOGGLE;
        mWaitms(300);
        GPIO_SetBits(GPIOA, GPIO_Pin_0);
        GPIO_SetBits(GPIOB, GPIO_Pin_15);
            /* Test EOC flag */
        while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
    
        /* Get ADC1 converted data */
        ADC1ConvertedValue =ADC_GetConversionValue(ADC1);
        debug_write_u16(ADC1ConvertedValue,10);
        debug_write_line("  ");

        // mWaitms(300);
        // I2C_write_1byte(SLAVE_ADDR2,254,22);
        // rxbuf = I2C_read_1byte(SLAVE_ADDR,254);
        // debug_write_ch(rxbuf);
        // rxbuf = I2C_read_1byte(SLAVE_ADDR1,254);
        // debug_write_ch(rxbuf);
        // rxbuf = I2C_read_1byte(SLAVE_ADDR2,254);
        // debug_write_ch(rxbuf);
    // TIM_SetCompare2 ( TIM2,1);
    // mWaitms(300);
    // TIM_SetCompare2 ( TIM2,0);
    // mWaitms(1000);
        // SPI_SendData8(SPI1, d);
        // while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
        // while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
        // rxbuf =  SPI_ReceiveData8(SPI1);
    }

}