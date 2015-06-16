%%
com = '/dev/tty.usbmodem1411';
s = Smores(com);

%%
%s.moveDofPosition('left', 0, 20000, 0, 0, 0, 0, 2);
data_length = 3000;
goal_position = zeros(1, data_length);
goal_velocity = zeros(1, data_length);
current_position = zeros(1, data_length);
current_velocity = zeros(1, data_length);
motor_cmd = zeros(1, data_length);
pause(0.5)
for i = 1:data_length
    [goal_position(i), goal_velocity(i), current_position(i), current_velocity(i), motor_cmd(i)] = s.UsbCom.get_current_state(21);
end

%%
subplot(3,1,1);plot(current_position); hold on; plot(goal_position, 'r');
subplot(3,1,2);plot(current_velocity); hold on; plot(goal_velocity, 'r');
subplot(3,1,3);plot(motor_cmd);