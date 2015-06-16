def parse_numbers(data, format_string):
  return map(parse_number, data, format_string)


def parse_number(data, format_string):
  if format_string == 'f':
    return float(data)
  else:
    return int(data)

if __name__=='__main__':
  from serial_packet import SerialPacket
  import packet_finder
  import argparse
  from time import sleep
  
  parser = argparse.ArgumentParser()
  parser.add_argument('type_byte')
  parser.add_argument('data', nargs='*')
  parser.add_argument("--port", default="/dev/m4_com")
  parser.add_argument("--msginfo", default="types.json")
  parser.add_argument("--format_string", default=None)
  parser.add_argument("--portal", action='store_true')
  args = parser.parse_args()
  
  type_byte = int(args.type_byte)
  format_string = args.format_string
  data = parse_numbers(args.data, format_string)
  
  if args.portal: conn = "portal"
  else: conn = "serial"
  sp = SerialPacket(portstr=args.port, conn_type=conn)

  sp.SendPacket(type_byte, data, format_string)
  sleep(0.1) # make sure pipe doesn't close before value gets read
