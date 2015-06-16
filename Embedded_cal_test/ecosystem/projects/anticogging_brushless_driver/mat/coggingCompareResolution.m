function [data] = coggingCompareResolution(varargin)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                Compare cogging vs pwm resolution
%                     using the F/T sensor
%  Use the F/T sensor to determine if cogging torque is dependent on pwm
%  resolution.
%  
%  Contact: Matthew Piccoli, piccoli@seas.upenn.edu
%  Date: October 21, 2014
%
%  data is a struct with a motor description and necessary values for the 
%  trials that were run.
%
%
% OPTIONS:
% name: Optional name to label this trial
% verbose: Boolean determines if the user receives updates via text on the console
% comPort: Com port to talk to the motor driver
% anticogEnable: Enable or disable anticogging, false = 0, true = 1, both using nominal's pwm=2
% testTime: Time to run each setting
% frequencies: Commutation frequencies to test.  pwm resolution is 72000000/frequency/2
% directions: directions to test (-1 or 1), vectors acceptable
% testRate: F/T sensor sample rate
%
%ex: data = coggingCompareResolution('com','COM7','cogEnable',true,'verbose',true,'directions',[1,-1],'frequencies',[1000,10000,100000],'encoder_counts',4096);
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%% Get all of the options, or use default values for them.
    try
    name        = getArgVal('name',   varargin, date());
    catch err
        if strcmp(err.identifier,'MATLAB:UndefinedFunction')
            error('Cannot find getArgVal in the path.  It might be at http://svn.modlabupenn.org/libraries/Matlab_Utilities (rev 67). Find the directory it is in and use addpath() to make it visible.')
        end
    end
    verbose     = getArgVal('verbose', varargin, true); % Boolean determines if the user receives updates via text on the console
    comPort = getArgVal('com', varargin, 'COM7'); % Com port to talk to the motor driver
    anticogEnable = getArgVal('anticogEnable', varargin, true); % enable or disable anticogging, true = enable
    testTime = getArgVal('testTime', varargin, 4); % time to run each setting
    testRate = getArgVal('testRate', varargin, 40000); % F/T sensor sample rate
    frequencies = getArgVal('frequencies', varargin, [1000, 5000, 10000, 50000]); % Commutation frequencies to test.  pwm resolution is 72000000/frequency/2
    directions = getArgVal('directions', varargin, [1 -1]); % directions to test (-1 or 1), vectors acceptable    

%% Initialize all of the things!
    out(verbose,'Initializing module');
    com = MessageInterface(comPort,115200);
    mot = BrushlessController('com',com,varargin{:});
    mot.pwm_cmd = 0; % Make sure the motor is stopped
    cog = Anticogging('com',com,varargin{:});
    if anticogEnable
        cog.Enable();
    else
        cog.Disable();
    end
    mot.set_electric_lead_rate(0);
    out(verbose,'Initialized module');
    
    out(verbose,'Initializing F/T sensor');
    [s, calibration, dataZero] = initAndZeroDaq();
    s.DurationInSeconds = testTime;
    s.Rate = testRate;
%     queueOutputData(s,zeros(s.Rate*s.DurationInSeconds,6));
    s.NotifyWhenDataAvailableExceeds = s.Rate*s.DurationInSeconds; % -1?
    lh = s.addlistener('DataAvailable', @getData); 
    out(verbose,'Initialized F/T sensor');

    %Setup cleanup
    cleanupObj = onCleanup(@()coggingCompareResolutionCleanup(com,mot,cog,s,lh,verbose));
    
%% Run the tests
    trialNum = 1;
    cogAnti = 0;
    for i = 1:length(directions) % for each desired pwm
        for j = 1:length(frequencies) % and for each desired commutation frequency
            for k = 1:length(anticogEnable)
                if anticogEnable(k) == 1
                    performStiction = true;
                    cog.Enable();
                    data.compRes(trialNum).anticogEnable = true;
                elseif anticogEnable(k) == 0
                    performStiction = true;
                    cog.Disable();
                    data.compRes(trialNum).anticogEnable = false;
                elseif anticogEnable(k) == 2
                    if cogAnti == 0
                        performStiction = true;
                        cog.Disable();
                        cogAnti = 1;
                        data.compRes(trialNum).anticogEnable = false;
                    else
                        performStiction = false;
                        cog.Enable;
                        cogAnti = 0;
                        data.compRes(trialNum).anticogEnable = true;
                    end
                end
                
                % setup trial
                iteration = 1;
                daqTime = zeros(s.Rate*s.DurationInSeconds,1);
                daqData = zeros(s.Rate*s.DurationInSeconds,6);
                data.compRes(trialNum).frequency = frequencies(j);
                mot.set_commutation_hz(frequencies(j));
                out(verbose,['cog: ' num2str(anticogEnable(k)) ', pwm: ' num2str(directions(i)) ', frequency: ' num2str(frequencies(j))]);
                
                % find stiction
                if performStiction
                    % stop
                    mot.pwm_cmd = 0;
                    pause(2); % to make sure we're stopped
                    [stictionPWM, minimumPWM] = findStiction(mot, directions(i), varargin{:});
                    [stictionPWM, minimumPWM] = getRolling(mot, stictionPWM, minimumPWM, varargin{:});
                end
                
                data.compRes(trialNum).pwm = minimumPWM;

                % Begin trial
                daqComplete = false;
                s.startBackground();
                startTime = toc;
%                 while(toc < startTime + testTime) % send motor commands while waiting
                while(~daqComplete) % send motor commands while waiting
                    mot.pwm_cmd = minimumPWM;
                    tempState = mot.get_state();
                    data.compRes(trialNum).time(iteration) = toc - startTime;
                    data.compRes(trialNum).position(iteration) = tempState.position;
                    data.compRes(trialNum).speed(iteration) = tempState.speed;
                    iteration = iteration + 1;
                    pause(.00000001);
                end
                pause(.01);
                
                % gather data from daqTime and daqData
                data.compRes(trialNum).daqTime = daqTime;
                data.compRes(trialNum).daqData = daqData*calibration' - ones(size(daqData,1),1)*dataZero;
                
                % Collect remaining info
                data.compRes(trialNum).motor = mot;
                data.compRes(trialNum).trialName = name;
                data.compRes(trialNum).trialDate = date();
                data.compRes(trialNum).trialNow = now();
                
                trialNum = trialNum + 1;
            end % for cogEnable
        end % for frequencies
    end % for pwms
    

function getData(src,event)
     daqTime = event.TimeStamps;
     daqData = event.Data;
     daqComplete = true;
end
    
%% Cleanup function
    %Call me when you're done with this function
    function coggingCompareResolutionCleanup(com,mot,cog,s,lh,verbose)
        mot.pwm_cmd = 0;
        out(verbose,'Clearing objects');
        if ~isempty(mot)
            clear mot;
        end
        if ~isempty(cog)
            clear cog;
        end
        if ~isempty(com)
            clear com;
        end
        if ~isempty(s)
            clear s
        end
        if ~isempty(lh)
            delete(lh)
        end
        out(verbose,'coggingForceBased cleaned');
    end
end