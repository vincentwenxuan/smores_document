/**
  *****************************************************************************************
  * @file   SMORES_periphs.c 
  * @author  Chaitanya Bhargava (cbhar)
  * @version V1.1.0
  * @date    10-September-2014
  * @brief   Initializes USART, I2C slave , ADC and PWM on the atmega. The pins are configures  
  *			 according to the SMORES connections. Also has functions to get ADC values, communicate 
  *			 and send pulses over the coils. Settings for the peripherals are in 
  *			 SMORES_platform_config.
  * @NOTE	 Since the pulses required to turn the coils on and off are not perfectly known
  *			 the functions available have the ability to take the parameters of the pulses
  *			 as inputs. This may change in future versions
  *****************************************************************************************
*/

//*****************************************************************************************
//
//! \addtogroup SMORES_peripherals_apis
//! @{
//
//*****************************************************************************************

 
#include "SMORES_periphs.h"

/*-------------------global variables-----------------------*/
uint8_t address_pins_value = 0;
uint16_t adc_val[2];
volatile uint8_t usart_rcv_buf = 0, newusart_data = 0;
uint8_t usart_buf[4];
uint8_t nos_times_usart_send = 1;

/*-------------------User Functions-------------------------*/
//**************************************************************************************
//
//!  init_all_periphs
//!
//!  @param    none
//!
//!  @return   none
//!
//!  @brief    This function will initialize all the peripherals required by SMORES
//!			   
//
//**************************************************************************************
void init_all_periphs()
{	
	/*Initialize USART. See USART.c for more details*/
	
	USART USART_CHANNEL;
	USART_CHANNEL.USART_BAUDRATE = SMORES_baudrate;
	USART_CHANNEL.USART_RECEIVER_STATUS = RECEIVER_ENABLE;
	USART_CHANNEL.USART_TRANSMITTER_STATUS = TRANSMITTER_DISABLE;
	USART_CHANNEL.USART_STOP_BIT = SMORES_stop_bit;
	USART_CHANNEL.USART_PARITY = SMORES_parity;
	USART_Init(USART_CHANNEL);
	
	/*Initialize I2C. See TWI.c for more details*/
	clear(I2C_add_config_DDR,I2C_add_config_PIN_num1);			// Set I2C address config pins to be inputs 
	clear(I2C_add_config_DDR,I2C_add_config_PIN_num2);
		
	set( I2C_add_config_PORT,I2C_add_config_PIN_num1);			//Enable internal pull up resistors of the 
	set( I2C_add_config_PORT,I2C_add_config_PIN_num2);			//address config pins. Solder them to ground externally
																//for different addresses
	address_pins_value = (check(PINB,6) | (check(PINB,7)<<1));	
	/* Initialize TWI module for slave operation*/
	TWI_Slave_Initialise( ((Base_face_add+address_pins_value)<<1) & 0xfe);
	TWI_Start_Transceiver();
	
	/*Initialize ADC channel. See ADC.c for more details*/
	ADC_Init ADC_start;
	ADC_start.ADC_CHANNEL = SMORES_ADC0;
	ADC_start.ADC_REF = VCC;
	ADC_start.ADC_PRESCALAR = SMORES_ADC_PRESCALER;
	ADC_start.ADC_MODE = FREE_RUN;
	adc_init(ADC_start);
	adcInterruptEnable;
	adcEnable;
	adcAutoTriggerEnable;
	adcStart;
	
	/*Initialize all coil pins to output low and set the common carrier sides for PWM*/
	init_all_coils();
	/*Set TIMER1 registers to output 500kHz PWM when turned on (default off)*/
	set_comm_channels();
	/*
	init_coil(167,0.5,5);    // initial the coil
	drive_coil1(STOP);
	drive_coil2(STOP);
	drive_coil3(STOP);
	drive_coil4(STOP);
	*/
	/*initialize the RF receiver shut down pin to be output and set it to be off*/
	set(Shut_down_RF_DDR,Shut_down_RF_PIN);
	RF_receiver_disable;
	
	sei();														//Enable global interrupts
}

//**************************************************************************************
//
//!  read_ADCs
//!
//!  @param    none
//!
//!  @return   adc_val			pointer to the array with the ADC values
//!
//!  @brief    This function will return a pointer to the array containing the ADC values
//!
//
//**************************************************************************************
uint16_t * read_ADCs(void)
{
	return adc_val;
}

