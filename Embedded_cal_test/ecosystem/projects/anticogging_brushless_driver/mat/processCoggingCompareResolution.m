function data = processCoggingCompareResolution(data,motor)

if strcmp(motor,'rocket710')
    motorR = .22; %Rocket 710
    motorKt = .0134; % Rocket 710
    motorLs = 0.000029; % Rocket 710
%     motorWaveError = [0,.004, .008, .012,0.0159,0.0198,0.0237,0.0275,0.0313,0.0350,0.0386]; % Rocket 710 % std of voltage error from being 0, 1, 2, 3 encoder ticks off from anticogged waveform
    filterFreq = 490;
%     cogTorque = 0.0044*1000;
%     cogTorque = 0.0034*1000;
    cogTorque = 0.0023*1000;
    mutualTorqueCoeff = 17.8;  %17.8*pwm + 2.71 = torqueSTD linear fit after removing other sources
    polePairs = 7;
elseif strcmp(motor,'maxon')
    motorR = 1.03; %Maxon
    motorKt = .0335; %Maxon
    motorLs = 0.000438; %Maxon
%     motorWaveError = [0,.0026, .0051, .0076, 0.0102, 0.0127, 0.0152, 0.0177, 0.0201, 0.0226, 0.0250]; % Maxon % std of voltage error from being 0, 1, 2, 3 encoder ticks off from anticogged waveform
    filterFreq = 335;
%     cogTorque = 6.5809e-04*1000;
%     cogTorque = 4.5294e-04*1000;
    cogTorque = 4.7963e-04*1000;
    mutualTorqueCoeff = 2.16; %2.16*pwm + .762 = torqueSTD linear fit after removing other sources
    polePairs = 8;
elseif strcmp(motor,'eflite740')
    motorR = .26; %Eflite 740Kv
    motorKt = .0129; %Eflite 740Kv
    motorLs = 0.0000465; %Eflite 740Kv
%     motorWaveError = [0,.0029, .0058, .0087, 0.0115, 0.0143, 0.0171, 0.0198, 0.0224, 0.0250, 0.0276]; %Eflite 740Kv % std of voltage error from being 0, 1, 2, 3 encoder ticks off from anticogged waveform
    filterFreq = 490;
%     cogTorque = 0.0025*1000;
    cogTorque = 0.0019*1000; %still high!
    polePairs = 7;
    mutualTorqueCoeff = 17.8; %17.8*pwm + 1.47 = torqueSTD linear fit after removing other sources
elseif strcmp(motor,'eflite1380')
    motorR = .33; %Eflite 740Kv
    motorKt = .0069; %Eflite 740Kv
    motorLs = 0.0000688; %Eflite 740Kv
%     motorWaveError = [?]; %Eflite 740Kv % std of voltage error from being 0, 1, 2, 3 encoder ticks off from anticogged waveform
    filterFreq = 500;
    cogTorque = 3.9334e-04*1000;
    mutualTorqueCoeff = 0; %?*pwm + ? = torqueSTD linear fit after removing other sources
    polePairs = 7;
else
    error('No motor type!');
end
% 
% if isfield(data,'wave')
%     cogTorque = std(data.wave)/motorR*motorKt*1000;
% else
%     cogTorque = 0;
% end

for i = 1:length(data.compRes)
    data.compRes(i).motorType = motor;
    data.compRes(i).R = motorR;
    data.compRes(i).Kt = motorKt;
    data.compRes(i).Ls = motorLs;
%     data.compRes(i).waveErrors = motorWaveError;
    data.compRes(i).filterFrequency = filterFreq;    
    data.compRes(i).filtZTorque = notchFilter(filterFreq, 40000,data.compRes(i).daqData(:,6));
    data.compRes(i).pwmFrequency = 2*data.compRes(i).frequency;
    data.compRes(i).meanSpeed = mean(data.compRes(i).speed);
    data.compRes(i).stdTorque = std(data.compRes(i).filtZTorque);
    data.compRes(i).ppTorque = max(data.compRes(i).filtZTorque)-min(data.compRes(i).filtZTorque);
    data.compRes(i).resTorque = (data.compRes(i).motor.state.voltage/(72000000/data.compRes(i).frequency/2))/motorR*motorKt*1000/sqrt(3); % Nm to Nmm
    data.compRes(i).mutualTorque = mutualTorqueCoeff*data.compRes(i).pwm;
    
    %% PWM torque
    vIn = data.compRes(i).motor.state.voltage*data.compRes(i).pwm/2; % v*d is PP, div by 2 to get amp
    omega = 2*pi*data.compRes(i).pwmFrequency;   
    tau = motorLs/motorR;
    vOut = vIn/(omega^2*tau^2+1); 
    iOut = vOut/motorR;
    data.compRes(i).pwmTorque = iOut*motorKt*1000; % Nm to Nmm
    
    %% Encoder error torque
    %%TODO::This should be on full wave trapezoid as well!!!
    theta = linspace(0,2*pi*polePairs,4096);
    waveM = data.compRes(i).pwm*data.compRes(i).motor.state.voltage*sin(theta);
    if isfield(data.compRes(i),'wave')
        waveC = data.compRes(i).wave;
    else
        waveC = zeros(1,4096);
    end
    encTickPerSec = 4096*data.compRes(i).meanSpeed/2/pi; % (enc ticks / rev) * (rad/s) * rev/rad = enc ticks/s
    commutationHz = data.compRes(i).frequency;
    encTickErrors = tickMonteCarlo(encTickPerSec,commutationHz);
    data.compRes(i).waveErrors = calcWaveErrors(waveC,length(encTickErrors)-1);
    waveErrorsM = calcWaveErrors(waveM,length(encTickErrors)-1);
    vError = sqrt(sum((encTickErrors.*data.compRes(i).waveErrors).^2) + sum(encTickErrors.*waveErrorsM).^2);
%     vError = sum(tickMonteCarlo(encTickPerSec,commutationHz).*motorWaveError);
%     vError = motorWaveError(min(ceil(encTickPerSec/commutationHz),length(motorWaveError)));
    iError = vError/motorR;
    data.compRes(i).encTorque = data.compRes(i).anticogEnable*iError*motorKt*1000; % Nm to Nmm
%     
    %% Anticogging resolution torque
%     if data.compRes(i).anticogEnable
%         res = (72000000/data.compRes(i).frequency/2);
%         vStd = std(data.compRes(i).wave/data.compRes(i).motor.state.voltage - fix(data.compRes(i).wave/data.compRes(i).motor.state.voltage*res)/res)*data.compRes(i).motor.state.voltage;
%         data.compRes(i).antiTorque = vStd/motorR*motorKt*1000;
%     else
        data.compRes(i).antiTorque = 0;
%     end
    
    %% Cog torque
    if data.compRes(i).anticogEnable
        data.compRes(i).cogTorque = 0;
    else
        data.compRes(i).cogTorque = cogTorque;
    end
end
