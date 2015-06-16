# UDP WIFI CLASS
import socket
import struct

class wifiModule():

    def __init__(self, sendAddr = '192.168.10.210', sendPort = 8888, receivePort = 4242, gate = '192.168.10.1'):
        '''
        Constructor
        '''
        self.s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.s.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, 20)
        self.s.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST,1)
        self.sendAddr = sendAddr
        self.sendPort = sendPort
        self.gate = gate
        self.receiveAddr = self.ipfinder()
        self.receivePort = receivePort
        self.r = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.r.bind((self.receiveAddr,self.receivePort))
        self.unpackMap = {101: 'B',
    	102: 'HHhihhhb',
    	103: 'BHhi',
    	104: 'hihib',
    	105: 'BHfffffff',
    	106: 'BHff',
    	107: 'fffffff',
    	108: 'ff',
    	109: 'BB',
    	110: 'H',
    	111: 'BBB',
    	112: 'IH'}

    def send(self, data):
    	"""send(data[, port[, addr]]) - multicasts a UDP datagram."""
    	self.s.sendto(data, (self.sendAddr, self.sendPort))

    def setSend(self, sendAddr, sendPort):
    	self.sendPort = sendPort
    	self.sendAddr = sendAddr

    def ipfinder(self):
    	self.s.connect((self.gate,0))
    	ip = self.s.getsockname()[0]
    	return ip

    def msgGenerator(self, msgtype, data=[], format=''):
        msg = chr(msgtype)
        if not isinstance(data, list):
    	    data = [data]
        msg += struct.pack("<" + format, *data)
        return msg

    def findMessageNum(self,data):
    	state_data = []
    	#unpack the first byte to see message type
    	msg_type = struct.unpack('B', data[0])
    	if (msg_type[0] in self.unpackMap.keys()): #MsgCurrentStateType
    	    state_data[:] = struct.unpack('<B'+ self.unpackMap[msg_type[0]],data)
    	else:
    	    state_data = "nothing here"
    	print "Message Type is:", msg_type[0]
    	return state_data

    def close(self):
    	self.s.close()
    	self.r.close()

    def ipconverter(self,addr):
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

    def receive(self):
    	data, addr = self.r.recvfrom(1024)
    	return data