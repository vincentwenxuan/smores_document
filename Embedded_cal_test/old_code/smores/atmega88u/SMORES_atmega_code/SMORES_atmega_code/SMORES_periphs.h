/*
 * SMORES_periphs.h
 *
 * Created: 9/8/2014 2:39:32 PM
 *  Author: Chaitanya Bhargava (cbhar@seas.upenn.edu)
 */ 


#ifndef SMORES_PERIPHS_H_
#define SMORES_PERIPHS_H_

#include "TWI_slave.h"
#include "USART.h"
#include "ADC.h"
#include "drive_coil_v2.h"
#include "SMORES_platform_config.h"
#include "SMORES_message_types.h"

/*-------------------------defines--------------------------*/
#define RF_receiver_disable			clear(Shut_down_RF_PORT,Shut_down_RF_PIN)
#define RF_receiver_enable			set(Shut_down_RF_PORT,Shut_down_RF_PIN)

typedef enum coil_driving_state
{
	NONE = 0,
	MAGNETS_ON,
	MAGNETS_OFF
}COIL_DRIVING_STATE;

void init_all_periphs(void);
uint16_t * read_ADCs(void);
void send_msg_all_coil(uint8_t nos_times_to_send);
void enable_coil_receiver(COIL_NUM coil_num);
void disable_coil_receiver(COIL_NUM coil_num);
uint8_t read_rcvd_msg();
void process_usart_rcvd_data(uint8_t data,connection_state *connstate);
uint8_t coil_driving_handler(COIL_DRIVING_STATE drive_state, coils_onoff_parameters drive_params);

#endif /* SMORES_PERIPHS_H_ */