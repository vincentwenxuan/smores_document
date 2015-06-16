/**
 * @author 
 * 	* Cedric Destin
 * @date
 * 	* 07/22/2013
 * @brief:
 * I2C config for the MPL
 * Temperature and barometer
 * Ref pressure = 101.325kPa(Sea level)
 * Setup for MPL311 (No interrupt yet, No usb interface, Temperature is a whole number,)
 * Altitude mode: 1
 * Barometer mode: 0
 * Temperature: always 1
 * I2C1
 * SCL: PA15
 * SDA: PA14
 * This is subject to be changed... to have a MPL311.c DONE
 * Interupts are sets and seem to work...
 * The interupts are:
 * 	DRDY: Data Ready, mode 0
 * 	PTH: Pressure threshold/ Altitude threshold, mode 1
 * 	PWTH: Pressure/ Altitude (outside), mode 2
 * 	PW: Pressure/ Altitude Window (inside), mode 3
 * 	None: mode ~ anything else
 * Notes: Blowing on the Barometer messes up with the altitude... 
 * I am not sure how to describe it... I will run some more test...
 * It gets somewhat close to the height... so I will run a test at 5
 * different heights and matching the actual and experimented heights
 **/
/**
 * @include
 * Common Header: 
 * 	stm32f37x
 * 	mGeneral
 * 	common_peripherals
 * 	stdio
 **/
#include "stm32f37x.h"
#include "mGeneral.h"
#include "common_peripherals.h"
#include "stdio.h"
/** Include your custom headers here
 * @brief
 * 	MPL311.h, contains the structs used for altitude reading and temperature reading
 **/
#include "MPL311.h"
#include "project_message_types.h"
#include "usb_interface.h"
/** Initialize variables here
 * @var MPL
 * 	MPL: Address of the MPL
 * @var ret
 * 	ret: Use to check successes/failures
 * @var Bias
 * 	Bias: Used to calibrate the altitude, for 10 cycles
 * @var GetAlt
 * 	GetAlt: Get the altitude from altitude register (meter)
 * @var CalAlt
 * 	CalAlt: Calibrates the altitude, for 10cycles
 * @var GetTemp
 * 	GetTemp: Get Temperature (Celcius)
 * @var ChkI2C
 * 	ChkI2C: Checks is device is ready to read...
 **/ 
#define MPL 0b11000000
uint8_t ret = 0;	// Dummy variable
uint8_t n = 0;		// Dummy loop variable
float Bias = 0;		// Bias to calibrate the altitude with reference to initial position
uint8_t data[5];		// Data array
uint8_t Intdata[6];	// Interupt data array
/** Function prototypes
 * @fn ChkI2C
 * 	@param[in] Adrs Slave's Address
 * 	@param[in] I2Cx I2C peripheral to use
 * 	@param[out] none
 * 	@brief this will check if the device is ready, this is
 * 	specific to the barometer
 * @fn CalAlt
 * 	@param[in] I2Cx
 * 	@brief this is to calibrate the altitude sensor (running avg)
 * @fn GetAlt
 * 	@param[in] I2Cx
 * 	@brief this will get the altitude only and store it in the
 * 		global struct
 * @fn GetTemp
 * 	@param[in]I2Cx
 * 	@brief saves the temperature in global struct
 * @fn GetDataBurst
 * 	@param[in] length how many bytes are to be read
 * 	@param[in] I2Cx
 * 	@param[in] *data pointer where to save the data read thru
 * 	@param[in] Regaddr what register to read the data from
 * 	@brief this function is used to read data through the burst mode
 * @fn MPLINT
 * 	@param[in] mode what mode of interrupt
 * 	@param[in] *Intdata Interupt data for setting threshold...
 * 	@param[in] length used for the Writeburst, so that we can
 *	 write the threshold for either the altitude or barometer
 **/ 
