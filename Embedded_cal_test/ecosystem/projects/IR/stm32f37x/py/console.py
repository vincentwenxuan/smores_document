"""
* Cedric Destin
"""
from datetime import datetime
from time import sleep
import sys
import time
sys.path.append("../../../../py_common/user_lib/src")
from serial_packet import *

from matplotlib.pyplot import *

sp = SerialPacket("/dev/ttyACM0")
Length = []
sample = []
t0 = 0
x = 0

while t0 <= 30:
  t0 = time.clock()
  (msg_type, msg_data) = sp.GetSerialMessage('f')
  if not len(msg_data) == 0:
    print "Length = ", msg_data[0]
    Length.append(msg_data[0]) # [n] = msg_data[0]
    t = time.clock() - t0 #this should be in seconds
    sample.append(x)
    x = x + 1
    
matplotlib.pyplot.plot(sample,Length)
matplotlib.pyplot.ylabel('Centimeter cm')
matplotlib.pyplot.xlabel('Samples n')
show()