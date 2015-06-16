function plotCoggingCompareResolution(data)
motor = 'maxon'; % 'rocket', 'eflite', 'maxon'

filter = true;

plotTorqeVTime = false;
plotSpeedVTime = false;
plotSpeedVFreq = false;
plotSTDTorqueVFreq = false;
plotPPTorqueVFreq = false;
plotSpeedVFreqLines = false;
plotSTDTorqueVFreqLines = false;
plotSTDTorqueVSpeed = false;
plotFFTTorqueVTime = false;
plotFFTTorqueVRot = true;

%% setup stuff
if strcmp(motor,'rocket')
    motorR = .22; %Rocket 710
    motorKt = .0134; % Rocket 710
    motorLs = 0.000029; % Rocket 710
    motorWaveError = [0,.004, .008, .012,0.0159,0.0198,0.0237,0.0275,0.0313,0.0350,0.0386]; % Rocket 710 % std of voltage error from being 0, 1, 2, 3 encoder ticks off from anticogged waveform
    filterFreq = 490;
elseif strcmp(motor,'maxon')
    motorR = 1.03; %Maxon
    motorKt = .0335; %Maxon
    motorLs = 0.000438; %Maxon
    motorWaveError = [0,.0026, .0051, .0076, 0.0102, 0.0127, 0.0152, 0.0177, 0.0201, 0.0226, 0.0250]; % Maxon % std of voltage error from being 0, 1, 2, 3 encoder ticks off from anticogged waveform
    filterFreq = 335;
elseif strcmp(motor,'eflite')
    motorR = .26; %Eflite 740Kv
    motorKt = .0129; %Eflite 740Kv
    motorLs = 0.0000465; %Eflite 740Kv
    motorWaveError = [0,.0029, .0058, .0087, 0.0115, 0.0143, 0.0171, 0.0198, 0.0224, 0.0250, 0.0276]; %Eflite 740Kv % std of voltage error from being 0, 1, 2, 3 encoder ticks off from anticogged waveform
    filterFreq = 490;
end

if filter
    for i = 1:length(data.compRes)
        data.compRes(i).daqData(:,6) = notchFilter(filterFreq, 40000,data.compRes(i).daqData(:,6));
    end
end

%% Plot torque vs time
if(plotTorqeVTime)
    h_torque = figure;
    clf;
    hold all;

    for i = 1:length(data.compRes)
        %plot timeseries
        if data.compRes(i).anticogEnable
            option1 = '-';
            antiString = 'anticogged';
        else
            option1 = '--';
            antiString = 'nominal';
        end
        plot(data.compRes(i).daqTime,data.compRes(i).daqData(:,6), option1);
        %make name for legend
        legend_string{i} = [num2str(data.compRes(i).pwm) ' pwm, ' num2str(data.compRes(i).frequency) ' hz, ' antiString];
    end

    legend(legend_string{:})
    xlabel('Time (s)');
    ylabel('Cog Torque (Nmm)');
    grid on;
end

%% Plot speed vs time
if(plotSpeedVTime)
    h_speed = figure;
    clf;
    hold all;

    for i = 1:length(data.compRes)
        %plot timeseries
        if data.compRes(i).anticogEnable
            option1 = '-';
            antiString = 'anticogged';
        else
            option1 = '--';
            antiString = 'nominal';
        end
        plot(data.compRes(i).time,data.compRes(i).speed, option1);
        %make name for legend
        legend_string{i} = [num2str(data.compRes(i).pwm) ' pwm, ' num2str(data.compRes(i).frequency) ' hz, ' antiString];
    end

    legend(legend_string{:})
    xlabel('Time (s)');
    ylabel('Speed (rad/s)');
    grid on;
end

