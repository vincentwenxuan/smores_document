function idealV = appliedV2IdealV(appliedV, Vbatt, varargin)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% function idealV = appliedV2IdealV(appliedV, Vbatt, varargin)
%
% appliedV2IdealV takes in a reported voltage from a motor driver and
% outputs the actual voltage applied.  Vbatt is the battery or power
% supply's voltage (motor.health.V.data)
%
% Default values are for Matthew Piccoli's BLDC Motor Driver V6
%
% OPTIONS:
% switchTicks: Number of PWM ticks that both FETs are turned off during switching (can be found using coggingPositionBased)
% pwmTicks: Maximum number of ticks possible by the PWM (motor.maxPWM.data)

%% Get all of the options, or use default values for them.
try
switchTicks     = getArgVal('switchTicks', varargin, 0); % Number of PWM ticks that both FETs are turned off during switching (can be found using coggingPositionBased)
catch err
    if strcmp(err.identifier,'MATLAB:UndefinedFunction')
        error('Cannot find getArgVal in the path.  It might be at http://svn.modlabupenn.org/libraries/Matlab_Utilities (rev 67). Find the directory it is in and use addpath() to make it visible.')
    end
end
pwmTicks        = getArgVal('pwmTicks', varargin, 1800); % Maximum number of ticks possible by the PWM

idealV = max((abs(appliedV) - switchTicks/pwmTicks*Vbatt),0).*sign(appliedV);