clear 
close all
clc
% !sudo ln -s /dev/ttyACM0 /dev/ttyS101
instrreset

quadrotor_PPR_obj = quadrotor_PPR('verbose',false,'joyMap','GP_RC');
[gains,stick_gains] = load_params('allpcb');
quadrotor_PPR_obj.setGains(gains);
stateLog = joyVehicle('comPort','COM40','verbose',false,'vehicleObject',quadrotor_PPR_obj,'joySensitivity',stick_gains,'doLog',false);