%%
clear
clc

%% add path for mat_common
addpath ../../../mat_common/user_lib/src/
addpath ../../../mat_common/user_lib/mex/

%% initial UsbCom object
UsbCom = usb_com('com', '/dev/tty.usbmodem1411');

%%
[gp, gv, cp_left, cv, mc] = UsbCom.get_current_state(21);
coeff_speed_left = [20000, cp_left];
UsbCom.send_trajectory_velocity(21, coeff_speed_left);
[gp, gv, cp_right, cv, mc] = UsbCom.get_current_state(22);
coeff_speed_right = [-20000, cp_right];
UsbCom.send_trajectory_velocity(22, coeff_speed_right);

%%

UsbCom.trajectory_velocity_begin(21);
UsbCom.trajectory_velocity_begin(22);
pause(2)
% UsbCom.trajectory_velocity_stop(21);
UsbCom.trajectory_velocity_stop(22);
pause(1.7)
UsbCom.trajectory_velocity_begin(22)
pause(2);
UsbCom.trajectory_velocity_stop(21);
UsbCom.trajectory_velocity_stop(22);