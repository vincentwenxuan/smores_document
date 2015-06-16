#include "packet.h"
#include <string.h>

/* Packet format:
 * Packet: | 0x55 | 0x55 |count|type|---data---|crcL|crcH|
 * State:     0      1      2    3       4       5    6
 */

enum PacketState
{
  kPacketStart1,
  kPacketStart2,
  kPacketCount,
  kPacketType,
  kPacketData,
  kPacketCRCL,
  kPacketCRCH,
};

static const unsigned int kPacketOverhead = 6;
static const unsigned int kPacketDataMaxSize = 100;
static const uint8_t kPacketStartString[] = {0x55, 0x55};

static uint16_t crc16(const uint8_t *data, int count);
static uint16_t crc_update(uint16_t crc, uint8_t data);

int process_serial_data(const uint8_t *data, const uint8_t count, uint8_t **out, uint8_t *output_len)
{
  int ret = -1;
  *out = NULL;
  *output_len = 0;

  static uint8_t buffer[200];
  static enum PacketState state = kPacketStart1;
  static uint16_t received_length = 0, data_count = 0;;
  static uint16_t expected_crc = 0, received_crc = 0;

  for(size_t i = 0; i < count; i++)
  {
    const uint8_t c = data[i];
    //printf("%d, %X\n", state, c);
    if(state == kPacketStart1 && c == kPacketStartString[0])
      state = kPacketStart2;
    else if(state == kPacketStart2 && c == kPacketStartString[1])
      state = kPacketCount;
    else if(state == kPacketCount)
    {
      received_length = c;
      if(received_length > kPacketDataMaxSize)
        state = kPacketStart1;
      else
      {
        expected_crc = crc16(&c, 1);
        data_count = 0;
        state = kPacketType;
      }
    }
    else if(state == kPacketType)
    {
      buffer[data_count] = c; // Put type in buffer
      expected_crc = crc_update(expected_crc, c);
      data_count++;
      if(received_length > 0)
        state = kPacketData;
      else
        state = kPacketCRCL;
    }
    else if(state == kPacketData)
    {
      buffer[data_count] = c;
      expected_crc = crc_update(expected_crc, c);
      data_count++;
      if(data_count == received_length+1)
        state = kPacketCRCL;
    }
    else if(state == kPacketCRCL)
    {
      received_crc = c;
      state = kPacketCRCH;
    }
    else if(state == kPacketCRCH)
    {
      received_crc = received_crc + 256*c;
      if(expected_crc == received_crc)
      {
        // Received complete packet
        *out = buffer;
        *output_len = data_count;
        ret = 0;
      }
      state = kPacketStart1;
    }
    else
      state = kPacketStart1;
  }
  return ret;
}

void create_packet(uint8_t data_len, uint8_t type, const void *data, void *output_data, uint8_t *tx_len)
{
  const uint8_t *in_data = data;
  uint8_t *out_data = output_data;
  out_data[0] = 0x55;
  out_data[1] = 0x55;
  out_data[2] = data_len;
  uint16_t crc = crc16(&data_len, 1);
  out_data[3] = type;
  crc = crc_update(crc, type);
  for(int i = 0; i < data_len; i++)
  {
    out_data[4+i] = in_data[i];
    crc = crc_update(crc, in_data[i]);
  }
  out_data[4+data_len+0] = crc & 0xFF;
  out_data[4+data_len+1] = crc >> 8;
  *tx_len = 4+data_len+2;
}

static uint16_t crc_update(uint16_t crc, uint8_t data)
{
  uint16_t x = (crc >> 8) ^ data;
  x ^= x >> 4;

  crc = (crc << 8) ^ (x << 12) ^ (x <<5) ^ x;
  return crc;
}

static uint16_t crc16(const uint8_t *data, int count)
{
  uint16_t crc = 0xffff;

  for(uint16_t i = 0; i < count; i++)
  {
    crc = crc_update(crc, data[i]);
  }
  return crc;
}
