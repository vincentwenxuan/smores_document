function [stictionPWM, minimumPWM] = getRolling(mot, stictionPWM, minimumPWM, varargin)
    try
        verbose     = getArgVal('verbose', varargin, 'n');
    catch err
        if strcmp(err.identifier,'MATLAB:UndefinedFunction')
            error('Cannot find getArgVal in the path.  It might be at http://svn.modlabupenn.org/libraries/Matlab_Utilities (rev 67). Find the directory it is in and use addpath() to make it visible.')
        end
    end
    direction = sign(minimumPWM);
    pwmStep = 1/(72000000/double(mot.get_commutation_hz())/2);
    
    %% Get rolling again
    out(verbose, 'Breaking stiction');
    revolution = 0;
    rollingStart = 0;
    oldPos = mot.get_state().position;
    newPos = oldPos;
    while revolution < rollingStart + 3
        if pi < direction*(oldPos - newPos) %if reached 360/0
            revolution = revolution + 1;
        end
        while oldPos == newPos
            stictionPWM = stictionPWM + direction*pwmStep;
            mot.pwm_cmd = stictionPWM;
            oldPos = newPos;
            newPos = mot.get_state().position;
        end
        mot.pwm_cmd = stictionPWM;
        oldPos = newPos;
        newPos = mot.get_state().position;
    end
    
    %% Keep rolling
    out(verbose, 'Rolling at minimum speed');
    while revolution < rollingStart + 10
        if pi < direction*(oldPos - newPos) %if reached 360/0
            revolution = revolution + 1;
        end
        if oldPos == newPos % if we stopped
            % increase minimumPWM
            minimumPWM = minimumPWM + direction*pwmStep;
            % break stiction, reset rollingStart
            mot.pwm_cmd = stictionPWM;
            rollingStart = 3;
            newPos = mot.get_state().position;
        end
        mot.pwm_cmd = minimumPWM;
        oldPos = newPos;
        newPos = mot.get_state().position;
    end
end