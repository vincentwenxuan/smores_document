from datetime import datetime
from time import sleep
import sys
sys.path.append("../../../..py_common/user_lib/src")
from serial_packet import *

sp = SerialPacket("/dev/ttyACM0")
while True:
  (msg_type, msg_data) = sp.GetSerialMessage('H')
  if not len(msg_data) == 0
  printf msg_data[0]