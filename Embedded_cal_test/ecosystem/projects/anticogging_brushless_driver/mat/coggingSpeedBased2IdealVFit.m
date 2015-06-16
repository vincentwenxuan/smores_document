function [idealVAccFit x] = coggingSpeedBased2IdealVFit(data, varargin)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%             Motor Cogging Modeling Using Fourier Transform
%  Using the outputted data from the coggingForceBased or coggingSpeedBased
%  functions, coggingSpeedBased2IdealVFit maps the accelerations to the 
%  ideal motor volages (volages felt across the motor) and performs a 
%  fourier transform using Shai Revzen's ft utilities.  

%  Default motor values are for the Exceed RC Rocket 710Kv (although 
%  measurements suggest 1022Kv)
%
%  Default motor driver values are for Matthew Piccoli's TI BLDC Driver V6
%
%  idealVPosFit are the fitted ideal voltage values calculated from the acceleration
%
%  x are the positions corresponding the the torque values
%  
%  Contact: Matthew Piccoli, piccoli@seas.upenn.edu
%  Date: November 07, 2012
%
%  OPTIONS:
%  verbose: Boolean determines if the user receives updates via text on the console
%  switchTicks: Number of PWM ticks that both FETs are turned off during switching (can be found using coggingPositionBased)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%% Get all of the options, or use default values for them.
try
verbose     = getArgVal('verbose', varargin, false); % Boolean determines if the user receives updates via text on the console
catch err
    if strcmp(err.identifier,'MATLAB:UndefinedFunction')
        error('Cannot find getArgVal in the path.  It might be at http://svn.modlabupenn.org/libraries/Matlab_Utilities (rev 67). Find the directory it is in and use addpath() to make it visible.')
    end
end

%% Make sure we can see everything we need that is not builtin to Matlab
pathQuery('ftFit', 'http://svn.modlabupenn.org/libraries/Matlab_Utilities/ft');

%% Extract useful data
out(verbose, 'Extracting Useful Data');
mTime = [data.constantVoltage.state(:).time]; 
mTime = mTime + mean(diff(mTime))*.5; % adjusting - 1 diff for speed sampling and +.5 diff for diff
mPos = [data.constantVoltage.state(:).position];
mSpd = [data.constantVoltage.state(:).speed];
mAcc = diff(mSpd)./diff(mTime); % dDPS/dt
%need to know ENCODER_COUNTS
encoderTicks = getArgVal('encoder_counts', varargin, []);
if isempty(encoderTicks)
    encoderTicks = input(['The number of encoder counts is required but is unable to ask the driver directly.' char(10) 'Please type the number of encoder counts then press Enter: ']);
end
pwmTicks = 1800;
V = mean([data.constantVoltage.state(:).voltage]);
direction = data.direction;
pwmCmd = data.constantVoltage.pwmCommand;

x = linspace(0,2*pi-2*pi/encoderTicks,encoderTicks);

%% FT on acc
out(verbose, 'Fitting Voltages From Acceleration');
accFit = ftFit(100, mPos(1:end-1), mAcc);
yAccFit = ftVal(accFit,x);
yAccMax = max(abs(yAccFit));
idealMaxV = (pwmCmd - 1/pwmTicks)*V; % 1/pwmTicks used to be 2/pwmTicks... but why?  MPP 11/7/2012
accToVRatio = direction*idealMaxV/yAccMax; 
idealVAccFit = -accToVRatio*yAccFit;

