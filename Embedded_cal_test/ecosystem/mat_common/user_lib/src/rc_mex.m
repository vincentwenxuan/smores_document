clear all
close all
clc

dev  = '/dev/cu.usbserial-A6009Ezc'; %'/dev/ttyUSB0';
baud = 115200;

%%
SerialDeviceAPI('connect',dev,baud);

buf = SerialDeviceAPI('read',20,1000)
clear all