// Zigbee Interface Message Definitions
#include "common_message_types.h"
#include <stdint.h>

#ifndef ZIGBEE_INTERFACE_MSGS_H
#define ZIGBEE_INTERFACE_MSGS_H

// This file must be included from project_message_types.h after appropriate 
// #defines are issued.  This file should not be included from ANY file other 
// than common_message_types.h.

#ifndef TYPE_ZIGBEE_INTERFACE_VAL
  #error "Define unique TYPE_ZIGBEE_INTERFACE_VAL for kTypeZigbeeInterface before inclusion."
#endif

////////////////////////////////////////////////////////////////////////////////
// Type
const uint8_t kTypeZigbeeInterface = TYPE_ZIGBEE_INTERFACE_VAL;
// Subtypes
const uint8_t kSubSelfId    = 0;
const uint8_t kSubRxChannel = 1;
const uint8_t kSubTxChannel = 2;
const uint8_t kSubDestId    = 3;

// type | sub | radio select         | direction      | value
// -----------------------------------------------------------------------------
// 12   | 0   | sub_radio_id (uint8) | access (uint8) | id             (uint8)
// 12   | 1   | sub_radio_id (uint8) | access (uint8) | rx_channel     (uint8)
// 12   | 2   | sub_radio_id (uint8) | access (uint8) | tx_channel     (uint8)
// 12   | 3   | sub_radio_id (uint8) | access (uint8) | destination_id (uint8)
// 'access' is 0-get, 1-set, 2-save, 3-reply
// default sub_radio_id is 0 for single-radio boards

#endif // ZIGBEE_INTERFACE_MSGS_H