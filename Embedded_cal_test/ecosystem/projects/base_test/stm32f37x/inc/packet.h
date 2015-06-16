#ifndef _PACKET_H
#define _PACKET_H

#include <stdint.h>

int process_serial_data(const uint8_t *data, const uint8_t count, uint8_t **out, uint8_t *output_len);

void create_packet(uint8_t data_len, uint8_t type, const void *data, void *out_data, uint8_t *tx_len);

#endif
