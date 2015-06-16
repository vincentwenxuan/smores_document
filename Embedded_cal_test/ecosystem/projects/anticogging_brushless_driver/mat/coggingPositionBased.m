function data = coggingPositionBased(varargin)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%             Motor Cogging Characterization using
%                   Position and Voltage Data
%  Use sensed position and applied PWM from Matthew Piccoli's motor driver 
%  board to try to get the force function associated with motor cogging
%                  for a particular motor.
%  
%  Contact: Matthew Piccoli, piccoli@seas.upenn.edu
%  Date: November 06, 2012
%
%  data is a struct with a motor description and necessary values for the 
%  trials that were run.
%
% OPTIONS:
% name: Optional name to label this trial
% verbose: Boolean determines if the user receives updates via text on the console
% nonstop: Boolean determines if user is promped (to execute, save, etc.)
% id: Motor id of the *MOTOR CONTROLLER BOARD*
% direction: Direction of motor spinning: 1 = forward, -1 = backward
% startPos: If sensing less then one revolution, specifies starting point.  Requires endPos.  startPos == endPos yelds no limit.
% endPos: If sensing less then one revolution, specifies ending point.  Requires startPos.  startPos == endPos yelds no limit.
% note: Optional note to store in json results under 'note' key
% kp: Temporary P gain for the position PID
% ki: Temporary I gain for the position PID
% kd: Temporary D gain for the position PID
% kc: Temporary I clamp for the position PID
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    %% Get all of the options, or use default values for them.
    try
    name        = getArgVal('name',   varargin, date());
    catch err
        if strcmp(err.identifier,'MATLAB:UndefinedFunction')
            error('Cannot find getArgVal in the path.  It might be at http://svn.modlabupenn.org/libraries/Matlab_Utilities (rev 67). Find the directory it is in and use addpath() to make it visible.')
        end
    end
    verbose     = getArgVal('verbose', varargin, false); % Boolean determines if the user receives updates via text on the console
    nonstop     = getArgVal('nonstop', varargin, false); % Boolean determines if user is promped (to execute, save, etc.)
    direction   = getArgVal('direction', varargin, 1); % Direction of motor spinning: 1 = forward, -1 = backward
    note        = getArgVal('note',   varargin, '');  % Optional note to store in json results under 'note' key
    startPos    = getArgVal('startPos', varargin, 0); % If sensing less then one revolution, specifies a position limit.  Requires endPos.  startPos == endPos yelds no limit.
    endPos      = getArgVal('endPos', varargin, 0); % If sensing less then one revolution, specifies a position limit.  Requires startPos.  startPos == endPos yelds no limit.
    kps          = getArgVal('kp', varargin, 1); % Temporary P gain for the position PID
    kis          = getArgVal('ki', varargin, 0); % Temporary I gain for the position PID
    kds          = getArgVal('kd', varargin, 0); % Temporary D gain for the position PID
    rotations       = getArgVal('rotations', varargin, 1); % Number of rotations

    outPathRoot = getArgVal('outDir', varargin, './positionBasedJsonData/');
    outPath = strcat(outPathRoot, name, '_', regexprep(num2str(clock()), '\s*', '-'), '.json'); % Matlab strings are dark comedy.

    %% Initialize all of the things!
    out(verbose,'Initializing module');
    mot = BrushlessController(varargin{:});
    mot.pwm_cmd = 0; % Make sure the motor is stopped
    out(verbose,'Initialized module');
    
    %Setup cleanup
    cleanupObj = onCleanup(@()coggingPositionBasedCleanup(mot,verbose));

    out(verbose,'Initializing parameters');
    pos_ctrl.Kp = kps(1);
    pos_ctrl.Ki = kis(1);
    pos_ctrl.Kd = kds(1);
    mot.set_position_control(pos_ctrl);
    out(verbose,'Initialized parameters');

    data.posCtrl.trialName = name;
    data.posCtrl.trialDate = date();
    data.posCtrl.trialNow = now();
    data.posCtrl.motorDataFields = 'pwm [pu], position [rad], desired position [rad]'; % TODO: This is nasty.  Make it nice (array.  when I tried, either matlab squashes the string here or savejson squashes it.)

    if strcmpi(nonstop,'n');
        input('To begin the test, hit Enter.');
    end

    %% Generate trajectory
    out(verbose, 'Generating trajectory');
    initialPos = mot.get_state.position;
    if startPos == endPos
        startPos = initialPos;
        endPos = initialPos-2*pi/4096*direction;
    end
    data.posCtrl.direction = direction;
    data.posCtrl.startPos = startPos;
    data.posCtrl.endPos = endPos;
    initialDelta = startPos - initialPos;
    initialDirection = (-1)^((abs(initialDelta) < pi) + (initialDelta >= 0));
    trajectoryInit = generateTrajectory(initialPos, startPos, initialDirection);
    trajectoryPos = generateTrajectory(startPos, endPos, direction);
    out(verbose, 'Generated trajectory');

    %% Get to startPos (execute trajectoryInit)
    out(verbose,'Starting Initial Trajectory');
    for desiredPos = trajectoryInit
    %Send new position command
        mot.position_cmd = desiredPos;
    %Wait a bit (rotate at 1/3.6 hz)
        pause(.01);
    end
    out(verbose,'Finished Initial Trajectory');

    %% Cycle through trajectory forwards
    out(verbose,'Starting Test Trajectory');
    data.posCtrl.motorData = zeros(3,length(trajectoryPos));
    oldPos = mot.get_state.position;
    iteration = 1;
    for i = 1:length(kps)
        out(verbose,'Setting gains');
        pos_ctrl.Kp = kps(i);
        pos_ctrl.Ki = kis(i);
        pos_ctrl.Kd = kds(i);
        mot.set_position_control(pos_ctrl);
        out(verbose,'Set gains');
        for rot = rotations
            for desiredPos = trajectoryPos
            %Send new position command
                mot.position_cmd = desiredPos;
            %Wait until the motor stops
                pause(.01);
                temp_pos = mot.get_state.position;
                while temp_pos ~= oldPos
                    oldPos = temp_pos;
                    pause(.01);
                    temp_pos = mot.get_state.position;
                end
            %Log the current position, pwm, and voltage (well, really current)
                temp_state = mot.get_state;
                data.posCtrl.motorData(1,iteration) = temp_state.pwm;
                data.posCtrl.motorData(2,iteration) = temp_state.position;
                data.posCtrl.motorData(3,iteration) = desiredPos;
                iteration = iteration + 1;
            end
        end
    end
    out(verbose,'Finished Test Trajectory');

    mot.get_commutation_hz();
    data.motor = mot;
    data.note = note;
    data.direction = direction;

    %% Output the data to a json file.
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

    %% Generate Trajectories
    function trajectory = generateTrajectory(startP, endP, dir)
        if dir == 1 && endP < startP % if forward and there's a wrap
            endP = endP + 2*pi;
        elseif dir == -1 && endP > startP % if backward and there's a wrap
            startP = startP + 2*pi;
        end
        trajectory = startP:dir*2*pi/4096:endP;
        trajectory = mod(trajectory,2*pi);
    end

    %% Cleanup function
    %Call me when you're done with this function
    function coggingPositionBasedCleanup(mot,verbose)
        mot.pwm_cmd = 0;
        out(verbose,'Clearing motor and communication monitor');
        if ~isempty(mot)
            clear mot;
        end
        out(verbose,'coggingForceBased cleaned');
    end

end