%% plot mean speed vs frequency
if(plotSpeedVFreq)
    h_speed_mean = figure;
    clf;
    hold all;

    maxHz = 0;
    for i = 1:length(data.compRes)
        %plot timeseries
        if data.compRes(i).anticogEnable
            option2 = 'o';
            antiString = 'anticogged';
        else
            option2 = 'x';
            antiString = 'nominal';
        end
        if data.compRes(i).frequency > maxHz
            maxHz = data.compRes(i).frequency;
        end
        plot(data.compRes(i).frequency, mean(data.compRes(i).speed),option2);

        %make name for legend
        legend_string{i} = [num2str(data.compRes(i).pwm) ' pwm, ' num2str(data.compRes(i).frequency) ' hz, ' antiString];
    end

    legend(legend_string{:})
    xlim([0 maxHz]);
    xlabel('Commutation Frequency (hz)');
    ylabel('Average Speed (rad/s)');
    grid on;
end

%% plot std torque ripple vs frequency
if(plotSTDTorqueVFreq)
    h_freq = figure;
    clf;
    hold all;

    maxHz = 0;
    for i = 1:length(data.compRes)
        %plot timeseries
        if data.compRes(i).anticogEnable
            option2 = 'o';
            antiString = 'anticogged';
        else
            option2 = 'x';
            antiString = 'nominal';
        end
        if data.compRes(i).frequency > maxHz
            maxHz = data.compRes(i).frequency;
        end
        % compute torque resolution
        vRes = data.compRes(i).motor.state.voltage/(72000000/data.compRes(i).frequency/2);
        iRes = vRes/motorR;
        tRes(i) = iRes*motorKt;

        plot(data.compRes(i).frequency, std(data.compRes(i).daqData(:,6)),option2);
        %make name for legend
        legend_string{i} = [num2str(data.compRes(i).pwm) ' pwm, ' num2str(data.compRes(i).frequency) ' hz, ' antiString];
    end
    plot([data.compRes(:).frequency],tRes*1000,'.k');

    legend(legend_string{:})
    xlim([0 maxHz]);
    % ylim([0 10]);
    xlabel('Commutation Frequency (hz)');
    ylabel('Standard Deviation (Nmm)');
    grid on;
end

%% plot filtered PP torque vs frequency
if(plotPPTorqueVFreq)
    h_pp = figure;
    clf;
    hold all;

    maxHz = 0;
    for i = 1:length(data.compRes)
        plot timeseries
        if data.compRes(i).anticogEnable
            option1 = '-';
            option2 = 'o';
            antiString = 'anticogged';
        else
            option1 = '--';
            option2 = 'x';
            antiString = 'nominal';
        end
        if data.compRes(i).frequency > maxHz
            maxHz = data.compRes(i).frequency;
        end
        compute torque resolution
        vRes = data.compRes(i).motor.state.voltage/(72000000/data.compRes(i).frequency/2);
        iRes = vRes/motorR;
        tRes(i) = iRes*motorKt;

        plot(data.compRes(i).frequency, max(data.compRes(i).daqData(:,6)) - min(data.compRes(i).daqData(:,6)),option2);
        make name for legend
        legend_string{i} = [num2str(data.compRes(i).pwm) ' pwm, ' num2str(data.compRes(i).frequency) ' hz, ' antiString];
    end
    plot([data.compRes(:).frequency],tRes*1000,'.k');

    legend(legend_string{:})
    xlim([0 maxHz]);
    ylim([0 10]);
    xlabel('Commutation Frequency (hz)');
    ylabel('Torque PP (Nmm)');
    grid on;
end

