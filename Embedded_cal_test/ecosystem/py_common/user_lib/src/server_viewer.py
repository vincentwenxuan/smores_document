if __name__=='__main__':
  import argparse
  import os
  from datetime import datetime
  import curses
  from time import sleep
  from packet_server import PacketServer
  from serial_packet import SerialPacket
  
  parser = argparse.ArgumentParser()
  parser.add_argument("--port", default="/dev/m4_com")
  parser.add_argument("--portal", action='store_true')
  parser.add_argument("--msginfo", default="types.json")
  args = parser.parse_args()


  def DrawScreen(stdscr, ps, sp):
    # configure curses window
    curses.curs_set(0)
    
    last_time = datetime.utcnow()
    msg_line = {} # dict of line number to start each message
    next_msg_line = 0
    
    while True:
    
      # catch messages
      types = []
      while True:
        (cpu_t, bytes) = sp.GetPacketBytes()
        type_byte = ps.AppendMessage(cpu_t, bytes);
        if type_byte is not None:
          types.append(type_byte)
        else:
          break
      
      # update display
      for t in types:
        # add header line if message never seen before
        if not msg_line.has_key(t):
          msg_line[t] = next_msg_line
          next_msg_line = next_msg_line + 4
          stdscr.addstr(msg_line[t], 0, ps.PrettyTitle(t));
          stdscr.addstr(msg_line[t]+1, 0, ps.PrettyHeader(t));

        stdscr.addstr(msg_line[t]+2, 0, ps.PrettyValue(t));
      stdscr.refresh()


  if args.portal: conn = "portal"
  else: conn = "serial"
  sp = SerialPacket(portstr=args.port, conn_type=conn)
  ps = PacketServer(args.msginfo)
  curses.wrapper(DrawScreen, ps, sp)
