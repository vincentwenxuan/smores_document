#include "mGeneral.h"
#include "I2C_V2.h"
#include "SPI.h"
#include "USARTdebug.h"
#include "tim_pwm_out.h"
#include "lib_adc.h"
#define SLAVE_ADDR  (0x0A & 0x7f) << 1
#define SLAVE_ADDR1  (0x0B & 0x7f) << 1
#define SLAVE_ADDR2  (0x42 & 0x7f) << 1
__IO uint16_t  ADC1ConvertedValue = 0;
int main( void )
{
    uint8_t data[5] = {'1','2','3','4','5'};
    uint8_t rxbuf[7],rxbuf3;
    uint8_t *rxbuf2;
    mInit();
    I2C_config();
    // SPI_config();
    debug_setup();
    // ADC_A_config();
    motor_dir_init();
    TIMA_config(100, 20,10); // timer1 channel 1 PA8
    TIMB_config(100, 20,10); // timer2 channel 3 PA9
    TIMC_config(100, 20,10); // timer15 channel 1 PB14
    TIMD_config(100, 20,10); // timer2 channel 2 PA1 // first 2 numbers
    //                          // in this function are not doing anything   
    mWaitms(300);//Wait for atmegas to be ready
    while(1)
    {
        GPIO_ResetBits(GPIOA, GPIO_Pin_0);
        GPIO_ResetBits(GPIOB, GPIO_Pin_15);
        debug_write_ch(0XAB);
        mGreenTOGGLE;
        mWhiteTOGGLE;
        mWaitms(300);
        GPIO_SetBits(GPIOA, GPIO_Pin_0);
        GPIO_SetBits(GPIOB, GPIO_Pin_15);
        // mWait(300);
        //I2C_write(SLAVE_ADDR,0xA0,data,2);
        // I2C_write_1byte(SLAVE_ADDR,0xA0,0x00);
        mWait(500);
        I2C_write_1byte(SLAVE_ADDR,0xA0,0x00);
        //mWait(1000);
        // rxbuf2 = I2C_read(SLAVE_ADDR,0xA0,rxbuf,6);
        // for (int a=0;a<6;a++)
        // debug_write_ch(rxbuf2[a]);
          //      mWait(300);
        //rxbuf3 = I2C_read_1byte(SLAVE_ADDR,0xA0);
       // debug_write_ch(rxbuf3);
                mWait(300);
        // rxbuf = I2C_read_1byte(SLAVE_ADDR,2);
        // debug_write_ch(rxbuf);
        //         mWait(300);
        // rxbuf = I2C_read_1byte(SLAVE_ADDR,3);
        // debug_write_ch(rxbuf);
        //         mWait(300);
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
