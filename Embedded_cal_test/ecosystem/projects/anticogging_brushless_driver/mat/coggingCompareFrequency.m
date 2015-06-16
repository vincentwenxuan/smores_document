function [data] = coggingCompareFrequency(motor,varargin)
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
    testTime = getArgVal('testTime', varargin, 4); % time to run each setting
    testRate = getArgVal('testRate', varargin, 40000); % F/T sensor sample rate
    directions = getArgVal('directions', varargin, [1 -1]); % directions to test (-1 or 1), vectors acceptable 
    outPathRoot     = getArgVal('outDir', varargin, './coggingMapData/');

%% Initialize all of the things!
    out(verbose,'Initializing module');
    com = MessageInterface(comPort,115200);
    mot = BrushlessController('com',com,varargin{:});
    mot.pwm_cmd = 0; % Make sure the motor is stopped
    cog = Anticogging('com',com,varargin{:});
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
    
%% Get all trial data
    listing = dir([outPathRoot '/' motor '*.mat']);
    listingNames = {listing.name};

%% Setup cleanup
    cleanupObj = onCleanup(@()coggingCompareFrequencyCleanup(com,mot,cog,s,lh,verbose));
    
%% Run the tests
    trialNum = 1;
    for i = 1:length(directions) % for each desired pwm
        for j = 1:length(listingNames) % and for each desired commutation frequency
            % setup trial
            iteration = 1;
            load([outPathRoot listingNames{j}]);
            daqTime = zeros(s.Rate*s.DurationInSeconds,1);
            daqData = zeros(s.Rate*s.DurationInSeconds,6);
            data.compRes(trialNum).frequency = trialData.frequency;
            mot.set_commutation_hz(trialData.frequency);
            cog.Disable();
            
            [trialData, uploaded] = coggingMethodComparison('com',com,'trialData',trialData,'switchTicks',0,'performPosition',true,'performSpeed',false,'performTorque',false,'writeReply','p','addReply',' ',varargin{:});
            data.compRes(trialNum).wave = uploaded;
            cog.Enable();
            
            out(verbose,['pwm: ' num2str(directions(i)) ', frequency: ' num2str(trialData.frequency)]);

            % find stiction
            mot.pwm_cmd = 0;
            pause(1); % to make sure we're stopped
            [stictionPWM, minimumPWM] = findStiction(mot, directions(i), varargin{:});
            [stictionPWM, minimumPWM] = getRolling(mot, stictionPWM, minimumPWM, varargin{:});

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
        end % for frequencies
    end % for pwms
    
%% Sort by frequency
    out(verbose,'Sorting by frequency');
    frequencies = [data.compRes(:).frequency];
    [f, i] = sort(frequencies);
    data.compRes = data.compRes(i);
    
function getData(src,event)
     daqTime = event.TimeStamps;
     daqData = event.Data;
     daqComplete = true;
end
    
%% Cleanup function
    %Call me when you're done with this function
    function coggingCompareFrequencyCleanup(com,mot,cog,s,lh,verbose)
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