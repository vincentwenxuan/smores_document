from ctypes import *
from numpy import array
from numpy.ctypeslib import ndpointer
from cfg import ecosystem_path

################################################################################
# Must have already complied to byte_queue.so; try from src:

# gcc -g -shared -fPIC ../../../c_common/user_lib/src/byte_queue.c -I ../../../c_common/user_lib/inc -o ../obj/_byte_queue.so

################################################################################
# Configure Low Level Access to C Library

f = ecosystem_path + '/py_common/user_lib/obj/_byte_queue.so'

clib = cdll.LoadLibrary(f);

class ByteQueueC(Structure):
  _fields_ = [("data",      POINTER(c_uint8)),
              ("start",     POINTER(c_uint8)),
              ("end",       POINTER(c_uint8)),
              ("data_size", c_uint16),
              ("count",     c_uint16)]
              
#class ByteQueueC32(Structure):
#  _fields_ = [("data",      POINTER(c_uint8)),
#              ("start",     POINTER(c_uint8)),
#              ("end",       POINTER(c_uint8)),
#              ("data_size", c_uint16),
#              ("count",     c_uint16),
#              ("buffer",    c_uint8 * 32)]

_InitBQ = clib.InitBQ
_InitBQ.argtypes = [POINTER(ByteQueueC), ndpointer(c_uint8), c_uint16]  # original

#_InitBQ.argtypes = [POINTER(ByteQueueC), POINTER(c_char*20), c_uint16]  # not any better
#_InitBQ.argtypes = [POINTER(ByteQueueC), POINTER(c_uint8), c_uint16]   # not any better

#_InitBQ32 = clib.InitBQ32
#_InitBQ32.argtypes = [POINTER(ByteQueueC32)]


################################################################################
# Define Convenience Class

#class ByteQueue:
#  def __init__(self, data_size=20):
#    self.bqc = ByteQueueC()
#    self.data = array(size, dtype=c_uint8)
#    _InitBQ(self.bqc, data, data_size)
