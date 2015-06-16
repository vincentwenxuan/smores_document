function appliedV = idealV2AppliedV(idealV, Vbatt, varargin)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% function appliedV = idealV2AppliedV(idealV, Vbatt, varargin)
%
% idealV2AppliedV takes in an ideal voltage and converts it to a commanded 
% voltage that would result with the idealV on the motor.  Vbatt is the 
% battery or power supply's voltage (motor.health.V.data).  idealV and
% Vbatt can either be vecotrs or scalars.  If both are vectors, they must
% be equal length.
%
% Default values are for Matthew Piccoli's BLDC Motor Driver V6
%
% OPTIONS:
% switchTicks: Number of PWM ticks that both FETs are turned off during switching (can be found using coggingPositionBased)
% pwmTicks: Maximum number of ticks possible by the PWM (motor.maxPWM.data)

%% Get all of the options, or use default values for them.
try
switchTicks     = getArgVal('switchTicks', varargin, 36); % Number of PWM ticks that both FETs are turned off during switching (can be found using coggingPositionBased)
catch err
    if strcmp(err.identifier,'MATLAB:UndefinedFunction')
        error('Cannot find getArgVal in the path.  It might be at http://svn.modlabupenn.org/libraries/Matlab_Utilities (rev 67). Find the directory it is in and use addpath() to make it visible.')
    end
end
pwmTicks        = getArgVal('pwmTicks', varargin, 1800); % Maximum number of ticks possible by the PWM

appliedV = (abs(idealV) + switchTicks/pwmTicks*Vbatt).*sign(idealV);