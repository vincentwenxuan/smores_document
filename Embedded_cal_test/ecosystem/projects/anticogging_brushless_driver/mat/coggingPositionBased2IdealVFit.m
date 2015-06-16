function [idealVPosFit x] = coggingPositionBased2IdealVFit(data, varargin)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%             Motor Cogging Modeling Using Fourier Transform
%  Using the outputted data from the coggingPositionBased function, 
%  coggingPositionBased2IdealVFit maps the applied motor voltages to the ideal 
%  motor volages (volages felt across the motor) and performs a fourier 
%  transform using Shai Revzen's ft utilities.  
%
%  Default motor values are for the Exceed RC Rocket 710Kv (although 
%  measurements suggest 1022Kv)
%
%  Default motor driver values are for Matthew Piccoli's TI BLDC Driver V6
%
%  idealVPosFit are the fitted ideal voltage values from the applied 
%  voltage data
%
%  x are the positions corresponding the the voltage values
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
dataPos = data.posCtrl.motorData;
encoderTicks = 4096;
pwmTicks = double(72000000/data.motor.commutation_hz/2);
V = data.motor.state.voltage

x = linspace(0,2*pi-2*pi/encoderTicks,encoderTicks);

%% Convert applied voltages into ideal voltages
out(verbose, 'Converting Voltages to Torques');
idealV = appliedV2IdealV(dataPos(1,:)*V, V, varargin{:}, 'pwmTicks', pwmTicks);

%% Fit ideal voltages
out(verbose, 'Fitting ideal voltages');
posFit = ftFit(100, dataPos(2,:), idealV);
idealVPosFit = ftVal(posFit,x);
