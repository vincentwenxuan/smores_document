#include "mGeneral.h"
#include "USARTdebug.h"
int main( void )
{
    mInit();
    debug_setup();
    mYellowON;
    while(1)
    {
        mWaitms(10);
        debug_write_line("Hello World");
        USART_SendData(DEBUG_USART, 0x12);
        // printf("Enter\r\n");
        // scanf("%d", &a);
        // mGreenON;
        // printf("Read: %d\r\n",a);
        // mGreenOFF;
        // mWaitms(1000);
    }

}