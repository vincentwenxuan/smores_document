/*
 * SMORES_atmega_code.c
 *
 * Created: 8/26/2014 3:33:16 PM
 *  Author: Chaitanya
 */ 

#include "SMORES_periphs.h"
#include "systick.h"
#include "SMORES_message_types.h"

/*TWI stuffs*/	
void process_twi(void);
uint8_t messageBuf[TWI_BUFFER_SIZE]; //May not be required later
uint8_t data[4] = {1,2,3,4};		//test data

/*messages stuff*/
const face_parameters default_params ={CO1,5,0.001,0,0}; //Default parameters
const coils_onoff_parameters default_Onoff_params ={100,20,2};
//Global for now. Better if made local to where required
face_state state;
connection_state conn_props;
face_parameters parameters;									
coils_onoff_parameters onoff_params;
float receiver_on_time = 0;
bool receiver_on=0;

COIL_DRIVING_STATE Drive_coils = NONE;		//Global variable to handle driving the coils

/*State Estimator*/
void estimate_state(uint16_t* raw_pos);

int main (void)
{
	m_clockdivide(0);
	parameters = default_params;							//initialize to default values
	onoff_params = default_Onoff_params;
	init_all_periphs();
	systick_init();
	uint16_t *adc_values;
	char usartdata;
	float systemtime;
	m_green(ON);
	while(1)
	{
		usartdata = read_rcvd_msg();
		if(usartdata)
		{
			if(usartdata == 0x12)
			{
				Drive_coils =MAGNETS_ON;
			}
			else if(usartdata ==0x23)
			{
				Drive_coils =MAGNETS_OFF;
			}
			else
			send_msg_all_coil(parameters.nos_times_usart_send);
		}
		adc_values = read_ADCs();
		//USART_Transmit_uint(adc_values[0]);
		//USART_Transmit_string("\r\n");
		//USART_Transmit_uint(adc_values[1]);
		//USART_Transmit_string("\r\n");
		
		systemtime = get_curr_time();
		//USART_Transmit_long((long)systemtime);
		//USART_Transmit_string("\r\n");
		estimate_state(adc_values);
		if (Drive_coils)					//If driving coils command was given 
		{									//and all coils have not been driven
			Drive_coils = coil_driving_handler(Drive_coils, onoff_params);
		}
		process_twi();
		if(receiver_on)																//if receiver on
		{
			if(get_curr_time()-receiver_on_time > parameters.receiver_on_timeout)   //and exceeded timeout value
			{
				disable_coil_receiver(parameters.receiver_coil);					//disable receiver
				receiver_on = 0;		
				usartdata = read_rcvd_msg();										//read any received msg
				process_usart_rcvd_data(usartdata,&conn_props);						//and parse it to get connected face, orientation
			}																		//and store it in conn_props struct
		}
		
	}
}

void process_twi()
{
	if ( ! TWI_Transceiver_Busy() )
	{
		if ( TWI_statusReg.RxDataInBuf )
		{
			m_green(TOGGLE);
			TWI_Get_Data_From_Transceiver(messageBuf, 1);
			switch(messageBuf[0])
			{
				case msg_face_state:
					//TWI_Start_Transceiver_With_Data((uint8_t*)&state,sizeof(state));
					TWI_Start_Transceiver_With_Data(data,4); //test case
				break;
				case msg_conn_state:
					TWI_Start_Transceiver_With_Data((uint8_t*)&conn_props,sizeof(conn_props));
				break;
				case msg_params:
					TWI_Get_Data_From_Transceiver((uint8_t *)&parameters,sizeof(parameters));
				break;
				case msg_coils_onoff_params:
					TWI_Get_Data_From_Transceiver((uint8_t *)&onoff_params,sizeof(onoff_params));
				break;
				case msg_coils_on:
					Drive_coils =MAGNETS_ON;
				break;
				case msg_coils_off:
					Drive_coils = MAGNETS_OFF;
					//TWI_Start_Transceiver_With_Data(data,4); //test case
				break;
				case msg_usartsender:
					send_msg_all_coil(parameters.nos_times_usart_send);
				break;
				case msg_usartlistener:
					enable_coil_receiver(parameters.receiver_coil);
					receiver_on_time = get_curr_time();
					receiver_on =1;
				break;
			}
		}
	}
}
//Dummy example. Make this better
void estimate_state(uint16_t* raw_pos)
{
	static bool firstcall = 1;
	static float time=0.000001, prev_time=0;
	float del_t;
	face_state prev_state;
	if(firstcall)
	{
		//TODO Something
	}
	prev_state = state;						//DANGER
	state.pos = (raw_pos[0]+raw_pos[1])/2;	//Wrong
	time = get_curr_time();
	del_t = time - prev_time;
	state.vel = (state.pos - prev_state.pos)/del_t;
	state.acc = (state.vel - prev_state.vel)/del_t;
	prev_time = time;
}