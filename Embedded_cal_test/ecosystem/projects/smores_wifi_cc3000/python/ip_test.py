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

def send(data, port=8888, addr='192.168.10.210'):
    """send(data[, port[, addr]]) - multicasts a UDP datagram."""
    # Create the socket
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    # Make the socket multicast-aware, and set TTL.
    s.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, 20) # Change TTL (=20) to suit
    # Send the data
    s.sendto(data, (addr, port))

def receive(port = 4242, addr = '192.168.10.100'):
    s = socket.socket(socket.AF_INET, # Internet
                     socket.SOCK_DGRAM) # UDP
    s.bind((addr,port))
    data, addr = s.recvfrom(1024)
    ## prototype for unpacking messages
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
        return (False,'0x00000000')
    hexList= []
    for numstr in ipList:
        try:
            num = int(numstr)
        except:
            return (False,'0x00000000')
        if num<0 or num>255:
            return (False,'0x00000000')
        hexList.append(hex(num))    
    hexaddr = '0x';
    for i in [3,2,1,0]:
        if len(hexList[i]) == 4:
            hexaddr += hexList[i][2:4]
        else:
            hexaddr += '0' + hexList[i][2] 
    return (True,int(hexaddr,0))


def findMessageNum(data):   # PROTOTYPE
    state_data = []
    #unpack the first byte to see message type
    msg_type = struct.unpack('B', data[0])
    if (msg_type[0] == 107): #MsgCurrentStateType
        (state_data) = struct.unpack('<Bfffffff',data)
    else:
        state_data = "nothing here"
    print "Message Type is:", msg_type[0]
    return state_data


ip = ipfinder()
print "Local IP is: ",ip
hexip = ipconverter(ip)
if hexip[0]:
    print "Local hex IP is: ", hexip[1]
# msg = msgGenerator(112 ,[hexip[1], 4242],'IH')
ip_msg = msgGenerator(112, [hexip[1], 4242], 'IH')
msg = msgGenerator(104,[10],'B')

msg_send = msgGenerator(105, [4,2,20,15,6,3,40,1.2,100],'BHfffffff')
# send(ip_msg)
while(1):
    send(msg_send)
    data = receive()
    print "State Data:", findMessageNum(data)
    time.sleep(1)


# print "messige sent:", msg
# send(msg)
# func_time = time.time()
# while (time.time() - func_time)< 100:
# #     # print "received message:", receive()
#     time.sleep(1)
#     send(msg)
#     print "receive message:", receive() 

# #     # print "messige sending:", msg
# #     # print "received message:", receive()



