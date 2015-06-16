%%

%% add path for mat_common
addpath ../../../mat_common/user_lib/src/
addpath ../../../mat_common/user_lib/mex/

%% initial UsbCom object
UsbCom = usb_com('com', '/dev/tty.usbmodem1451');
% UsbCom = usb_com('com', '/dev/ttyACM0');
% com = MessageInterface('/dev/tty.usbmodem1411');
% UsbCom = usb_com('com', com);

%% for talking between matlab and micro

% MsgCmd
UsbCom.toggle_white_led;
UsbCom.toggle_yellow_led;
data = UsbCom.get_usb_data(10);

% MsgMotor
Motor.motor = 4;
Motor.motor_direction = 1;
Motor.motor_speed = 50;
%UsbCom.send_usb_motor(Motor);
DoF.DoF = 11;
DoF.direction = 1;
DoF.speed = 50;
% forward
UsbCom.send_usb_dof(DoF);
pause(1);
% stop
DoF.speed = 0;
UsbCom.send_usb_dof(DoF);
pause(0.5);
% backwards
DoF.direction = 0;
DoF.speed = 50;
UsbCom.send_usb_dof(DoF);
pause(1);
% stop
DoF.speed = 0;
UsbCom.send_usb_dof(DoF);



%% for getting data without cmd
% data = zeros(1, 1000);
% for i = 1:1000
%     data(i) = UsbCom.get_usb_data_continuous;
% end

%%
UsbCom.delete;