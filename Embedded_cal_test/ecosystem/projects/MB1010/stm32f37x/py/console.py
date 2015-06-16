"""
  * Cedric Destin
"""
from datetime import datetime
from time import sleep
import sys
import time
sys.path.append("../../../../py_common/user_lib/src")
from serial_packet import *

sp = SerialPacket("/dev/ttyACM0")

while True:
  (msg_type, msg_data) = sp.GetSerialMessage('f')
  if not len(msg_data) == 0:
    fo = open("Data.txt", "a")
    i = 3
    D = str(msg_data[0])
    fo.write(D)
    fo.write("\r\n")
    fo.close()
    print msg_data[0]
    '''time.sleep(1)'''