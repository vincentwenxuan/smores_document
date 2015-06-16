'''
This is a simple console script for reading out packet data.
When a valid packet is recieved, the the byte values are printed.
Corrupted packets are discarded and not printed.
'''

from datetime import datetime
from time import sleep
import sys
sys.path.append("../../../../py_common/user_lib/src")
from serial_packet import *

sp = SerialPacket("/dev/ttyACM0")

old_time = datetime.utcnow()
while True:
  bytes = sp.GetPacketBytes()
  if not len(bytes) == 0:
    new_time = datetime.utcnow()
    ms = (new_time - old_time).microseconds / 1000.0
    old_time = new_time
    print "%5.1f ms, msg =" % ms,
    print bytes
