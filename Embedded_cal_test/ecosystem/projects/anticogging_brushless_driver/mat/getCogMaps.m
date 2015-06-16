function getCogMaps(motor,varargin)

%% Get all of the options, or use default values for them.
    try
    verbose     = getArgVal('verbose', varargin, true); % Boolean determines if the user receives updates via text on the console
    catch err
        if strcmp(err.identifier,'MATLAB:UndefinedFunction')
            error('Cannot find getArgVal in the path.  It might be at http://svn.modlabupenn.org/libraries/Matlab_Utilities (rev 67). Find the directory it is in and use addpath() to make it visible.')
        end
    end
    comPort = getArgVal('com', varargin, 'COM7'); % Com port to talk to the motor driver
    frequencies = getArgVal('frequencies', varargin, 550*1.33.^(0:17)); % Commutation frequencies to test.  pwm resolution is 72000000/frequency/2    
    outPathRoot     = getArgVal('outDir', varargin, './coggingMapData/');
    
%% Initialize all of the things!
    out(verbose,'Initializing module');
    com = MessageInterface(comPort,115200);
    mot = BrushlessController('com',com,varargin{:});
    mot.pwm_cmd = 0; % Make sure the motor is stopped
    cog = Anticogging('com',com,varargin{:});
    mot.set_electric_lead_rate(0);
    out(verbose,'Initialized module');
    
    cleanupObj = onCleanup(@()getCogMapsCleanup(com,mot,cog,verbose));
    
%% Collect data
    for i = 1:length(frequencies)
        out(verbose,['getCogMaps running frequency ' num2str(frequencies(i)) ' hz. ' num2str((i-1)/length(frequencies)*100) '% done.']);
        mot.set_commutation_hz(frequencies(i));
        [trialData, ~] = coggingMethodComparison('com',com,'nonstop',true,'performPosition',true,'performSpeed',false,'performTorque',false,varargin{:});
        trialData.frequency = frequencies(i);
        outPath = strcat(outPathRoot, motor, '_', num2str(frequencies(i)), 'hz_', regexprep(num2str(clock()), '\s*', '-'), '.mat'); % Matlab strings are dark comedy.
        save(outPath,'trialData');
    end
    
%% Cleanup function
    %Call me when you're done with this function
    function getCogMapsCleanup(com,mot,cog,verbose)
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
        out(verbose,'getCogMaps cleaned');
    end
end