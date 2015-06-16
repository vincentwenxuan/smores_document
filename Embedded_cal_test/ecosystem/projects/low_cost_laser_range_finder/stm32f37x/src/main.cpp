#include "common_peripherals.h"
#include "system_clock.h"
// common components 
#include "common_message_types.h" 
// common messages
#include "usb_interface.h"
// usb interface
//#include "mBus.h"

// common libraries
#include "project_message_types.h"
#include "error_reporting.h"
#include <string.h> 
//needed for memset
//#define   mBUS 1

// user libraries
#include "SCCB.h"
#include "ov7660.h"
#include "My_Usart.h"
#include "SPI.h"


#define FOCUS
//#define NORMAL

//#define USB_COM
#define SPI

#define PA5             GPIO_Pin_5//PA5

#ifdef NORMAL
#define LINE_MAX        640//240           // the number of line
#define ROW_MAX         480//200           // the number of row
#endif

#ifdef FOCUS
#define LINE_MAX        640//240           // the number of line
#define ROW_MAX         40//200           // the number of row
#endif

#define PA6             GPIO_Pin_6
#define PF7             GPIO_Pin_7

void GPIO_Configuration(void)
{
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOF, ENABLE);
    
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 |GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 |GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);    // DATA lines
    
    //PA6=VSYNC,PF7=HREF
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOF, &GPIO_InitStructure);
    
    //PA5=PCLK
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    

}

#ifdef FOCUS
//***********************  USART configuration   **************************//

void USART1_Send(char *str)
{
    while(*str)
    {
        USART_SendData(USART1,*str++);
        
        while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
        
    }
}

void COM_Send(unsigned char (*img)[LINE_MAX])
{
    unsigned  short i,j;
    
    for(i = 0;i < ROW_MAX;i++)
    {
        for(j=0;j<LINE_MAX;j++)
        {
            
            USART_SendData(USART1,img[i][j]);
            while(USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);
            
        }
    }
    
}
void COM_Send2(unsigned short *maxindex)
{
    unsigned  short i;
    
    unsigned char high_byte,low_byte;
    for(i = 0;i < ROW_MAX;i++)
    {
        high_byte = maxindex[i]>>8;
        low_byte = maxindex[i]& 0xFF;
        USART_SendData(USART1,high_byte);
        while(USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);
        USART_SendData(USART1,low_byte);
        while(USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);
    }
}
#endif

// configure all the peripheral
void Init_All_Periph(void)
{
    u8 DATA_OVM;
    while(1!=OV7660_init());// configureOV7690
    rdOV7660Reg(0x79, &DATA_OVM); //check 3b for previous setting
    while(DATA_OVM!= 0x26);	   // test if write successfully //3b should be 42 in previous settings
    GPIO_Configuration();
    USART_Config();
    SPI3_Init();
}


int main (void){
	InitPeripherals();
    
    Init_All_Periph(); //initialization
#ifdef FOCUS
    unsigned char Image[ROW_MAX][LINE_MAX] = {{0}}, maxval = 0;   // buffer to store the pixel data
#endif
    
#ifdef NORMAL
    unsigned char Image[LINE_MAX] = {0}, maxval = 0;
#endif
    unsigned  short maxind[ROW_MAX] = {0};   // buffer to store the laser pixel position
    unsigned  short row1 = 0,count = 0,line;
    
    volatile unsigned char Disflag = 0;	   // finish symbol

#ifdef USB_COM
    UsbInterface usb = UsbInterface();
    usb.Init();
#endif

	DelayMilliseconds(10);

  //////////////////////////////////////////////////////////////////////////////
  // Main Loop
    while(1) {
        
//        usb.printf("tsec = %d\n\r", t.sec);
//        usb.printf("tnsec = %d\n\r", t.nsec);
//        usb.printf("ctsec = %d\n\r", currentTime.sec);
//        usb.printf("ctnsec = %d\n\r", currentTime.nsec);
//        usb.SendNow();
        row1 = 0;
        while(GPIOA->IDR & PA6);
        while(!(GPIOA->IDR & PA6));	//Wait while v-sync is still low
        while(GPIOA->IDR & PA6)    //While v-sync is high
        {
            line = 0;
            while(!(GPIOF->IDR & PF7));    //wait while h-ref is still low
            while((GPIOF->IDR & PF7))		//while h-ref is high
            {
                while(!(GPIOA->IDR & PA5));   //Wait while pclk is still low
#ifdef FOCUS
                if(line < LINE_MAX)
                {
                    Image[row1][line] = (GPIOB->IDR & 0xff);
                    
                }
#endif
                
#ifdef NORMAL
                Image[line] = (GPIOB->IDR & 0xff);
#endif
                while((GPIOF->IDR & PA5)); //Wait while pclk is still high
                line++;
            }
            maxval = 0;
            for (int a = 0;a<LINE_MAX;a++) //once line is finished find max value in the line
            {
#ifdef FOCUS
                if(Image[row1][a] > maxval)
                {
                    maxind[row1] = a;
                    maxval = Image[row1][a];
                }
#endif
                
#ifdef NORMAL
                if(Image[a]>maxval)
                {
                    maxind[row1] = a;
                    maxval = Image[a];
                }
#endif
            }
            if(row1 == 0)
            {
#ifdef SPI
                SPI3->DR = 0xABCD; //Send data indicating the start of frame
#endif
#ifdef USB_COM
                usb.printf("%d", 0xABCD);
                usb.SendNow();
#endif
            }
            else
            {
#ifdef SPI
                SPI3->DR = maxind[row1] | ((row1 & 1) << 15); //Send max value over SPI
#endif
#ifdef USB_COM
                usb.printf("%d", maxind[row1] | ((row1 & 1) << 15));
                usb.SendNow();
#endif
            }
            row1++;
            if (row1 > (ROW_MAX-1))
            {
                Disflag = 1;
                count = count + 1;
                break;
            }
        }
        
#ifdef NORMAL
        if (Disflag)
        {
            Disflag = 0;
#ifdef SPI
            SPI3->DR = 0x1111;  // send data indicating the end of the frame
#endif
#ifdef USB_COM
            usb.printf("%d", 0x1111);
            usb.SendNow();
#endif
            DelayMilliseconds(15);
#endif
            
#ifdef FOCUS
            if(Disflag && (count == 2)) // transmit the data, use count to control which frame you want
            {
                Disflag = 0;
                USART1_Send("s");
                COM_Send(Image);
                USART1_Send("e");
                DelayMilliseconds(500);
                DelayMilliseconds(500);
                DelayMilliseconds(1000);
                USART1_Send("s");
                COM_Send2(maxind);
                USART1_Send("e");
#endif			
            }
    } // while(1)
    return(0);
}

