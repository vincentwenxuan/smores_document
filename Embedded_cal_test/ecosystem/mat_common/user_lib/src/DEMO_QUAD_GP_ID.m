clear
close all
clc
addpath(genpath('../../../mat_common'))
% !sudo ln -s /dev/ttyACMzz0 /dev/ttyS101
instrreset

quadrotor_PPR_obj = quadrotor_PPR_ID('ID',2,'verbose',false,'joyMap','GP_RC');
[gains,stick_gains] = load_params('planar_ekf');
quadrotor_PPR_obj.setGains(gains);
stateLog = joyVehicle_ID('comPort','COM49','verbose',false,'vehicleObject',quadrotor_PPR_obj,'joySensitivity',stick_gains,'doLog',true);