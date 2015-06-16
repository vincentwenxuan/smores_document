/// This header defines type values kTypeXXX and struct structures MsgXXX for 
/// common message types. This common_message_types.h header is typically 
/// included by a project's unique project_message_types.h header, which may 
/// then append new message types.  Common library functions that need to know 
/// about message types will include project_message_types.h.
///
/// Metadata is typically handled by wrapping a message type+data inside 
/// another message with the appropriate metadate fields.  Examples of metadata 
/// include "send this message to X on channel Y and have them reply to me at Z."
///
/// To avoid conflicts, common_message_types.h types range from 1-127 and 
/// project_message_types.h types range from 128-255.
///
/// Typically a function component (eg. the imu) will have one base command type 
/// used to get/set/save/reply single data members in the standard way.  It may 
/// have additional convenience message types for efficient bulk reporting, ect.

#ifndef COMMON_MESSAGE_TYPES_H
#define COMMON_MESSAGE_TYPES_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum { GET_CMD = 0, SET_CMD = 1, SAVE_CMD = 2, RESPONSE_CMD = 3 } GetSetEnum;


////////////////////////////////////////////////////////////////////////////////
// Communications Management

const uint8_t kTypeWarn = 0;

/// Radio metadata message wraper for sending messages over the air.
/// * Don't declare a struct with this, just use it to pack into and extract from 
///   byte arrays, in which case the data field length won't matter.
const uint8_t kTypeRadio = 1;
typedef struct __attribute__ ((__packed__)) {
  uint8_t receiver_id;      /// node ID of recipient
  uint8_t sender_id;        /// node ID of sender
  uint8_t type;               /// sent message type
  uint8_t data;               /// first byte of message data
} MsgRadio;

/// Radio send command (typical USB packet to base radio).
/// "Send a message of this type on this_channel with this data."
/// The message might typically be of type MsgRadio
/// * Don't delare a struct with this, just use it to pack into and extract from 
///   byte arrays, in which case the data field length won't matter.
const uint8_t kTypeRelaySend = 2;
typedef struct __attribute__ ((__packed__)) {
  uint8_t receiver_channel; /// send on this radio channel
  uint8_t type;                 /// sent message type
  uint8_t data;                 /// first byte of message data
} MsgRelaySend;

/// Request A Message Response
const uint8_t kTypeRequestMessage = 3;
typedef struct __attribute__ ((__packed__)) {
  uint8_t msg_type;
} MsgRequestMessage;

/// Take No Action
const uint8_t kTypeKeepAlive = 4;

////////////////////////////////////////////////////////////////////////////////
// State Estimation

/// State estimate, rad/s rates and quaternion orientation
const uint8_t kTypeEstQuat = 25;
typedef struct __attribute__ ((__packed__)) {
  float t;
  float wx;
  float wy;
  float wz;
  float qw;
  float qx;
  float qy;
  float qz;
} MsgEstQuat;

/// State estimate, rad/s rates and roll, pitch, yaw (ZYX) orientation
const uint8_t kTypeEstEuler = 26;
typedef struct __attribute__ ((__packed__)) {
  float t;
  float wx;
  float wy;
  float wz;
  float roll;
  float pitch;
  float yaw;
} MsgEstEuler;

/// UKF Estimator Commands
const uint8_t kTypeUkf6dof = 27;
typedef struct __attribute__ ((__packed__)) {
  uint8_t cmd;
} MsgUkf6dof;
/*
27 | 0 | // reinitialize estimator
*/

////////////////////////////////////////////////////////////////////////////////
// Components

// used by "mpu_60XX_imu_msgs.h"
#define TYPE_IMU_VAL                        7
#define TYPE_IMU_COMMAND_VAL                24
#define TYPE_IMU_RAW_VAL                    20
#define TYPE_IMU_METRIC_VAL                 21
#define TYPE_IMU_OFFSET_VAL                 22
#define TYPE_IMU_SCALE_VAL                  23
#include "mpu_60XX_imu_msgs.h"

// used by "brushless_controller_msgs.h"
#define TYPE_BRUSHLESS_CONTROLLER_VAL       10
#define TYPE_BRUSHLESS_CONTROLLER_STATE_VAL 30

// used by "pulsing_controller_msgs.h"
#define TYPE_PULSING_CONTROLLER_VAL         28
#define TYPE_PULSING_CONTROLLER_STATE_VAL   29

// used by "battery_monitor_msgs.h"
#define TYPE_BATTERY_MONITOR_VAL            40
#define TYPE_BATTERY_REPORT_VAL             41

// used by "supply_monitor_msgs.h"
#define TYPE_SUPPLY_MONITOR_VAL            40 //Supply is replacement for Battery, so same number
#define TYPE_SUPPLY_REPORT_VAL             41 //Supply is replacement for Battery, so same number

// used by "zigbee_interface_msgs.h"
#define TYPE_ZIGBEE_INTERFACE_VAL           12

// used by "usb_radio_msgs.h"
#define TYPE_USB_RADIO_VAL                  13


/// PersistentMemory Commands
const uint8_t kTypePersistentMemory = 11;
typedef struct __attribute__ ((__packed__)) {
  uint8_t command; // 0 = format
} MsgPersistentMemory;
// type | sub | 
// 11   | 0   | // format memory

// Poll Device for Any Responses
// A contentless message which may be used to signal that communications are 
// still alive, to give the device an opportunity to respond, or to illicit 
// some generic device response response.
const uint8_t kTypePoll = 14;


#ifdef __cplusplus
}
#endif // __cplusplus

#endif // COMMON_MESSAGE_TYPES_H