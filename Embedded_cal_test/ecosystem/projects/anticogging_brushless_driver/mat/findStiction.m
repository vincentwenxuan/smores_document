function [stictionPWM, minimumPWM] = findStiction(mot, direction, varargin)
    try
        verbose     = getArgVal('verbose', varargin, 'n');
    catch err
        if strcmp(err.identifier,'MATLAB:UndefinedFunction')
            error('Cannot find getArgVal in the path.  It might be at http://svn.modlabupenn.org/libraries/Matlab_Utilities (rev 67). Find the directory it is in and use addpath() to make it visible.')
        end
    end
    
    pwmStep = 1/(72000000/double(mot.get_commutation_hz())/2);

    %% start finding limits
    revolution = 0;
    oldPos = mot.get_state().position;
    newPos = oldPos;
    stictionPWM = 0;
    %Find stictionPWM
    out(verbose,'Finding stiction PWM');
    while revolution < 10
        while oldPos == newPos
            %Ramp PWM up slowly
            stictionPWM = stictionPWM + direction*pwmStep; %1000 ticks per pwm frequency
            mot.pwm_cmd = stictionPWM;
            oldPos = newPos;
            newPos = mot.get_state().position;
        end
        if pi < direction*(oldPos - newPos) %if reached 360/0
            revolution = revolution + 1;
        end
        mot.pwm_cmd = stictionPWM;
        oldPos = newPos;
        newPos = mot.get_state().position;
    end
    out(verbose, ['Found stiction PWM: ' num2str(stictionPWM)]);

    %Find minimumPWM
    minimumPWM = stictionPWM;
    while oldPos ~= newPos
        %Ramp PWM down slowly
        if pi < direction*(oldPos - newPos) %if reached 360/0
            revolution = revolution + 1;
            minimumPWM = minimumPWM - direction*pwmStep;
        end
        mot.pwm_cmd = minimumPWM;
        oldPos = newPos;
        newPos = mot.get_state().position;
    end
    minimumPWM = minimumPWM + direction*pwmStep;
    out(verbose, ['Found minimum PWM: ' num2str(minimumPWM)]);
end