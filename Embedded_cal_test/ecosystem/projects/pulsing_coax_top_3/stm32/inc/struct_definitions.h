#ifndef STRUCT_DEFINITIONS_H
#define STRUCT_DEFINITIONS_H

typedef struct __attribute__ ((__packed__)) {
  float speed;
  float volts;
  float amps;
  float watts;
  float joules;
} SlaveMasterTransferStruct;

#endif // STRUCT_DEFINITIONS_H