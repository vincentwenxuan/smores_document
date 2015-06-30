%%
clear all
clc

%% add path for mat_common
addpath ../../../mat_common/user_lib/src/
addpath ../../../mat_common/user_lib/mex/

%% initial UsbCom object
UsbCom = usb_com('com', '/dev/tty.usbmodem1451');

%%

UsbCom.update_control_goal(10000, 0);

data_length = 1000;
current_position = zeros(1, data_length);
current_velocity = zeros(1, data_length);
motor_cmd = zeros(1, data_length);

for i = 1:data_length
    [current_position(i), current_velocity(i), motor_cmd(i)] = UsbCom.get_current_state();
end
    

%%

subplot(3,1,1);plot(current_position);
subplot(3,1,2);plot(current_velocity);
subplot(3,1,3);plot(motor_cmd);
    
    