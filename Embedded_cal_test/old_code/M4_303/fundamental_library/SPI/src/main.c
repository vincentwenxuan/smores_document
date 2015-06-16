#include "mGeneral.h"
#include "SPI.h"
int main( void )
{
    uint8_t d = 0x34,rxbuf;
    mInit();
    SPI_config();
    mYellowON;
    while(1)
    {
        GPIO_ResetBits(GPIOA, GPIO_Pin_4);
        mWaitms(1000);
        SPI_SendData8(SPI1, d);
        while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
        while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
        rxbuf =  SPI_ReceiveData8(SPI1);
    }

}