quadrotor_PPR_obj = quadrotor_PPR('verbose',true,'joyMap','xbox360_RC','viconKp',[0.6; 0.6; 1],'viconKd',[0.8; 0.8; 0.6],'maxXYVel',2,'maxZVel',1);
gains = load_params('allpcb');
quadrotor_PPR_obj.setGains(gains);
stateLog = joyVehicle('comPort','COM10','viconVehicle','Quadrotorallpcb','verbose',true,'vehicleObject',quadrotor_PPR_obj,'joySensitivity',[1 1 1 1 1],'doLog',false,'joyDeadzone',.15);