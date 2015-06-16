from ctypes import *
from numpy import array, zeros
from numpy.ctypeslib import ndpointer
import struct
from cfg import ecosystem_path

################################################################################
# Must have already complied to packet_finder.so; try from src:

# gcc -shared -fPIC ../../../c_common/user_lib/src/crc_helper.c -I ../../../c_common/user_lib/inc -o ../obj/_crc_helper.so

################################################################################
# Configure Access to C Library

f = ecosystem_path + '/py_common/user_lib/obj/_crc_helper.so'
ch = cdll.LoadLibrary(f);

MakeCrc = ch.MakeCrc
MakeCrc.argtypes = [ndpointer(c_uint8), c_uint8]
MakeCrc.retype = c_uint16


################################################################################
# Test C Library

if __name__=='__main__':

  # example packet data
  msg_type = 1;
  data = [2, 345, 6.78];
  format_string = "BHf";

  # form packet from Python
  data_length = struct.calcsize(">" + format_string)
  packet = zeros(data_length + 5, c_uint8)
  packet[0] = 0x55
  packet[1] = data_length
  packet[2] = msg_type
  struct.pack_into("<" + format_string, packet, 3, *data)
  struct.pack_into("<H", packet, data_length + 3, \
                         MakeCrc(packet[1:3+data_length], 2+data_length))

  print packet
  print "Probably ok."