%% plot mean speed vs frequency
if(plotSpeedVFreqLines)
    h_speed_mean_cont = figure;
    clf;
    hold all;

    meanSpeedFreq0 = [];
    meanSpeedSpeed0 = [];
    meanSpeedFreq1 = [];
    meanSpeedSpeed1 = [];
    meanSpeedFreq2 = [];
    meanSpeedSpeed2 = [];
    legend_string = {};
    maxHz = 0;
    for i = 1:length(data.compRes)
        %plot timeseries
        if data.compRes(i).anticogEnable
            antiNum = 1;
            if i > 1 && data.compRes(i).pwm == data.compRes(i-1).pwm
                highSpeed = 1;
                legend_string{3} = ['Anticogged at Nominal PWM'];
            else
                highSpeed = 0;
                legend_string{2} = ['Anticogged'];
            end
        else
            option2 = 'x';
            antiString = 'nominal';
            antiNum = 0;
            highSpeed = 0;
            legend_string{1} = ['Nominal'];
        end
        if data.compRes(i).frequency > maxHz
            maxHz = data.compRes(i).frequency;
        end

        if highSpeed+antiNum == 0
            meanSpeedFreq0(end+1) = data.compRes(i).frequency;
            meanSpeedSpeed0(end+1) = mean(data.compRes(i).speed);
        elseif highSpeed+antiNum == 1
            meanSpeedFreq1(end+1) = data.compRes(i).frequency;
            meanSpeedSpeed1(end+1) = mean(data.compRes(i).speed);
        elseif highSpeed+antiNum == 2
            meanSpeedFreq2(end+1) = data.compRes(i).frequency;
            meanSpeedSpeed2(end+1) = mean(data.compRes(i).speed);
        end

    end

    if ~isempty(meanSpeedFreq0)
        plot(meanSpeedFreq0,meanSpeedSpeed0)
    end
    if ~isempty(meanSpeedFreq1)
        plot(meanSpeedFreq1,meanSpeedSpeed1)
    end
    if ~isempty(meanSpeedFreq2)
        plot(meanSpeedFreq2,meanSpeedSpeed2)
    end

    legend(legend_string{:})
    xlim([0 maxHz]);
    xlabel('Commutation Frequency (hz)');
    ylabel('Average Speed (rad/s)');
    grid on;
end

%% plot std torque ripple vs frequency
if(plotSTDTorqueVFreqLines)
    h_freq_cont = figure;
    clf;
    hold all;

    stdFreq0 = [];
    stdStd0 = [];
    stdFreq1 = [];
    stdStd1 = [];
    stdFreq2 = [];
    stdStd2 = [];
    legend_string = {};
    maxHz = 0;
    for i = 1:length(data.compRes)
        %plot timeseries
        if data.compRes(i).anticogEnable
            antiNum = 1;
            if i > 1 && data.compRes(i).pwm == data.compRes(i-1).pwm
                highSpeed = 1;
                legend_string{3} = ['Anticogged at Nominal PWM'];
            else
                highSpeed = 0;
                legend_string{2} = ['Anticogged'];
            end
        else
            option2 = 'x';
            antiString = 'nominal';
            antiNum = 0;
            highSpeed = 0;
            legend_string{1} = ['Nominal'];
        end
        if data.compRes(i).frequency > maxHz
            maxHz = data.compRes(i).frequency;
        end

        if highSpeed+antiNum == 0
            stdFreq0(end+1) = data.compRes(i).frequency*2;
            stdStd0(end+1) = std(data.compRes(i).daqData(:,6));
        elseif highSpeed+antiNum == 1
            stdFreq1(end+1) = data.compRes(i).frequency*2;
            stdStd1(end+1) = std(data.compRes(i).daqData(:,6));
        elseif highSpeed+antiNum == 2
            stdFreq2(end+1) = data.compRes(i).frequency*2;
            stdStd2(end+1) = std(data.compRes(i).daqData(:,6));
        end

        % compute torque resolution
        vRes = data.compRes(i).motor.state.voltage/(72000000/data.compRes(i).frequency/2);
        iRes = vRes/motorR;
        tRes(i) = iRes*motorKt;
        frequency(i) = data.compRes(i).frequency*2;

        vIn = data.compRes(i).motor.state.voltage*data.compRes(i).pwm/2; %PP to amplitude
        omega = 2*pi*data.compRes(i).frequency;   
        tau = motorLs/motorR;
        vOut = vIn/(omega^2*tau^2+1);
        iOut = vOut/motorR;
        tOut(i) = iOut*motorKt;

        encTickPerSec = 4096*mean(data.compRes(i).speed)/2/pi; % (enc ticks / rev) * (rad/s) * rev/rad = enc ticks/s
        commutationHz = data.compRes(i).frequency;
        vError = sqrt(sum((tickMonteCarlo(encTickPerSec,commutationHz).*motorWaveError).^2));
    %     vError = sum(tickMonteCarlo(encTickPerSec,commutationHz).*motorWaveError);
        iError = vError/motorR;
        tError(i) = iError*motorKt;
    end

    if ~isempty(stdFreq0)
        plot(stdFreq0,stdStd0)%*2 for up-down counter
    end
    if ~isempty(stdFreq1)
        plot(stdFreq1,stdStd1)%*2 for up-down counter
    end
    if ~isempty(stdFreq2)
        plot(stdFreq2,stdStd2)%*2 for up-down counter
    end
    plot(frequency,tRes*1000)
    plot(frequency,tOut*1000)
    plot(frequency,tError*1000)
    plot(frequency,sqrt(tRes.^2+tOut.^2+tError.^2)*1000)

    legend(legend_string{:},'PWM resolution','Osc current','Tick error','All error')
    xlim([0 maxHz*2]);
    % ylim([0 10]);
    xlabel('PWM Frequency (hz)');
    ylabel('Standard Deviation (Nmm)');
    grid on;
