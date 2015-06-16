'''
    This module picks up any udp message broadcast to the channel PORT and sends back a reply to the transmitter (assuming they are all muscle modules). For each transmitter, a MuscleInfo object would be created to store its relevant information. This module is capable of talking to multiple CC3000s. 
'''
 
import socket
import sys
from muscleInfo import MuscleInfo #A class object to store info of the muscle module
 
HOST = ''   # Symbolic name meaning all available interfaces
PORT = 8888 # Arbitrary non-privileged port
 
# Datagram (udp) socket
try :
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    print 'Socket created'
except socket.error, msg :
    print 'Failed to create socket. Error Code : ' + str(msg[0]) + ' Message ' + msg[1]
    sys.exit()
 
 
# Bind socket to local host and port
try:
    s.bind((HOST, PORT))
except socket.error , msg:
    print 'Bind failed. Error Code : ' + str(msg[0]) + ' Message ' + msg[1]
    sys.exit()
     
print 'Socket bind complete'

muscle_module=[] #creates a list named muscle_module
index=-1
#This function initializes a new MuscleInfo object in the list
def create_new_module(): 
    print "Creating Muscle Module Object"
    muscle_module.append(MuscleInfo(addr[0]))
    muscle_module[-1].print_info()

#now keep talking with the client
while 1:
    # receive data from client (data, addr)
    d = s.recvfrom(1024)
    data = d[0]
    addr = d[1]
    #change the port back to 2222 (which CC3000s listen to)
    new_addr = (addr[0], 2222)

    if not data: 
        break

    reply = '!!'
    s.sendto(reply , new_addr)
    print 'Message[' + addr[0] + ':' + str(addr[1]) + '] - ' + data.strip()

    #This part checks if a MuscleInfo object has already been created for a particular IP address, if not it would then call the create_new_module function.
    if len(muscle_module)==0:
	create_new_module()
    else:
	for module in muscle_module:
	    if (module.ip != addr[0]):
 		create_new_module()

s.close()
