%%
clear all
clc

%% add path for mat_common
addpath ../../../mat_common/user_lib/src/
addpath ../../../mat_common/user_lib/mex/

%% initial UsbCom object
UsbCom = usb_com('com', '/dev/tty.usbmodem1411');
% UsbCom = usb_com('com', '/dev/ttyACM0');
% com = MessageInterface('/dev/tty.usbmodem1411');
% UsbCom = usb_com('com', com);

%% for talking between matlab and micro
% UsbCom.toggle_white_led;
% UsbCom.toggle_yellow_led;
% data = UsbCom.get_usb_data(10);

%% for getting data without cmd
data_length = 2000;
time_sec = zeros(1, data_length);
time_nsec = zeros(1, data_length);
velocity = zeros(1, data_length);
position = zeros(1, data_length);
for i = 1:data_length
    [time_sec(i), time_nsec(i), velocity(i), position(i)] = UsbCom.get_usb_data_continuous;
end

%% 
subplot(2,1,1);plot(position)
subplot(2,1,2);plot(velocity)

%%
% velocity = 0;
% position = 0;
% subplot(2,1,1);h1 = plot(position, 'ro');
% subplot(2,1,2);h2 = plot(velocity, 'ro');
% index = 0;
% while true
%     index = index + 1;
%     [time_sec, time_nsec, velocity, position] = UsbCom.get_usb_data_continuous;  
%     hold on
%     subplot(2,1,1);
%     x = index - 100;    
%     axis([x x+200 -32767 32767]);
%     set(h1, 'XData', index, 'YData', position);
%     subplot(2,1,2);
%     axis([x x+200 -32767 32767]);    
%     set(h2, 'XData', index, 'YData', velocity);
%     drawnow;
%     pause(0.000000000001);
% end




%%
UsbCom.delete;