//**************************************************************************************
//
//!  send_msg_all_coils
//!
//!  @param    nos_times_to_send	The number of times to try sending
//!
//!  @return   none
//!
//!  @brief    Sends the face specific, coil specific msg
//!			   over each coil one by one for communication
//!
//
//**************************************************************************************
void send_msg_all_coil(uint8_t nos_times)
{
	nos_times_usart_send = nos_times;
	set(UCSR0B,TXEN0);						//Enable TX
	usart_buf[0] = (0x01<<7)|(address_pins_value<<4)|0x00; //Face specific,coil specific msg
	usart_buf[1] = (0x01<<7)|(address_pins_value<<4)|0x01; //Face specific,coil specific msg
	usart_buf[2] = (0x01<<7)|(address_pins_value<<4)|0x02; //Face specific,coil specific msg
	usart_buf[3] = (0x01<<7)|(address_pins_value<<4)|0x03; //Face specific,coil specific msg				
	
	set(UCSR0B,UDRIE0);						//Enable USART ready to send data interrupt
}

//**************************************************************************************
//
//!  enable_coil_listener
//!
//!  @param    coil_num		The number of the coil to make it receiver (CO1-4)
//!
//!  @return   none
//!
//!  @brief    Sets the coil to be receiver and starts up receiver IC
//!
//!  @Note		Always call disable_coil_receiver or coil_settings(coil_num,STOP_ALL)
//!				Once reception process is complete
//
//**************************************************************************************
void enable_coil_receiver(COIL_NUM coil_num)
{
	coil_setting(coil_num,USART_LISTENER);		//TODO check implementation
	RF_receiver_enable;
}

//**************************************************************************************
//
//!  disable_coil_listener
//!
//!  @param    coil_num		The number of the coil to disable reception
//!
//!  @return   none
//!
//!  @brief    Sets the coil to be receiver and starts up receiver IC
//!
//
//**************************************************************************************
void disable_coil_receiver(COIL_NUM coil_num)
{
	coil_setting(coil_num,STOP_ALL);		//TODO check implementation
	RF_receiver_disable;					
}

//**************************************************************************************
//
//!  read_rcvd_msg
//!
//!  @param    none
//!
//!  @return   UDR0		The contents of the USART receive data register
//!  @return   0		If there is no data on the USART buffer
//!
//!  @brief    This will return the data if any received over USART else 0
//!
//
//**************************************************************************************
uint8_t read_rcvd_msg()
{
	if(newusart_data)
	{
		newusart_data = 0;
		return usart_rcv_buf;
	}
	else
	return 0;
}

//**************************************************************************************
//
//!  process_usart_rcvd_data
//!
//!  @param    data			The received data to parse
//!  @param    connstate	The pointer to the buffer where to store the connection props
//!
//!  @return   none
//!
//!  @brief    This will parse through message received over the coils and store the 
//!			   face number and the orientation(coil number) it is connected to
//!
//
//**************************************************************************************
void process_usart_rcvd_data(uint8_t data,connection_state *connstate)
{
	connstate->isconnected = (data>>7);				//isconnected
	connstate->face_num = (data>>4)&0b00000011;		//face number
	connstate->coil_num = (data)&0b00000011;		//coil number
}

//**************************************************************************************
//
//!  coil_driving_handler
//!
//!  @param    driving_state			MAGNETS_ON
//!	 @param								MAGNETS_OFF
//!	 @param	   coils_onoff_parameters	parameters to set the duty cycle freq and number of
//!										pulses
//!
//!  @return   driving state	if all coils driven then NONE otherwise returns previous state
//!
//!  @brief    handles driving the coils either on or off based on the driving state. 
//!
//
//**************************************************************************************
uint8_t coil_driving_handler(COIL_DRIVING_STATE driving_state, coils_onoff_parameters drive_params)
{
	static uint8_t curr_driven_coil = 0,start_call = 0;
	//if(!start_call)		//Error, why not working with the if statement?	
	//{						//calculation every function call is redundant
		int32_t freq = 1000/(drive_params.high_time+drive_params.low_time);
		set_comm_freq(freq);
		set_comm_duty_cycle(0.001*drive_params.high_time*freq);
		set_comm_nos_pulses(drive_params.nos_pulses);
		start_call++;
		m_yellow(TOGGLE);
	//}
	if (curr_driven_coil<=CO4)
	{
		if(!check(TCCR1B,CS12))
		{
			
			switch (driving_state)
			{
				case MAGNETS_ON:
					coil_setting(curr_driven_coil,TURN_ON);
								
					if(curr_driven_coil)
					coil_setting(curr_driven_coil-1,STOP_ALL);
					//error handling cause COMM_LOW_ON not working on first call
					if(start_call==1)
					{
						coil_setting(curr_driven_coil,STOP_ALL);
						curr_driven_coil--;
						start_call++;
					}
					//If fixed comment out above lines after the comment
					COMM_LOW_ON;
					curr_driven_coil++;
					break;
				case MAGNETS_OFF: 
					coil_setting(curr_driven_coil,TURN_OFF);
			
					if(curr_driven_coil)
					coil_setting(curr_driven_coil-1,STOP_ALL);
					//error handling cause COMM_HIGH_ON not working on first call
					if(start_call==1)
					{
						coil_setting(curr_driven_coil,STOP_ALL);
						curr_driven_coil--;
						start_call++;
					}
					//If fixed comment out above lines after the comment
					COMM_HIGH_ON;
					curr_driven_coil++;
					break;
				case NONE:
				break;
			}
		}
		return driving_state;
	}
	else
	{
		if(!check(TCCR1B,CS12))
		{
			coil_setting(curr_driven_coil-1,STOP_ALL);
			curr_driven_coil = 0;
			start_call = 0;
			return NONE;
		}
	}				//Ideally should never reach here
	return -1;
}

