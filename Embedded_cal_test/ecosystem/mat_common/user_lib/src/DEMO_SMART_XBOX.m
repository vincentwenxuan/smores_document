% smart_copter_obj = smart_copter('verbose',true,'joyMap','xbox360_RC');
% gains.kpxy = 700;
% gains.kdxy = 100;
% gains.kpz = 300;
% gains.kdz = 150;
% smart_copter_obj.setGains(gains);
% smart_copter_obj.id = 3;
% stateLog = joyVehicle('comPort','COM5','verbose',true,'vehicleObject',smart_copter_obj,'joySensitivity',[1 1 1 .1 .1],'doLog',false);

mot = BrushlessController('com','COM5');

joyHandle = vrjoystick(1);
[joyAxes, joyButtons, joyPOVs] = read(joyHandle);
while(~joyButtons(8))
    [joyAxes, joyButtons, joyPOVs] = read(joyHandle);
    mot.pwm_cmd = -max(0,-joyAxes(2));
    pause(.025);
end
clear mot;

% motorSpeed = [];
% bodySpeed = [];
% time = [];
% for i = 1:length(stateLog)
% time(end+1) = stateLog{1,i}.time;
% smart_copter_obj.parseMessage(stateLog{1,i}.messagesTypeIn,stateLog{1,i}.messagesDataIn);
% motorSpeed(end+1) = smart_copter_obj.currentState.motorxd;
% bodySpeed(end+1) = smart_copter_obj.currentState.xd(3);
% end
% plot(time,motorSpeed,time,bodySpeed,time,bodySpeed+motorSpeed)