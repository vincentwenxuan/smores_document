function idealV = torque2IdealV(torque, varargin)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% function idealV = torque2IdealV(torque, varargin)
% 
% torque2IdealV takes torque values in Nm and returns the ideal voltage
% across a motor to achieve the torque.  This ignores inductance effects.
% torque can be a scalar or vector.
% 
% Default values are for Exceed RC's Rocket 710Kv with the motor at rest.
%
% OPTIONS:
% R: Motor resistance in ohms
% Kv: Motor speed constant in RPM/V
% RPM: Motor speed in RPM.  RPM can be a scalar or vector of equal length
% to idealV

%% Get all of the options, or use default values for them.
try
R     = getArgVal('R', varargin, .22); % Boolean determines if the user receives updates via text on the console
catch err
    if strcmp(err.identifier,'MATLAB:UndefinedFunction')
        error('Cannot find getArgVal in the path.  It might be at http://svn.modlabupenn.org/libraries/Matlab_Utilities (rev 67). Find the directory it is in and use addpath() to make it visible.')
    end
end
Kv     = getArgVal('Kv', varargin, 710); % Boolean determines if the user receives updates via text on the console
RPM     = getArgVal('RPM', varargin, 0); % Motor speed in RPM.  RPM can be a scalar or vector of equal length to idealV

Kt = 60/2/pi/Kv;
idealV = Kv*RPM+torque*Kt*R;
