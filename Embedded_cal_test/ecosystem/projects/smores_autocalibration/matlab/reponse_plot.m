%%
clear all
clc

%% add path for mat_common
addpath ../../../mat_common/user_lib/src/
addpath ../../../mat_common/user_lib/mex/

%% initial UsbCom object
UsbCom = usb_com('com', '/dev/tty.usbmodem1411');

%%
%goal = 2*1820;
% goal = 0;
% UsbCom.update_control_goal(goal, 0);

%%

data_length = 5000;
goal_position = zeros(1, data_length);
goal_velocity = zeros(1, data_length);
current_position = zeros(1, data_length);
current_velocity = zeros(1, data_length);
motor_cmd = zeros(1, data_length);

for i = 1:data_length
    [goal_position(i), goal_velocity(i), current_position(i), current_velocity(i), motor_cmd(i)] = UsbCom.get_current_state();
end
    

%%

subplot(3,1,1);plot(current_position); hold on; plot(goal_position, 'r');
subplot(3,1,2);plot(current_velocity); hold on; plot(goal_velocity, 'r');
subplot(3,1,3);plot(motor_cmd);
    
    