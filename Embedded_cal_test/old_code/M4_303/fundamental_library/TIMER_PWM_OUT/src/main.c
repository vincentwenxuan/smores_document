#include "mGeneral.h"
#include "I2C_V2.h"
#include "SPI.h"
#include "USARTdebug.h"
#include "tim_pwm_out.h"
#define SLAVE_ADDR  (0x69 & 0x7f) << 1
int main( void )
{
    uint8_t d = 0x34,rxbuf;
    mInit();
    I2C_config();
    SPI_config();
    debug_setup();
    //         I2C_write_1byte(SLAVE_ADDR,0X6B,0X01);
    // // I2C_write_1byte(MIMU,0x1C,accel_scale<<3);

    // // SET THE GYRO FULL-SCALE +/- RANGE (0=250, 1=500, 2=1000, 3=2000 DEG/SEC)
    // // reg: 0x1B (GYRO_CONFIG)
    // // bits: 3-4 set +/- full-scale range
    // // value: 0x00
    // // I2C_write_1byte(MIMU,0x1B,gyro_scale<<3); // this shouldn't be doing anything
      
    // // SET THE MPU INTO I2C BYPASS MODE
    // // reg: 0x37 (INT_PIN_CFG)
    // // bit: 1 (1=bypass, 0=normal op)
    // I2C_write_1byte(SLAVE_ADDR,0x37,0x02); // switch the MPU into bypass mode
    
    // // CONFIGURE THE MAGNETOMETER
    // // address: 0x1E (for the magnetometer)
    // // reg: 2 (mode register)
    // // val: 0 (continuous output)
    // // I2C_write_1byte(0x1E,2,0); // set the mag. to continuous output mode
    
    // // SET THE MPU TO NORMAL I2C MODE
    // // reg: 0x37 (INT_PIN_CFG)
    // // bit: 5 (1=master, 0=passthrough)
    // I2C_write_1byte(SLAVE_ADDR,0x37,0);    // switch the MPU out of bypass mode

    // // SET THE AUXILLIARY I2C SAMPLE RATE
    // // reg: 0x34 (I2C_SLC4_CTRL) 
    // // value: 0x1E (30 > 8000/30 = 266.6Hz)
    // // reg: 0x67 (MST_DELAY_CTRL)
    // // bit: 0 (1=slowed down, 0=full speed)
    // I2C_write_1byte(SLAVE_ADDR,0x34,0x1E);
    // I2C_write_1byte(SLAVE_ADDR,0x67,0x01);

    // // ADJUST the auxilliary I2C port clock speed, etc.
    // // reg: 0x24 (MST_CTRL)
    // // bit: 4 = 1 (stop then start), 2 (restart)
    // // bit: 0-3 = clock prescaler (0x0D = 400kHz)
    // // not necessary, perhaps.  We shall see?
    // I2C_write_1byte(SLAVE_ADDR,0x24,0x1D);

    // // SET THE MAGNETOMETER ADDRESS
    // // reg: 0x25 (I2C_SLC0_ADDR)
    // // value: 0x1E (from the magnetometer datasheet)
    // // notes: for read from slave, set bit 7 to 1; to write, set bit 7 to 0
    // I2C_write_1byte(SLAVE_ADDR,0x25,0x9E);

    // // SET THE MAGNETOMETER DATA START ADDRESS
    // // reg: 0x26 (I2C_SLV0_REG)
    // // val: 3 (X high byte)
    // I2C_write_1byte(SLAVE_ADDR,0x26,3);
    
    // // OTHER AUXILLIARY I2C SETTINGS
    // // reg: 0x27 (I2C_SLV0_CTRL
    // // bit: 0-3 (# of bytes to ingest) = 6
    // //      4: byte/word grouping control
    // //      6: swap bytes
    // //      7: enable slave (1=on, 0=off)
    // I2C_write_1byte(SLAVE_ADDR,0x27,0x96);
    
    // // START THE AUXILLIARY I2C PORT
    // // reg: 0x6A (USER_CTRL)
    // I2C_write_1byte(SLAVE_ADDR,0x6A,0x20); // enable master mode

    mYellowON;
    TIMA_config(100, 20,10); // timer1 channel 1 PA8
    TIMB_config(100, 20,10); // timer2 channel 3 PA9
    TIMC_config(100, 20,10); // timer15 channel 1 PB14
    TIMD_config(100, 20,10); // timer2 channel 2 PA1 // first 2 numbers
                             // in the function are not doing anything   
    while(1)
    {
        GPIO_ResetBits(GPIOA, GPIO_Pin_4);
        mBlueTOGGLE;
        mWaitus(30);
        // rxbuf = I2C_read_1byte(SLAVE_ADDR,0X3F);
        debug_write_ch(0XAB);
        mYellowTOGGLE;
         mWaitms(300);
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