end

%% Plot std torque ripple vs speed
if(plotSTDTorqueVSpeed)
    h_speed2 = figure;
    clf;
    hold all;

    frequencies = [data.compRes(:).frequency];
    uniqueFrequencies = unique(frequencies);

    for i = 1:length(uniqueFrequencies)
        inds = find(frequencies == uniqueFrequencies(i));
        meanSpeeds = [];
        stdTorques = [];
        for j = 1:length(inds)
            meanSpeeds(j) = mean(data.compRes(inds(j)).speed);
            stdTorques(j) = std(data.compRes(inds(j)).daqData(:,6));
        end
        legend_string{i} = [num2str(uniqueFrequencies(i)) ' hz'];
        plot(meanSpeeds,stdTorques,'.-')
    end

    % for i = 1:length(data.compRes)
    %     %plot timeseries
    %     if data.compRes(i).anticogEnable
    %         option1 = 'o';
    %         antiString = 'anticogged';
    %     else
    %         option1 = 'x';
    %         antiString = 'nominal';
    %     end
    %     plot(mean(data.compRes(i).speed),std(data.compRes(i).daqData(:,6)), option1);
    %     %make name for legend
    %     legend_string{i} = [num2str(data.compRes(i).pwm) ' pwm, ' num2str(data.compRes(i).frequency) ' hz, ' antiString];
    % end

    legend(legend_string{:})
    xlabel('Speed (rad/s)');
    ylabel('Standard Deviation (Nmm)');
    grid on;
end

%% Plot torque fft v time
if(plotFFTTorqueVTime)
    h_FFTTorque = figure;
    clf;
    hold all;
    legend_string = {};
    for i = 1:length(data.compRes)
        Fs = 40000;
        L = length(data.compRes(i).daqTime);
        NFFT = 2^nextpow2(L);
        Y = fft(data.compRes(i).daqData(:,6),NFFT)/L;
        f = Fs/2*linspace(0,1,NFFT/2+1);
        plot(f,2*abs(Y(1:NFFT/2+1))) 
        legend_string{i} = [num2str(data.compRes(i).pwm) ' pwm, ' num2str(data.compRes(i).frequency*2) ' pwm hz, ' antiString];
    end
    legend(legend_string{:})

    xlabel('Frequency (1/s)');
    ylabel('|Torque(f)|');
    grid on;
end

%% Plot torque fft
if(plotFFTTorqueVRot)
    h_FFTTorque = figure;
    clf;
    hold all;
    legend_string = {};
    for i = 1:length(data.compRes)
        if data.compRes(i).frequency < 2000
        Fs = 40000;
        L = length(data.compRes(i).daqTime);
        NFFT = 2^nextpow2(L);
        Y = fft(data.compRes(i).daqData(:,6),NFFT)/L;
        f = Fs/2*linspace(0,1,NFFT/2+1)/(mean(data.compRes(i).speed)/2/pi);
        plot(f,2*abs(Y(1:NFFT/2+1))) 
        legend_string = {legend_string{:} [num2str(data.compRes(i).pwm) ' pwm, ' num2str(data.compRes(i).frequency*2) ' pwm hz']};
        end
    end
    legend(legend_string{:})

    xlabel('Frequency (1/rot)');
    ylabel('|Torque(f)|');
    grid on;
end