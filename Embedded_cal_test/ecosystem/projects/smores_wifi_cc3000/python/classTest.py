import time
import wlc
import numpy as np


wifi = wlc.wifiModule();
print "Local IP is: ", wifi.receiveAddr;
hexip = wifi.ipconverter(wifi.receiveAddr);
if hexip[0]:
	print "Local hex IP is: ", hexip[1]

ip_msg = wifi.msgGenerator(112 ,[hexip[1], 4242],'IH')
msg = wifi.msgGenerator(101,[10],'H')
pos = [1,2,3,4,5,6,7,8,100]
pos_msg = wifi.msgGenerator(105,pos,'BHfffffff')
pos_tx_msg = wifi.msgGenerator(107,[9],'B')
vel_tx_msg = wifi.msgGenerator(108,[9],'B')
vel_msg = wifi.msgGenerator(106,[3,4,5.4332,1.226],'BHff')
torq_msg = wifi.msgGenerator(111,[2,1,5],'BBB')
ctrl_msg = wifi.msgGenerator(103,[21,20,11,25],'BHhi')
ctrl_msg_up = wifi.msgGenerator(103,[21,10,11,25],'BHhi')
i = 0
wifi.send(ip_msg)

while(1):
    wifi.send(ctrl_msg_up)
    wifi.send(ctrl_msg)
    data = wifi.receive() 
    formated_data = wifi.findMessageNum(data)
    print "Current State:", (formated_data[:]) 
    time.sleep(.001)
    ctrl_msg_up = wifi.msgGenerator(103,[21,10,i,(i-200)],'BHhi')
    i = i+2 
