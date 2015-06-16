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
data_length = 5000;
time_sec = zeros(1, data_length);
time_nsec = zeros(1, data_length);
encoder_counter = zeros(1, data_length);
velocity = zeros(1, data_length);
position = zeros(1, data_length);
adc0 = zeros(1, data_length);
adc1 = zeros(1, data_length);
for i = 1:data_length
    [time_sec(i), time_nsec(i), encoder_counter(i), velocity(i), position(i), adc0(i), adc1(i)] = UsbCom.get_usb_data_continuous;
end


%% save data

data = [velocity; position; adc0; adc1; encoder_counter];

%% 
figure(1)
subplot(5,1,2);plot(position)
subplot(5,1,1);plot(velocity)
subplot(5,1,3);plot(adc0);
subplot(5,1,4);plot(adc1);
subplot(5,1,5);plot(encoder_counter)

figure(2)
subplot(2,1,1);plot(position)
subplot(2,1,2);plot(encoder_counter)

%%
UsbCom.delete;