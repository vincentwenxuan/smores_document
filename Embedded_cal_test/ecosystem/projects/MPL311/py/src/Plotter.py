'''
  Cedric Destin
  Ok so this will extract the data sent through usb
  the data will be the altitude from the encoder
'''
# Get data from the packet_server.py

''' !!!!
  * Imports
  * packet_server
    * script made by Jimmy
  * os
    * Im not sure
  * datetime
  * arparse
    * 
  * time
    * for timers, sleep...
  * sys
    * Used for path directories
    TODO
'''
import os
from datetime import datetime
from collections import namedtuple
import argparse
import time
import sys
import json
'''
  Go to the common folder
'''
# Oh so this is what I needed to run from my project!!!!
sys.path.append("../../../../py_common/user_lib/src")
from packet_server import *
from matplotlib.pyplot import *

'''
  TODO
'''
parser = argparse.ArgumentParser()
parser.add_argument("--port", default = "/dev/m4_com")
parser.add_argument("--msginfo", default = "types.json")
args = parser.parse_args()


sp = SerialPacket(args.port)
ps = PacketServer(args.msginfo)
last_time = datetime.utcnow();
this_time = datetime.utcnow();

t = 0

Alt = []

t0 = time.clock()
# loop for 30sec
while t - t0 <= 30:
  #print this_time.totalsecond()
  (cpu_t, bytes) = sp.GetPacketBytes()
  type_byte = ps.AppendMessage(cpu_t, bytes);
  #if type_byte is not None: #Why won't you listen to me!!!! Big Fau
    #break
  #print 'hi'
  this_time = datetime.utcnow();
  
  if (this_time - last_time).microseconds > 50000:
    last_time = this_time
    os.system('cls' if os.name == 'nt' else 'clear')
  t = time.clock()
  ps.DisplayAll()
Alt = [ (m.Altitude) for m in ps.msg_byte[9]  ]
length = len(Alt)
x = []
n = 0
for n in range(0, length):
  x.append(n)
  n = n + 1
matplotlib.pyplot.plot(x, Alt)
show()