/*--------------------------------private Functions-----------------------*/
//**************************************************************************************
//
//!  ISR (interrupt service routine)
//!
//!  @param    ADC_vect
//!
//!  @return   none
//!
//!  @brief    Interrupt handler. Called when an ADC conversion is complete. Will store 
//!			   ADC value in the adc_val global variable
//
//**************************************************************************************
ISR(ADC_vect)
{
	static ADC_Channel ADC_channel = ADC0;
	if (ADC_channel==ADC0)
	adc_val[0] = ADC;
	else
	adc_val[1] = ADC;
	ADC_channel = !ADC_channel;
	adc_set_channel(ADC_channel);
}

//**************************************************************************************
//
//!  ISR (interrupt handler)
//!
//!  @param    USART_RX_vect
//!
//!  @return   none
//!
//!  @brief    Interrupt handler. Called when some data is received by the USART. Stores
//!			   the data into a global buffer.
//!	 @Note	   It seems there is a dummy byte received by the receiver after each actual
//!			   data receive end. Its value is >0xF8
//
//**************************************************************************************
ISR(USART_RX_vect)
{
	if(UDR0<=0xF8)			
	{						
		usart_rcv_buf = UDR0;
		newusart_data = 1;
	}
	else
	USART_Receive_flush();	
}

//**************************************************************************************
//
//!  ISR (interrupt handler)
//!
//!  @param    USART_TX_vect
//!
//!  @return   none
//!
//!  @brief    Interrupt handler. Called when TX is complete. Needed cause of NOR IC
//!
//
//**************************************************************************************
ISR(USART_TX_vect)
{
	clear(UCSR0B,TXEN0);			//Disable TX (Have to do cause of NOR IC)	
	Carrier_Wave(OFF);
}

//**************************************************************************************
//
//!  ISR (interrupt handler)
//!
//!  @param    USART_UDRE_vect
//!
//!  @return   none
//!
//!  @brief    Interrupt handler. Called when USART data register is ready to receive data
//!			   When use gives command to send data over usart this is enabled and 
//!			   send data from the usart_buf over individual coils
//!
//!	 @NOTE	   For now the implementation while non-blocking calls the interrupt 3 extra 
//!			   times when no msg is send over the coils. Consider improving this in future
//!			   versions
//
//**************************************************************************************
ISR(USART_UDRE_vect)
{
	static uint8_t count = 0, numsentall=0;
	if(count<4)
	UDR0 = usart_buf[count];
	else
	UDR0 = 0;
	
	if(!count)
	{	
	}	//Seems to be getting called twice in the starting
	else if(count==1)
	{
		coil_setting(count-1,USART_SENDER);
		Carrier_Wave(ON);	
	}
	else
	{
		coil_setting(count-2,STOP_ALL);
		coil_setting(count-1,USART_SENDER);
	}
	count++;
	if(count ==6)
	{
		count = 0;
		numsentall++;
		if(numsentall==nos_times_usart_send)
		{
			numsentall = 0;
			clear(UCSR0B,UDRIE0);
		}
	}
}
//***************************************************************************************
//
// Close the Doxygen group.
//! @}
//
//
//***************************************************************************************