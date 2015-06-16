clear
close all  
clc
addpath(genpath('../../../../mat_common'))
% !sudo ln -s /dev/ttyACM0 /dev/ttyS101
instrreset

quadrotor_PPR_obj = quadrotor_PPR_ID_v2('ID',2,'verbose',false,'joyMap','GP_RC');
[gains,stick_gains] = load_params_v2('planar_ekf');
quadrotor_PPR_obj.setGains(gains);
stateLog = joyVehicle_ID_v2('comPort','COM9','verbose',false,'vehicleObject',quadrotor_PPR_obj,'joySensitivity',stick_gains,'doLog',true);