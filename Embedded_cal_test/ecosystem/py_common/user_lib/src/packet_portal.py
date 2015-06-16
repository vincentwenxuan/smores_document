from argparse import ArgumentParser
from multiprocessing.connection import Listener, Client
from multiprocessing import Process, Queue
from serial_packet import SerialPacket

def request_server(q):
  print
  print "Starting PacketPortal on hostname 'localhost', port 50000."
  print

  listener = Listener(address=('localhost', 50000), authkey='abracadabra')
  free_port = 50001
  while True:
    conn = listener.accept()
    print "Accepted request from " + str(listener.last_accepted)
    conn.send(free_port)
    print "  Making available port " + str(free_port)
    q.put(free_port)
    free_port = free_port + 1




if __name__=='__main__':

  parser = ArgumentParser()
  parser.add_argument('server_or_client')
  parser.add_argument("--port", default="/dev/m4_com")
  args = parser.parse_args()



  # start a server process
  if args.server_or_client == 'server':

    sp = SerialPacket(args.port)

    open_ports = []
    port_requests = Queue()
    
    p = Process(target=request_server, args=((port_requests,)))
    p.start()
    
    count = 0
    while True:
      # new connection requests
      while port_requests.qsize() > 0:
        receive_port = port_requests.get()
        listener = Listener(address=('localhost', receive_port), authkey='abracadabra')
        conn = listener.accept()
        open_ports.append((conn, receive_port))
        print "  New receive connection accepted from port " + str(receive_port)
        print "  Open connections: " + str(len(open_ports))
      
      # dispatch serial to existing receive connections
      (cpu_t, bytes) = sp.GetPacketBytes()
      if len(bytes) > 0:
        temp = []
        for c in open_ports:
          try:
            c[0].send((cpu_t, bytes))
          except IOError:
            print "Broken Pipe, Removed Connection to " + str(c[1])
          else:
            temp.append(c)
        open_ports = temp
        
      # aggregate serial output
      temp = []
      for c in open_ports:
        try:
          if c[0].poll():
            bytes = c[0].recv()
            sp.SendPacket(bytes[0], bytes[1:], (len(bytes)-1)*"B")
        except IOError:
          print "Broken Pipe, IOError, Removed Connection to " + str(c[1])
        except EOFError:
          print "Broken Pipe, EOFError, Removed Connection to " + str(c[1])
        else:
          temp.append(c)
      open_ports = temp




  # start a client process
  elif args.server_or_client == 'client':
    # request port to use from server
    print "Connecting to PacketPortal on hostname 'localhost', port 50000."
    conn = Client(address=('localhost', 50000), authkey='abracadabra')
    receive_port = conn.recv()
    serial_port = Client(address=('localhost', receive_port), authkey='abracadabra')
    
    # get serial data and print to terminal
    while True:
      (cpu_t, bytes) = serial_port.recv()
      print bytes
        
