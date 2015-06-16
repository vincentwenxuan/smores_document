'''
This module defines the PacketServer object for interpreting, storing, 
retrieving, and displaying received packet messages. At creation the optional 
parameter types='types_file.json' may be used to initialize the dictionary of 
metadata for interpreting future recieved messages. Alternatively, at creation 
the optional parameter data='data_file.json' may be used to load a previous 
session.
'''

from serial_packet import *
import json
from collections import namedtuple
from time import sleep
import sys


def _MakeHeaderString(names, types):
  col_widths = _ColWidths(names, types)
  return " ".join(f.rjust(w) for f,w in zip(names, col_widths))


def _MakeFormatString(names, types):
  col_width = _ColWidths(names, types)
  string = ""
  for i in xrange(len(types)):
    if types[i] == 'f':
      string = " ".join((string, "{{:>{}.4f}}".format(col_width[i])))
    else:
      string = " ".join((string, "{{:>{}d}}".format(col_width[i])))
  string = string[1:]
  return string


# Form a list of string labels names and a string of struct variable types, 
# return the required column width for clean display. Floats are assumed to be 
# displayed as 10.3f.
def _ColWidths(names, types):
  def TypeWidth(ch):
    return {'t':5, 'b':4, 'B':3, 'h':6, 'H':5, 'i':11, 'I':10, 'f':10}[ch]
  header_widths = map(len, names)
  value_widths = map(TypeWidth, types)
  col_widths = map(max, header_widths, value_widths)
  return tuple(col_widths)




