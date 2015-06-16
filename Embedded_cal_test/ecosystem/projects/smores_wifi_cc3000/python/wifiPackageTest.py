# Wifi Meg Pack Test
import socket
import struct
import time

def msgGenerator(msgtype, data=[], format=''):
    msg = chr(msgtype)
    if not isinstance(data, list):
	    data = [data]
    msg += struct.pack("<" + format, *data)
    return msg

def send(data, port=2424, addr='192.168.10.210'):
    """send(data[, port[, addr]]) - multicasts a UDP datagram."""
    # Create the socket
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    # Make the socket multicast-aware, and set TTL.
    s.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, 20) # Change TTL (=20) to suit
    s.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST,1)
    # Send the data
    s.sendto(data, (addr, port))

def receive(port = 4242, addr = '192.168.10.100'):
	s = socket.socket(socket.AF_INET, # Internet
                     socket.SOCK_DGRAM) # UDP
	s.bind((addr,port))
	data, addr = s.recvfrom(1024)
	return data

def ipfinder(gate = '192.168.10.1'):
	#gate way of the routor
	s = socket.socket(socket.AF_INET, # Internet
					 socket.SOCK_DGRAM) # UDP
	s.connect((gate,0))
	ip = s.getsockname()[0]
	return ip

def ipconverter(addr):
	# ip string to hex ip backward
	ipList = addr.split('.')
	if len(ipList) != 4:
		return (False,0)
	hexList= []
	for numstr in ipList:
		try:
			num = int(numstr)
		except:
			return (False,0)
		if num<0 or num>255:
			return (False,0)
		hexList.append(hex(num))	
	hexaddr = '0x';
	for i in [3,2,1,0]:
		if len(hexList[i]) == 4:
			hexaddr += hexList[i][2:4]
		else:
			hexaddr += '0' + hexList[i][2] 
	return (True,int(hexaddr,0))

def findMessageNum(data):
	unpackMap = {101: 'B',
	102: 'HHhihhhb',
	103: 'BHhi',
	104: 'hihib',
	105: 'BHfffffff',
	106: 'BHff',
	107: 'ffffffff',
	108: 'ff',
	109: 'BB',
	110: 'H',
	111: 'BBB',
	112: 'IH'}
	state_data = []
	#unpack the first byte to see message type
	msg_type = struct.unpack('B', data[0])
	if (msg_type[0] in unpackMap.keys()): #MsgCurrentStateType
	    (state_data) = struct.unpack('<B'+unpackMap[msg_type[0]],data)
	else:
	    state_data = "nothing here"
	print "Message Type is:", msg_type[0]
	return state_data


ip = ipfinder()
print "Local IP is: ",ip
hexip = ipconverter(ip)
if hexip[0]:
	print "Local hex IP is: ", hexip[1]
ip_msg = msgGenerator(112 ,[hexip[1], 4242],'IH')
msg = msgGenerator(104,[10],'B')
msg_send = msgGenerator(105, [4,2,20,15,6,6,40,2,100],'BHfffffff')

send(ip_msg)
while(1):
    send(msg)
    data = receive()
    print "State Data:", findMessageNum(data)
    time.sleep(1)
    send(msg_send)
    time.sleep(1)