void ChkI2C(uint8_t Adrs, uint8_t I2Cx); // Checks I2C reading
void GetAlt(uint8_t I2Cx);		// This gets the altitude in meter
void CalAlt(uint8_t I2Cx);		// Calibrates the altitude with reference to the initial position
void GetTemp(uint8_t I2Cx);
void GetDataBurst(uint8_t length, uint8_t I2Cx, uint8_t* data, uint8_t Regaddr);	// Get Data using Burst Mode
void MPLINT(uint8_t mode, uint8_t* Intdata, uint8_t length);			// Set up the interupts
// initialize USB interface object
/** Parameters
 * @param usb_interface
 * @param Altitude
 * 	* Custom made struct to store altitude
 * @param Temp
 * 	* Custom made strcut to store the temperature
 * @param GPIO_InitStructure
 * 	* GPIO structure contains all setup for pins config
 **/

struct ALTITUDE Altitude;		// Simple struct to store the altitude
struct TEMP Temp;			// Simple struc to store the temperature
GPIO_InitTypeDef GPIO_InitStructure;
uint8_t tx_buf[136];
uint8_t tx_len;
/** Main
 * 		
 **/
int main(void){
  InitPeripherals();  // configure GPIO, USB, I2C, ect based on peripherals.h
  InitUsbInterface();
   __asm__(".global _printf_float");
  __asm__(".global _scanf_float");
  /// Initialize code here
  /** Set PA.2 for input interrupt
   * That is easy Cedric
   **/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; // Set pin 2 as input to read interupts
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  /** Check if MPL is ready
   * This implies talking to the device through I2C 
   **/
   ret = mBusPeriphReady(MPL,1);
   while(!ret){				// Check if the MPL is ready
     ret = mBusPeriphReady(MPL,1);
     mRedON;
   }
  /** Control register 1
   * Set the Barometer as (38)
   * 	* Altimeter mode 0
   *	* RAW 0
   * 	* OS[2:0] 111
   **/
   mRedOFF;
  ret = mBusWrite(MPL, 0x26, 0x38, 1);
  while(!ret){				// Check if the writing was successful
    ret = mBusWrite(MPL, 0x26, 0x38, 1);
    mRedON;
  }
  mRedOFF;
  /** Interupt register (PT_DATA_CFG)
   * Setup an interrupt
   * This is kind of complicated but not really, it is related to the control register #
   **/
  ret = mBusWrite(MPL, 0x13, 0x07, 1);
  while(!ret){
    ret = mBusWrite(MPL, 0x13, 0x07, 1);
    mRedON;
  }
  mRedOFF;
  /** Calibrate the altitude
   * 	* make sure you bias the output so that they give true values related to the altitude
   * 	* also make sure you take care of negative stuff
   **/
  
  /** Checklist
   * 1) Does the I2C work
   * 	* Yes
   * 2) Can you read and bias altitude measurements that make sense
   * 	* Yes, the data is noisy tho TODO get the frequency
   * 3) Can you send your data thru USB using Jimmy's
   * 	* Yes that is done
   * 4) Can you plot them on python
   * 	* Yes, but I must detail. Title, x label...
   * 5) Apply Fan
   * 	* yep, must save data
   * 6) Find out how fast is can you read data
   **/
  /// Setup to no interupts
  MPLINT(4, Intdata, 0);
  /// Calibrate the altitude 
  CalAlt(1);
  /// Continuous loop
  while(1){
    /// Read using the busrt mode
    GetDataBurst(3, 1, data, 0x01);
    /// Set up CDC to send packet data later to be caught by python
    uint8_t msg_type = 9;
    
    /// Packing data into struct
    MsgMyData data_out;
    data_out.float_var = Altitude.valWord;
    data_out.float_var2 = Bias;
    
    /// Packetize the data
    FormPacket(msg_type, (uint8_t*)&data_out, sizeof(MsgMyData),
               tx_buf, &tx_len);

    /// Send
    CDC_Send_DATA(tx_buf, tx_len);
  }
}
void ChkI2C(uint8_t Adrs, uint8_t I2Cx){
  ret = mBusRead(Adrs, 0x00, I2Cx);
    while((ret & 0x08) == 0){
      /// Checks if the MPL is ready to read i am not sure why it is ended
      ret = mBusRead(Adrs, 0x00, I2Cx);
      mRedON;
    }
    mRedOFF;
}
void CalAlt(uint8_t I2Cx){
  while(n < 5){
    ChkI2C(MPL, 1);
    Altitude.hiByte = mBusRead(MPL, 0x01, I2Cx);
    Altitude.loByte = mBusRead(MPL, 0x02, I2Cx);
    Altitude.fracNib = mBusRead(MPL, 0x03, I2Cx);
    if(Altitude.hiByte > 0x7F){
      //printf("\rneg \t %d\n", Altitude.hiByte);
      Altitude.valWord = ~Altitude.hiByte + 1;
    }
    Altitude.valWord = (float)(Altitude.hiByte * 256  + Altitude.loByte);
    float fret = 0;
    if(Altitude.fracNib & 0b10000000){
      fret += 0.5;
    }
    if(Altitude.fracNib & 0b01000000){
      fret += 0.25;
    }
    if(Altitude.fracNib & 0b00100000){
      fret += 0.125;
    }
    if(Altitude.fracNib & 0b00010000){
      fret += 0.0625;
    }
    Altitude.valWord = Altitude.valWord + fret;
    printf("\rAltitude = %f\n", Altitude.valWord);
    Bias = Altitude.valWord + Bias;
    //printf("\rBias = %f\n", Bias);
    n = n + 1;
  }
  Bias = Bias / n;
  printf("\r%f\n", Bias);
}
void GetAlt(uint8_t I2Cx){
  ChkI2C(MPL, I2Cx);
  Altitude.hiByte = mBusRead(MPL, 0x01, I2Cx);
  Altitude.loByte = mBusRead(MPL, 0x02, I2Cx);
  Altitude.fracNib = mBusRead(MPL, 0x03, I2Cx);
  if(Altitude.hiByte > 0x7F){
    printf("\rneg \t %d\n", Altitude.hiByte);
    Altitude.valWord = ~Altitude.hiByte + 1;
  }else{
    printf("\rpos\t %d\n", Altitude.hiByte);
  }
  Altitude.valWord = (float)(Altitude.hiByte * 256  + Altitude.loByte);
  float fret = 0;
  if(Altitude.fracNib & 0b10000000){
    fret += 0.5;
  }
  if(Altitude.fracNib & 0b01000000){
    fret += 0.25;
  }
  if(Altitude.fracNib & 0b00100000){
    fret += 0.125;
  }
  if(Altitude.fracNib & 0b00010000){
    fret += 0.0625;
  }
  Altitude.valWord = Altitude.valWord + fret - Bias;
  //printf("\rBias = %f\n", Bias);
  //printf("\rAltitude = %f\n", Altitude.valWord);
}
void GetTemp(uint8_t I2Cx){
  ChkI2C(MPL, I2Cx);
  Temp.TByte = mBusRead(MPL, 0x04, I2Cx);
  Temp.fracTNib = mBusRead(MPL, 0x05, I2Cx);
  Temp.valTWord = (float)(Temp.TByte);
  float fret = 0;
  if(Temp.fracTNib & 0b10000000){
    fret += 0.5;
  }
  if(Temp.fracTNib & 0b01000000){
    fret += 0.25;
  }
  if(Temp.fracTNib & 0b00100000){
    fret += 0.125;
  }
  if(Temp.fracTNib & 0b00010000){
    fret += 0.0625;
  }
}
void GetDataBurst(uint8_t length, uint8_t I2Cx, uint8_t* data, uint8_t Regaddr){
 ChkI2C(MPL, I2Cx);
 
 ret = mBusReadBurstStartNB(MPL, Regaddr, length, I2Cx);
  while(!ret){
    ret = mBusReadBurstStartNB(MPL, Regaddr, length, I2Cx);
    mYellowON;
  } 
  mYellowOFF;
 //_delay_ms(500);
  ret = mBusReadBurstDataNB(length, data, I2Cx);
  while(!ret){
    ret = mBusReadBurstDataNB(length, data, I2Cx);
    //mWaitms(100);
    mYellowON;
  }
  mYellowOFF;
  Altitude.hiByte = data[0];
  Altitude.loByte = data[1];
  Altitude.fracNib = data[2];
  Temp.TByte = data[3];
  if(data[0] > 0x7F){
    data[0] = ~data[0] + 1;
  }
  Altitude.valWord = (float)(Altitude.hiByte * 256 + Altitude.loByte);
  float fret = 0;
  if(Altitude.fracNib & 0b10000000){
    fret += 0.5;
  }
  if(Altitude.fracNib & 0b01000000){
    fret += 0.25;
  }
  if(Altitude.fracNib & 0b00100000){
    fret += 0.125;
  }
  if(Altitude.fracNib & 0b00010000){
    fret += 0.0625;
  }
  Altitude.valWord = Altitude.valWord + fret - Bias;
  printf("\rAltitude = %f\n", Altitude.valWord);
}
void MPLINT(uint8_t mode, uint8_t* Intdata, uint8_t length){
  switch(mode){
    case 0:		/// Data ready interrupt
      mGreenON;
      ret = mBusWrite(MPL, 0x28, 0x11, 1);
      while(!ret){				// Check if the writing was successful
	ret = mBusWrite(MPL, 0x28, 0x11, 1);
      }
      ret = mBusWrite(MPL, 0x29, 0x80, 1);
      while(!ret){				// Check if the writing was successful
	ret = mBusWrite(MPL, 0x29, 0x80, 1);
      }
      ret = mBusWrite(MPL, 0x26, 0xB9, 1);
      while(!ret){				// Check if the writing was successful
	ret = mBusWrite(MPL, 0x26, 0xB9, 1);
      }
      break;
    case 1:		/// Threshold for altitude
      //mYellowON;
      ret = mBusWrite(MPL, 0x28, 0x11, 1);
      while(!ret){				// Check if the writing was successful
	ret = mBusWrite(MPL, 0x28, 0x11, 1);
      }
      ret = mBusWrite(MPL, 0x29, 0x08, 1);
      while(!ret){				// Check if the writing was successful
	ret = mBusWrite(MPL, 0x29, 0x08, 1);
      }
      ret = mBusWriteBurstNB(MPL, 0x1A, length, Intdata, 1); //length = 6
      ret = mBusWrite(MPL, 0x2A, 0x08, 1);
      while(!ret){				// Check if the writing was successful
	ret = mBusWrite(MPL, 0x2A, 0x08, 1);
      }
      ret = mBusWrite(MPL, 0x26, 0xB9, 1);
      while(!ret){				// Check if the writing was successful
	ret = mBusWrite(MPL, 0x26, 0xB9, 1);
      }
      break;
    case 2:	/// Window threshold
      mRedON;
      ret = mBusWrite(MPL, 0x28, 0x11, 1);
      while(!ret){				// Check if the writing was successful
	ret = mBusWrite(MPL, 0x28, 0x11, 1);
      }
      ret = mBusWrite(MPL, 0x29, 0x08, 1);
      while(!ret){				// Check if the writing was successful
	ret = mBusWrite(MPL, 0x29, 0x08, 1);
      }
      ret = mBusWriteBurstNB(MPL, 0x1A, length, Intdata, 1); //length = 6
      ret = mBusWrite(MPL, 0x2A, 0x08, 1);
      while(!ret){				// Check if the writing was successful
	ret = mBusWrite(MPL, 0x2A, 0x08, 1);
      }
      ret = mBusWrite(MPL, 0x26, 0xB9, 1);
      while(!ret){				// Check if the writing was successful
	ret = mBusWrite(MPL, 0x26, 0xB9, 1);
      }
      break;
    case 3:	/// Window (outside)
      mWhiteON;
      ret = mBusWrite(MPL, 0x28, 0x11, 1);
      while(!ret){				// Check if the writing was successful
	ret = mBusWrite(MPL, 0x28, 0x11, 1);
      }
      ret = mBusWrite(MPL, 0x29, 0x08, 1);
      while(!ret){				// Check if the writing was successful
	ret = mBusWrite(MPL, 0x29, 0x08, 1);
      }
      mBusWriteBurstNB(MPL, 0x1A, length, Intdata, 1); //length = 6
      ret = mBusWrite(MPL, 0x2A, 0x08, 1);
      while(!ret){				// Check if the writing was successful
	ret = mBusWrite(MPL, 0x2A, 0x08, 1);
      }
      ret = mBusWrite(MPL, 0x26, 0xB9, 1);
      while(!ret){				// Check if the writing was successful
	ret = mBusWrite(MPL, 0x26, 0xB9, 1);
      }
      break;
    default:	/// No interrupt
      mBusWrite(MPL, 0x26, 0x39, 1);
      ChkI2C(MPL, 1);
      break;    
  }
}
