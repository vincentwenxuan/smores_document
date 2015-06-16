function data = coggingSpeedBased(varargin)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%             Motor Cogging Characterization using Speed Data
%  Use the speed data from Matt Piccoli's motor driver board to try to get 
%  the force function associated with motor cogging for a particular motor.
%  
%  Contact: Matthew Piccoli, piccoli@seas.upenn.edu
%  Date: November 10, 2012
%  Updated: January 15, 2014
%
% OPTIONS:
% name: Optional name to label this trial
% outDir: Destination directory for the save data
% tTrial: [s] - time length of trial
% verbose: Boolean determines if the user receives updates via text on the console
% nonstop: Boolean determines if user is promped (to execute, save, etc.)
% id: Motor id of the *MOTOR CONTROLLER BOARD*
% pwm: PWM command on top of minimum rolling pwm (-1.0 to 1.0) (for tests at higher speeds)
% direction: Direction of motor spinning: 1 = forward, -1 = backward
% CVTest: Boolean that determines whether the constant voltage test is performed
% RDTest: Boolean that determines whether the ramp down test is performed
% note: Optional note to store in json results under 'note' key
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    
%% Get all of the options, or use default values for them.
try
name        = getArgVal('name',   varargin, date());
catch err
    if strcmp(err.identifier,'MATLAB:UndefinedFunction')
        error('Cannot find getArgVal in the path.  It might be at http://svn.modlabupenn.org/libraries/Matlab_Utilities (rev 67). Find the directory it is in and use addpath() to make it visible.')
    end
end
tTrial      = getArgVal('tTrial', varargin, 5);   % [s] - time length of trial
verbose     = getArgVal('verbose', varargin, false); % Boolean determines if the user receives updates via text on the console
nonstop     = getArgVal('nonstop', varargin, false); % Boolean determines if user is promped (to execute, save, etc.)
pwmCom      = getArgVal('pwm',    varargin, 0); % PWM command on top of minimum rolling pwm (-1.0 to 1.0) (for tests at higher speeds) % Switching to findStiction (MPP 2012-11-28)
direction   = getArgVal('direction', varargin, 1); % Direction of motor spinning: 1 = forward, -1 = backward
CVTest      = getArgVal('CVTest', varargin, true); % Boolean that determines whether the constant voltage test is performed
RDTest      = getArgVal('RDTest', varargin, false); % Boolean that determines whether the ramp down test is performed
note        = getArgVal('note',   varargin, '');  % Optional note to store in json results under 'note' key

if ((pwmCom > 1.0) || (pwmCom < -1.0))
    error('The ''pwm'' option value must be between -1.0 and 1.0');
end
outPathRoot = getArgVal('outDir', varargin, './forceBasedJsonData/');
outPath = strcat(outPathRoot, name, '_', regexprep(num2str(clock()), '\s*', '-'), '.json'); % Matlab strings are dark comedy.
motorSampleRate = 1000; % Estimated, probably way less than this.  (MPP: Nope, belive it!)

%% Initialize all of the things!
out(verbose,'Initializing module');
mot = BrushlessController(varargin{:});
mot.pwm_cmd = 0; % Make sure the motor is stopped
out(verbose,'Initialized module');

%Setup cleanup
cleanupObj = onCleanup(@()coggingSpeedBasedCleanup(mot,verbose));

if strcmpi(nonstop,'n');
    input('To begin the test, hit Enter.');
end

[stictionPWM, minimumPWM] = findStiction(mot, direction,varargin{:});
getRolling(mot, stictionPWM, minimumPWM+pwmCom*direction, varargin{:});

%% Run the Constant Voltage (CV) trial.
if CVTest
    dataCV = runTrial(name, ...
                    tTrial, minimumPWM+pwmCom*direction, motorSampleRate);
end
                  
%% Now run the ramp down (RD) test after making sure the motor is first spun up.
if RDTest
    dataRD = runTrial(name, ...
                 tTrial, 0.0, motorSampleRate);
end
% For sanity's sake, set pwm to 0.0 even though the last trial should have.
mot.pwm_cmd = 0.0; 

%% Store the trials and the note string in the data struct then
%  output it to file as a json object.
if CVTest
    data.constantVoltage = dataCV;
end
if RDTest
    data.rampDown = dataRD;
end
data.motor = mot;
data.note = note;
data.direction = direction;

% Output the data to a json file.
if strcmpi(nonstop,'n');
    fprintf('Outputting all data to ''%s'' in json format.\n', outPath);
    reply = input('Is this alright?  Y/N [N]: ', 's');
    if isempty(reply)
        reply = 'N';
    end
    if strcmpi(reply,'Y') % Save json data
        json = savejson(data);
        fh = fopen(outPath, 'w');
        fprintf(fh, json);
        fclose(fh);
        out(verbose,'Data written');
    else
        out(verbose,'No data written');
    end
end
out(verbose,'Woot!  Success.');


%% Utility functions.
    function data = runTrial(name, tTrial, pwmTrial, ...
                      motorSampleRate)
        data.trialName = name;
        data.trialDate = date();
        data.trialNow = now();
        data.trialLen = tTrial;
        
        temp_state = mot.get_state();
        temp_state.time = 0;
        data.state(motorSampleRate*tTrial) = temp_state;
        
        data.pwmCommand = pwmTrial;
        k = 1;
        % Let it spin up
        t0 = toc;   % Initializing a motor calls tic, and can freak out if it is
        % called again.  So, capture toc for time offset.
        tCur = t0;
        while (tCur < (t0+tTrial))
            tCur = toc;
            temp_state = mot.get_state();
            temp_state.time = tCur - t0;
            data.state(k) = temp_state;
            k = k+1;
        end
        data.state = data.state(1:k-1); % Remove any empty data
    end

%% Cleanup function
    %Call me when you're done with this function
    function coggingSpeedBasedCleanup(mot,verbose)
        mot.pwm_cmd = 0;
        out(verbose,'Clearing motor and communication monitor');
        if ~isempty(mot)
            clear mot;
        end
        out(verbose,'coggingSpeedBased cleaned');
    end
end
