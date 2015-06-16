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
% UsbCom.toggle_white_led;
% UsbCom.toggle_yellow_led;
% data = UsbCom.get_usb_data(10);

%% for getting data without cmd
data = zeros(1, 1000);
for i = 1:1000
    data(i) = UsbCom.get_usb_data_continuous;
end

%%
UsbCom.delete;