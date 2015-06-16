quadrotor_PPR_obj = quadrotor_PPR('verbose',true,'joyMap','xbox360_default');
% gains.kpxy = 200;
% gains.kdxy = 100;
% gains.kpz = 300;
% gains.kdz = 150;
% quadrotor_PPR_obj.setGains(gains);
% [gains, stick_gains] = load_params('passive_active');
[gains, stick_gains] = load_params('passive');
quadrotor_PPR_obj.setGains(gains);
stateLog = joyVehicle('comPort','COM5','verbose',true,'vehicleObject',quadrotor_PPR_obj,'joySensitivity',stick_gains,'joyDeadzone',[.1 .1 .1 .5 .5],'doLog',false);
if ~isempty(stateLog)
    desThrust = [];
    desRoll = [];
    desPitch = [];
    desYaw = [];
    estRoll = [];
    estPitch = [];
    estYaw = [];
    estAngVelX = [];
    estAngVelY = [];
    estAngVelZ = [];
    estAccX = [];
    estAccY = [];
    estAccZ = [];
    time = [];
    isitempty = [];
    PWMs = [];
    for i = 1:length(stateLog)
        if(isempty(stateLog{1,i}.messagesTypeIn))
            isitempty(end+1) = 1;
        else
            isitempty(end+1) = 0;
        end
        quadrotor_PPR_obj.parseMessage(stateLog{1,i}.messagesTypeIn,stateLog{1,i}.messagesDataIn);
        time(end+1) = stateLog{1,i}.time;
        desThrust(end+1)= quadrotor_PPR_obj.stateVehicle.desThrust;
        desRoll(end+1)= quadrotor_PPR_obj.stateVehicle.desRoll;
        desPitch(end+1)= quadrotor_PPR_obj.stateVehicle.desPitch;
        desYaw(end+1)= quadrotor_PPR_obj.stateVehicle.desYaw;
        estRoll(end+1)= quadrotor_PPR_obj.stateVehicle.estRoll;
        estPitch(end+1)= quadrotor_PPR_obj.stateVehicle.estPitch;
        estYaw(end+1)= quadrotor_PPR_obj.stateVehicle.estYaw;
        estAngVelX(end+1) = quadrotor_PPR_obj.stateVehicle.estAngVelX;
        estAngVelY(end+1) = quadrotor_PPR_obj.stateVehicle.estAngVelY;
        estAngVelZ(end+1) = quadrotor_PPR_obj.stateVehicle.estAngVelZ;
        estAccX(end+1) = quadrotor_PPR_obj.stateVehicle.estAccX;
        estAccY(end+1) = quadrotor_PPR_obj.stateVehicle.estAccY;
        estAccZ(end+1) = quadrotor_PPR_obj.stateVehicle.estAccZ;
%         PWMs(end+1,:)= quadrotor_PPR_obj.stateVehicle.PWMs;
    end
    figure(1);
    plot(time,estAngVelX,time,estAngVelY,time,estAngVelZ);
    figure(2);
    plot(time,estAccX,time,estAccY,time,estAccZ);
    figure(3);
    plot(time,isitempty);
    mean(isitempty)
    figure(4);
    plot(time,desThrust);
    figure(5);
    plot(time,desRoll,'r',time,desPitch,'g',time,desYaw,'b',time,estRoll,'c',time,estPitch,'m',time,estYaw,'y');
end