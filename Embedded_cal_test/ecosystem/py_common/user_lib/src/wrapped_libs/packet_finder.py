from byte_queue import ByteQueueC, _InitBQ

from cfg import ecosystem_path

from ctypes import *
from numpy import array, zeros, concatenate
from numpy.ctypeslib import ndpointer
from struct import unpack, pack_into, calcsize

################################################################################
# Must have already complied packet_finder.so and _byte_queue.so
# If not available, try from src:

# gcc -g -shared -fPIC ../../../c_common/user_lib/src/packet_finder.c ../../../c_common/user_lib/src/crc_helper.c ../../../c_common/user_lib/src/byte_queue.c -I ../../../c_common/user_lib/inc -o ../obj/_packet_finder.so

# gcc -g -shared -fPIC ../../../c_common/user_lib/src/byte_queue.c -I ../../../c_common/user_lib/inc -o ../obj/_byte_queue.so


################################################################################
# Configure Low Level Access to C Library

f = ecosystem_path + '/py_common/user_lib/obj/_packet_finder.so'
clib = cdll.LoadLibrary(f);

class PacketFinderC(Structure):
  _fields_ = [("start_data",        POINTER(c_uint8)),
              ("end_data",          POINTER(c_uint8)),
              ("state",               c_uint32),
              ("parse_index",         c_uint16),
              ("packet_start_index",  c_uint16),
              ("received_length",     c_uint16),
              ("expected_crc",      c_uint16),
              ("received_crc",      c_uint16),
              ("data_bytes",        c_uint16),
              ("buffer",            c_uint8 * 255),
              ("out_buffer",        c_uint8 * 64),
              ("packet_indices",    POINTER(ByteQueueC)),
]

_InitPacketFinder = clib.InitPacketFinder
_InitPacketFinder.argtypes = [POINTER(PacketFinderC), POINTER(ByteQueueC)]
_InitPacketFinder.restype = c_int8

_PutBytes = clib.PutBytes
_PutBytes.argtypes = [POINTER(PacketFinderC), ndpointer(c_uint8), c_uint8]
_PutBytes.restype = c_int8

_PeekPacket = clib.PeekPacket
_PeekPacket.argtypes = [POINTER(PacketFinderC), POINTER(POINTER(c_uint8)), POINTER(c_uint8)]
_PeekPacket.restype = c_int8

_DropPacket = clib.DropPacket
_DropPacket.argtypes = [POINTER(PacketFinderC)]
_DropPacket.restype = c_int8

_GetPacketCopy = clib.GetPacketCopy
_GetPacketCopy.argtypes = [POINTER(PacketFinderC), ndpointer(c_uint8), POINTER(c_uint8)]
_GetPacketCopy.restype = c_int8

_FormPacket = clib.FormPacket
_FormPacket.argtypes = [c_uint8, ndpointer(c_uint8), c_uint8, ndpointer(c_uint8), POINTER(c_uint8)]
_FormPacket.restype = c_int8

kMaxPacketDataSize = c_uint8.in_dll(clib, "kMaxPacketDataSize").value
kMaxPacketSize = c_uint8.in_dll(clib, "kMaxPacketSize").value



################################################################################
# Define Convenience Class

class PacketFinder:
  def __init__(self):
    self.pfc = PacketFinderC()
    self.bqc = ByteQueueC()
    index_data_size = 20
    self.index_data = zeros(index_data_size, dtype=c_uint8)
    _InitBQ(self.bqc, self.index_data, index_data_size)
    _InitPacketFinder(self.pfc, self.bqc)
    
  def PutBytes(self, string):
    bytes = array(unpack('B'*len(string), string), dtype=c_uint8)
    return _PutBytes(self.pfc, bytes, c_uint8(bytes.size))      # return status
    
  def PeekPacket(self):
    bytes = POINTER(c_uint8)() # a null pointer
    length = c_uint8(0)
    status = _PeekPacket(self.pfc, bytes, byref(length))
    return array(bytes[:length.value], dtype=c_uint8)   # return copy of payload
    
  def DropPacket(self):
    return _DropPacket(self.pfc);                               # return status
    
  def GetPacketCopy(self):
    bytes = zeros(kMaxPacketSize, dtype=c_uint8)
    length = c_uint8(0)
    status = _GetPacketCopy(self.pfc, bytes, byref(length))
    return bytes[:length.value]                         # return copy of payload



################################################################################
# Convenience Function

# form packet from python data types
# see struct.pack_into docs for format_string types
def FormPacket(msg_type, data, format_string):
  data_length = c_uint8(calcsize("<" + format_string))
  data_bytes = zeros(data_length.value, c_uint8)
  pack_into("<" + format_string, data_bytes, 0, *data)

  packet_length = c_uint8(data_length.value + 5)
  packet_bytes = zeros(packet_length.value, c_uint8)
  _FormPacket(c_uint8(msg_type), data_bytes, data_length, packet_bytes, packet_length)

  return packet_bytes



################################################################################
# Test C Library

if __name__=='__main__':
  
  ###############################################
  # Demonstrate Convenience Class Use
  
  pf = PacketFinder()
  
  print
  print "PacketFinder Demo:"
  print

  print "Create Two Packets:"
  msg_type1 = 2;
  data1 = [2, 345, 6.78];
  format_string1 = "BHf";
  packet1 = FormPacket(msg_type1, data1, format_string1);
  msg_type2 = 2;
  data2 = [-6, -1000];
  format_string2 = "bh";
  packet2 = FormPacket(msg_type2, data2, format_string2);
  print "  ", packet1[:]
  print "  ", packet2[:]
         
  print "PutBytes to PacketFinder"
  data_with_added_garbage = concatenate([packet1[8:], packet1, packet2, packet1[:4]])
  status = pf.PutBytes(data_with_added_garbage)
  print "  Status:", status
  
  print "Find first packet with Peek/Drop"
  peek_data = pf.PeekPacket()
  print "  ", peek_data[:peek_data.size]
  drop_status = pf.DropPacket()

  print "Find second packet with Get"
  get_copy_data = pf.GetPacketCopy()
  print "  ", get_copy_data[:get_copy_data.size]
  print
  
  print "Stress test using packet_finder_test.py"
  print
  
