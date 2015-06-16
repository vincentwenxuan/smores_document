from packet_finder import *
from struct import unpack, pack_into, calcsize
import serial
from datetime import datetime
from multiprocessing.connection import Client


################################################################################
# Setup serial port

class SerialPacket:

  # initialize for either serial device or client port use
  #   if conn_type="serial" use portstr for device port
  #   else use client connection
  def __init__(self, portstr="/dev/m4_com", conn_type="serial"):
    self._pf = PacketFinder()
    self.conn_type = conn_type
    if self.conn_type == "serial":
      self._ser = serial.Serial(portstr, 115200, timeout=0)
      self._ser.flushInput()
    elif self.conn_type == "portal":
      conn = Client(address=('localhost', 50000), authkey='abracadabra')
      port = conn.recv()
      self.conn = Client(address=('localhost', port), authkey='abracadabra')
      
  # construct and send a packet from data
  def SendPacket(self, msg_type, data, format_string):
    if self.conn_type == "serial":
      packet = FormPacket(msg_type, data, format_string)
      bytes_sent = self._ser.write(packet)
    elif self.conn_type == "portal":
      data_length = c_uint8(calcsize("<" + format_string))
      payload_bytes = zeros(data_length.value+1, c_uint8)
      payload_bytes[0] = msg_type
      pack_into("<" + format_string, payload_bytes, 1, *data)
      self.conn.send(payload_bytes)
      bytes_sent = len(payload_bytes)
    return bytes_sent
  

  # get bytes from last packet, packet IS removed from input buffer
  def GetPacketBytes(self):
    if self.conn_type == "serial":
      cpu_t = datetime.utcnow()
      chars_received = self._ser.read(64)
      #print "raw: ", [ord(c) for c in chars_received]
      self._pf.PutBytes(chars_received)
      msg = self._pf.GetPacketCopy()
      #print "msg: ", msg.tolist()
    elif self.conn_type == "portal":
      if self.conn.poll():
        (cpu_t, msg) = self.conn.recv()
      else:
        cpu_t = datetime.utcnow()
        msg = ""
    return (cpu_t, msg)


################################################################################
# Demo: Print Serial-Received Packets to Console

if __name__=='__main__':
  import argparse
  
  parser = argparse.ArgumentParser()
  parser.add_argument("--port", default="/dev/m4_com")
  parser.add_argument("--portal", action='store_true')
  args = parser.parse_args()
  
  if args.portal: conn = "portal"
  else: conn = "serial"
  sp = SerialPacket(portstr=args.port, conn_type=conn)

  while True:
    (cpu_t, bytes) = sp.GetPacketBytes()
    if not len(bytes) == 0:
      print bytes
