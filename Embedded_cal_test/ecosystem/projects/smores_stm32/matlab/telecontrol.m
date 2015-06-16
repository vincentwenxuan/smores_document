%%
clear
clc

%% add path for mat_common
addpath ../../../mat_common/user_lib/src/
addpath ../../../mat_common/user_lib/mex/

%% initial UsbCom object
UsbCom = usb_com('com', '/dev/tty.usbmodem1411');

%%
coeff = [493.8272, -3703.7037, 7407.4074, 0, 0, 0];
% coeff = [-493.8272, 3703.7037, -7407.4074, 0, 0, 20000];
duration = 3;

%%
coeff = [1837.3125, -9186.5625, 12248.75, 0, 0, 100];
duration = 2;


%%
data_length = 3000;
goal_position = zeros(1, data_length);
goal_velocity = zeros(1, data_length);
current_position = zeros(1, data_length);
current_velocity = zeros(1, data_length);
motor_cmd = zeros(1, data_length);
% UsbCom.send_trajectory_position(22, coeff, duration);
for i = 1:data_length
    [goal_position(i), goal_velocity(i), current_position(i), current_velocity(i), motor_cmd(i)] = UsbCom.get_current_state(23);
end

%%
% subplot(3,1,1);plot(current_position); hold on; plot(goal_position, 'r');
% subplot(3,1,2);plot(current_velocity); hold on; plot(goal_velocity, 'r');
% subplot(3,1,3);plot(motor_cmd);

subplot(3,1,1);plot(current_position); hold on; plot(goal_position, 'r');
subplot(3,1,2);plot(current_velocity); hold on; plot(goal_velocity, 'r');
subplot(3,1,3);plot(motor_cmd);