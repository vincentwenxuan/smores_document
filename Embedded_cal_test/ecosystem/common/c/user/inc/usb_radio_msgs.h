// Usb Radio Message Definitions
#include "common_message_types.h"
#include <stdint.h>

#ifndef USB_RADIO_MSGS_H
#define USB_RADIO_MSGS_H

// This file must be included from project_message_types.h after appropriate 
// #defines are issued.  This file should not be included from ANY file other 
// than common_message_types.h.

#ifndef TYPE_USB_RADIO_VAL
  #error "Define unique TYPE_USB_RADIO_VAL for kTypeUsbRadio before inclusion."
#endif

////////////////////////////////////////////////////////////////////////////////
// Type
const uint8_t kTypeUsbRadio = TYPE_USB_RADIO_VAL;
// Subtypes
// Identical to kTypeZigbeeInterface subtypes.

#endif // USB_RADIO_MSGS_H