class PacketServer:

  msg_byte = {}    # dict of lists of receive messages, keyed by type byte
  about_byte = {}  # dict of About tuples of public message type metadata
  _util_byte = {}   # dict of Util tuples of private message type metadata
  
  _default_header = "cpu_t"
  
  # public metadata type for defined message types
  _About = namedtuple("About",
                    ["type_byte",
                     "type_name",
                     "field_names",
                     "field_units",
                     "field_types"])

  # private metadata type for defined message types
  _Util = namedtuple("Util",
                    ["header_string",
                     "format_string",
                     "field_widths",
                     "factory"])


  def __init__(self, types_filename="types.json", data_filename=None):      # serial

    # if data file provided, load only from that
    if data_filename is not None:
      self.FromFile(data_filename)
    
    # otherwise initialize empty using type file
    else:

      # load message information file
      try:
        types_file = open(types_filename)
      except IOError:
        print
        print "WARNING:"
        print "Filename " + types_filename + " not found. Using empty dictionary."
        print
        msg_info = []
        sleep(3)
      else:
        msg_info = json.load(types_file)
        types_file.close()
        
      
      # prepare data structures
      self.about_byte =  {m['type_byte']:
                          self._About(
                            m["type_byte"],
                            m["type_name"],
                            ("cpu_t",) + tuple(m["field_names"]),
                            ("utctime",) + tuple(m["field_units"]),
                            "t" + m["field_types"])
                          for m in msg_info}
       
      self._util_byte = {m.type_byte:
                        self._Util(
                          _MakeHeaderString(m.field_names, m.field_types),
                          _MakeFormatString(m.field_names, m.field_types),
                          _ColWidths(m.field_names, m.field_types),
                          namedtuple(m.type_name, m.field_names)
                          )
                        for m in self.about_byte.values()}


  def AppendMessage(self, cpu_t, bytes):
    type_byte = None

    if len(bytes) > 0:
      type_byte = bytes[0]
      if self._util_byte.has_key(type_byte):
        format_string = '<' + self.about_byte[type_byte].field_types[1:]
        data = unpack(str(format_string), bytes[1:])
        msg = self._util_byte[type_byte].factory._make((cpu_t,) + data)
      else:
        data = unpack('<'+'B'*(len(bytes)-1), bytes[1:])
        msg = (cpu_t,) + data
      if not self.msg_byte.has_key(type_byte):
        self.msg_byte[type_byte] = []
      self.msg_byte[type_byte].append(msg)
      
    return type_byte


  def PrettyTitle(self, type_byte):
    if self.about_byte.has_key(type_byte):
      return self.about_byte[type_byte].type_name.upper() + ': ' + str(type_byte)
    else:
      return 'UNKNOWN' + ': ' + str(type_byte)


  def PrettyHeader(self, type_byte):
    if self.about_byte.has_key(type_byte):
      return self._util_byte[type_byte].header_string
    else:
      return self._default_header


  def PrettyValue(self, type_byte):
    if len(self.msg_byte[type_byte]) > 1:
      cpu_dt = int(round((self.msg_byte[type_byte][-1][0] - self.msg_byte[type_byte][-2][0]).total_seconds()*1000))
      if cpu_dt > 9999:
        cpu_dt = 9999
    else:
      cpu_dt = 0
    display_values = (cpu_dt,) + self.msg_byte[type_byte][-1][1:]
    if self._util_byte.has_key(type_byte):
      return self._util_byte[type_byte].format_string.format(*display_values)
    else:
      return " " + ("".join(str(v).rjust(3+1) for v in display_values))


  def DisplayByte(self, type_byte):
    print self.PrettyTitle(type_byte)
    print self.PrettyHeader(type_byte)
    print self.PrettyValue(type_byte)


  def DisplayAll(self):
    for tb in self.msg_byte.keys():
      self.DisplayByte(tb)
      print
      
  def ToDictRepr(self):
    record = {}
    for msg_type in self.msg_byte.keys():
      entry = {}
      entry['type_byte'] = self.about_byte[msg_type].type_byte;
      entry['type_name'] = self.about_byte[msg_type].type_name;
      entry['field_names'] = list(self.about_byte[msg_type].field_names);
      entry['field_units'] = list(self.about_byte[msg_type].field_units);
      entry['field_types'] = self.about_byte[msg_type].field_types;
      #entry['data'] = [[x[0].isoformat()]+list(x[1:]) for x in self.msg_byte[msg_type]]
      entry['data'] = [[x[0].isoformat()]+list(x[1:]) for x in self.msg_byte[msg_type]]
      record[str(msg_type)] = entry
    return record


  def FromDictRepr(self, record):
    self.about_byte = {}
    self.msg_byte = {}
    for type_byte_string in record.keys():
      entry = record[type_byte_string]
      type_byte = int(type_byte_string)
      self.about_byte[type_byte] = self._About( entry['type_byte'],
                                                entry['type_name'],
                                                entry['field_names'],
                                                entry['field_units'],
                                                entry['field_types'])
    self._util_byte = {m.type_byte:
                  self._Util(
                    _MakeHeaderString(m.field_names, m.field_types),
                    _MakeFormatString(m.field_names, m.field_types),
                    _ColWidths(m.field_names, m.field_types),
                    namedtuple(m.type_name, m.field_names)
                    )
                  for m in self.about_byte.values()}
                  
    for type_byte_string in record.keys():
      data = record[type_byte_string]['data']
      type_byte = int(type_byte_string)
      self.msg_byte[type_byte] = [
        self._util_byte[type_byte].factory._make(
          [datetime.strptime(m[0], '%Y-%m-%dT%H:%M:%S.%f')] + m[1:]) 
          for m in data
      ]

    return record
      
      
  
  
  def ToFile(self, filename):
    with open(filename, 'w') as f:
      json.dump(self.ToDictRepr(), f, indent=0, separators=(',',':'))
  
  def FromFile(self, filename):
    with open(filename, 'r') as f:
      self.FromDictRepr(json.load(f))


# for real time display, prefer use of server_viewer.py
if __name__=='__main__':
  import os
  from datetime import datetime
  import argparse
  import time
  
  parser = argparse.ArgumentParser()
  parser.add_argument("--port", default="/dev/m4_com")
  parser.add_argument("--portal", action='store_true')
  parser.add_argument("--msginfo", default="types.json")
  args = parser.parse_args()
  
  if args.portal: conn = "portal"
  else: conn = "portal"
  sp = SerialPacket(portstr=args.port, conn_type=conn)
  ps = PacketServer(args.msginfo)
  last_time = datetime.utcnow()
  
  while True:
  
    # catch messages
    while True:
      (cpu_t, bytes) = sp.GetPacketBytes()
      type_byte = ps.AppendMessage(cpu_t, bytes);
      if type_byte is None:
        break

    # display messages
    this_time = datetime.utcnow();
    
    if (this_time - last_time).microseconds > 50000:
      last_time = this_time
      os.system('cls' if os.name=='nt' else 'clear')
      ps.DisplayAll()
