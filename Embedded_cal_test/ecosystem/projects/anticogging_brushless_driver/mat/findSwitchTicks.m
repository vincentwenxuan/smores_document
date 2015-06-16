function [switchTicksMax switchTicksMean switchTicksStd stictionTicksMax stictionTicksMean stictionTicksStd] = findSwitchTicks(dataPosFw, dataPosBw, varargin)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% function switchTime = findSwitchTicks(dataPosFw, dataPosBw, varargin)
%
% findSwitchTicks finds the number of PWM ticks that the motor driver is
% incapable of applying voltage.  
%
% dataPosFw is the outputted data from coggingPositionBased run forward
%
% dataPosBw is the outputted data from coggingPositionBased run backward
%
% OPTIONS:
% verbose: Boolean determines if the user receives updates via text on the console

%% Get all of the options, or use default values for them.
try
verbose     = getArgVal('verbose', varargin, false); % Boolean determines if the user receives updates via text on the console
catch err
    if strcmp(err.identifier,'MATLAB:UndefinedFunction')
        error('Cannot find getArgVal in the path.  It might be at http://svn.modlabupenn.org/libraries/Matlab_Utilities (rev 67). Find the directory it is in and use addpath() to make it visible.')
    end
end

% Check that the two position trials were done similarly

encoderTicks = 4096;
pwmTicks = 1800;
%positions = linspace(0,360-360/encoderTicks,encoderTicks) + min(min(dataPosFw.posCtrl.motorData(2,:)),min(dataPosBw.posCtrl.motorData(2,:)));
positions = linspace(0,2*pi-2*pi/encoderTicks,encoderTicks);
%% Find the range of unusable ticks
for i = 1:length(positions)
    positionIndFw = find(dataPosFw.posCtrl.motorData(2,:) >= positions(i)-.0001 & dataPosFw.posCtrl.motorData(2,:) <= positions(i)+.0001);
    positionIndBw = find(dataPosBw.posCtrl.motorData(2,:) >= positions(i)-.0001 & dataPosBw.posCtrl.motorData(2,:) <= positions(i)+.0001);
    if ~isempty(positionIndFw) && ~isempty(positionIndBw)
        posMaxPWM(i) = max(dataPosFw.posCtrl.motorData(1,positionIndFw));
        posMinPWM(i) =  min(dataPosBw.posCtrl.motorData(1,positionIndBw));
        tickDifferences(i) = posMaxPWM(i) - posMinPWM(i);
    else
        posMaxPWM(i) = nan;
        posMinPWM(i) =  nan;
        tickDifferences(i) = nan;
    end
end
switchTicksMax = max(tickDifferences)/2*pwmTicks;

%% Find stiction and switch ticks max, mean, and std
switchValsMin = abs(posMinPWM)*pwmTicks < switchTicksMax;
switchValsMax = abs(posMaxPWM)*pwmTicks < switchTicksMax;
switchVals = switchValsMin & switchValsMax;
stictionValsMin = abs(posMinPWM)*pwmTicks > switchTicksMax;
stictionValsMax = abs(posMaxPWM)*pwmTicks > switchTicksMax;
stictionVals =  stictionValsMin & stictionValsMax;

switchTicksMean = mean(tickDifferences(switchVals))/2*pwmTicks;
switchTicksStd = std(tickDifferences(switchVals))/2*pwmTicks;
switchTicksMin = min(tickDifferences(switchVals))/2*pwmTicks;
stictionTicksMax = max(tickDifferences(stictionVals))/2*pwmTicks;
stictionTicksMean = mean(tickDifferences(stictionVals))/2*pwmTicks;
stictionTicksStd = std(tickDifferences(stictionVals))/2*pwmTicks;

figure(4);
plot(dataPosFw.posCtrl.motorData(2,:),dataPosFw.posCtrl.motorData(1,:),dataPosBw.posCtrl.motorData(2,:),dataPosBw.posCtrl.motorData(1,:))