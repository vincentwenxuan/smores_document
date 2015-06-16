function [trialData uploaded] = coggingMethodComparison(varargin)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% function coggingMethodComparison(varargin)
%
% coggingMethodComparison performs various cog measuring techniques and
% plots the results.  This function will spin the motor in both directions.
% If data is passed in using the trialData option, the tests that it
% contains will not be run.  Tests that are missing from trialData will be
% run on the hardware.  
%
% trialData returned is the raw data
% switchTicks is the calculated switchTicks
% vAc is the anticogging voltage waveform written to the microconroller
%
% OPTIONS:
% verbose: Boolean determines if the user receives updates via text on the console
% trialData: Data from a previous coggingMethodComparison test
% performPosition: Boolean determines if coggingPositionBased is performed
% performSpeed: Boolean determines if coggingSpeedBased is performed
% performForce: Boolean determines if coggingForceBased is performed
% (automatically sets performSpeed true as it's done for free with force)
% switchTicks: Number of PWM ticks that both FETs are turned off during switching (can be found using coggingPositionBased)
% R: Motor resistance in ohms
% Kv: Motor speed constant in RPM/V
% RPM: Motor speed in RPM.  RPM can be a scalar or vector of equal length
% to idealV
% name: Optional name to label this trial
% outDir: Destination directory for the save data
% nonstop: Boolean determines if user is promped (to execute, save, etc.)
% id: Motor id of the *MOTOR CONTROLLER BOARD*
% startPos: If sensing less then one revolution, specifies starting point.  Requires endPos.  startPos == endPos yelds no limit.
% endPos: If sensing less then one revolution, specifies ending point.  Requires startPos.  startPos == endPos yelds no limit.
% note: Optional note to store in json results under 'note' key
% kp: Temporary P gain for the position PID
% ki: Temporary I gain for the position PID
% kd: Temporary D gain for the position PID
% kc: Temporary I clamp for the position PID
% tTrial: [s] - time length of speed/force trial
% pwm: PWM command on top of minimum rolling pwm (-1.0 to 1.0) (for tests at higher speeds)
% CVTest: Boolean that determines whether the constant voltage test is performed
% RDTest: Boolean that determines whether the ramp down test is performed
% outPath: Path for saved data

try
verbose     = getArgVal('verbose', varargin, false); % Boolean determines if the user receives updates via text on the console
catch err
    if strcmp(err.identifier,'MATLAB:UndefinedFunction')
        error('Cannot find getArgVal in the path.  It might be at http://svn.modlabupenn.org/libraries/Matlab_Utilities (rev 67). Find the directory it is in and use addpath() to make it visible.')
    end
end
nonstop            = getArgVal('nonstop',   varargin, false);
name            = getArgVal('name',   varargin, date());
performPosition	= getArgVal('performPosition', varargin, false); % Boolean determines if coggingPositionBased is performed
performForce    = getArgVal('performForce', varargin, false); % Boolean determines if coggingPositionBased is performed
performSpeed    = (performForce || getArgVal('performSpeed', varargin, false)); % Boolean determines if coggingPositionBased is performed
trialData       = getArgVal('trialData', varargin, []); % Data from a previous coggingMethodComparison test
outPathRoot     = getArgVal('outDir', varargin, './coggingMethodComparisonData/');
writeReply      = getArgVal('writeReply',varargin, []);

outPath = strcat(outPathRoot, name, '_', regexprep(num2str(clock()), '\s*', '-'), '.mat'); % Matlab strings are dark comedy.

numVarycolor = 0;
newTestsPerformed = false;
switchTicks = [];
daqPP = [];
posRMSE = [];
accRMSE = [];

% If the input data comes in as a string instead of a file, try opening
% that file
if ischar(trialData)
    try
        out(verbose,'Loading data from trialData file');
        loadedTrialData = load(trialData);
        trialData = loadedTrialData.trialData;
    catch dataLoadException
        out(verbose, ['Exception caught while trying to load trialData string:' char(10) dataLoadException.message char(10) 'Executing tests on the motor']);
    end
end

%% Make sure we can see everything we need that is not builtin to Matlab
if performPosition
    pathQuery('coggingPositionBased', 'http://svn.modlabupenn.org/personal/piccoli/Brushless_Module/Software/Anti_Cogging/Position_Based');
end
if performForce
    pathQuery('coggingForceBased', 'http://svn.modlabupenn.org/personal/piccoli/Brushless_Module/Software/Anti_Cogging/Force_Based');
end
if performSpeed && ~performForce
    pathQuery('coggingSpeedBased', 'http://svn.modlabupenn.org/personal/piccoli/Brushless_Module/Software/Anti_Cogging/Speed_Based');
end
pathQuery('idealV2Torque', 'http://svn.modlabupenn.org/personal/piccoli/Brushless_Module/Software/Anti_Cogging/Conversions');

%% Begin Position Tests
if performPosition
    out(verbose, 'Starting Position Tests');
    if ~isstruct(trialData) || ~isfield(trialData,'dataPosFw')
        trialData.dataPosFw = coggingPositionBased('direction', 1, varargin{:});
        newTestsPerformed = true;
    end
    if ~isstruct(trialData) || ~isfield(trialData,'dataPosBw')
        trialData.dataPosBw = coggingPositionBased('direction', -1, varargin{:});
        newTestsPerformed = true;
    end
    out(verbose, 'Finished Position Tests');
    
    out(verbose, 'Starting Finding Switch Ticks');
    [switchTicksMax switchTicksMean switchTicksStd stictionTicksMax stictionTicksMean stictionTicksStd] = findSwitchTicks(trialData.dataPosFw, trialData.dataPosBw, varargin)
%     switchTicks = switchTicksMean
    switchTicks = switchTicksMax
    if isnan(switchTicks)
        warning('switchTicks is nan!  Setting to 0');
        switchTicks = 0;
    end
    varargin = [varargin {'switchTicks' switchTicks}];
    out(verbose, 'Finished Finding Switch Ticks');
    
    out(verbose, 'Starting Position Applied Voltage Conversion to Ideal Voltage');
    [idealVPosFitFw xPosFw] = coggingPositionBased2IdealVFit(trialData.dataPosFw, varargin{:});
    [idealVPosFitBw xPosBw] = coggingPositionBased2IdealVFit(trialData.dataPosBw, varargin{:});
    out(verbose, 'Finished Position Applied Voltage Conversion to Ideal Voltage');
    
    out(verbose, 'Starting Position Ideal Voltage Conversion to Torque');
    torquePosFitFw = idealV2Torque(idealVPosFitFw, varargin{:});
    torquePosFitBw = idealV2Torque(idealVPosFitBw, varargin{:});
    meanTorquePosFitFw = mean(torquePosFitFw);
    meanTorquePosFitBw = mean(torquePosFitBw);
    out(verbose, 'Finished Position Ideal Voltage Conversion to Torque');
    
    numVarycolor = numVarycolor + 2;
end

%% Begin Speed and (maybe) Force Based
if performForce
    out(verbose, 'Starting Force Tests');
    if ~isstruct(trialData) || ~isfield(trialData,'dataSpeedFw')
        trialData.dataSpeedFw = coggingForceBased('direction', 1, varargin{:});
        newTestsPerformed = true;
    end
    if ~isstruct(trialData) || ~isfield(trialData,'dataSpeedBw')
        trialData.dataSpeedBw = coggingForceBased('direction', -1, varargin{:});
        newTestsPerformed = true;
    end
    out(verbose, 'Finished Force Tests');
    
    out(verbose, 'Starting Acceleration Conversion to Ideal Voltage');
    [idealVAccFitFw xAccFw] = coggingSpeedBased2IdealVFit(trialData.dataSpeedFw, varargin{:});
    [idealVAccFitBw xAccBw] = coggingSpeedBased2IdealVFit(trialData.dataSpeedBw, varargin{:});
    out(verbose, 'Finished Acceleration Conversion to Ideal Voltage');
    
    out(verbose, 'Starting Acceleration Ideal Voltage Conversion to Torque');
    torqueAccFitFw = idealV2Torque(idealVAccFitFw, varargin{:});
    torqueAccFitBw = idealV2Torque(idealVAccFitBw, varargin{:});
    out(verbose, 'Finished Acceleration Ideal Voltage Conversion to Torque');
    
    out(verbose, 'Starting Sensed Torque Conversion to Fitted Torque');
    [torqueForceFitFw xForceFw] = coggingForceBased2TorqueFit(trialData.dataSpeedFw, varargin{:});
    [torqueForceFitBw xForceBw] = coggingForceBased2TorqueFit(trialData.dataSpeedBw, varargin{:});
    meanTorqueAccFitFw = mean(torqueAccFitFw);
    meanTorqueAccFitBw = mean(torqueAccFitBw);
    meanTorqueForceFitFw = mean(torqueForceFitFw);
    meanTorqueForceFitBw = mean(torqueForceFitBw);
    out(verbose, 'Finished Sensed Torque Conversion to Fitted Torque');
    
    daqPP = max(mean([torqueForceFitFw, torqueForceFitBw],2)) - min(mean([torqueForceFitFw, torqueForceFitBw],2))

    accRMSE = sqrt(mean((mean([torqueAccFitFw, torqueAccFitBw],2)-mean([torqueForceFitFw, torqueForceFitBw],2)).^2));
    if performPosition
        posRMSE = sqrt(mean((mean([torquePosFitFw, torquePosFitBw],2)-mean([torqueForceFitFw, torqueForceFitBw],2)).^2));
    end
    
    numVarycolor = numVarycolor + 4;
elseif performSpeed
    out(verbose, 'Starting Speed Tests');
    if ~isstruct(trialData) || ~isfield(trialData,'dataSpeedFw')
        trialData.dataSpeedFw = coggingSpeedBased('direction', 1, varargin{:});
        newTestsPerformed = true;
    end
    if ~isstruct(trialData) || ~isfield(trialData,'dataSpeedBw')
        trialData.dataSpeedBw = coggingSpeedBased('direction', -1, varargin{:});
        newTestsPerformed = true;
    end
    out(verbose, 'Finished Speed Tests');
    
    out(verbose, 'Starting Acceleration Conversion to Ideal Voltage');
    [idealVAccFitFw xAccFw] = coggingSpeedBased2IdealVFit(trialData.dataSpeedFw, varargin{:});
    [idealVAccFitBw xAccBw] = coggingSpeedBased2IdealVFit(trialData.dataSpeedBw, varargin{:});
    out(verbose, 'Finished Acceleration Conversion to Ideal Voltage');
    
    out(verbose, 'Starting Acceleration Ideal Voltage Conversion to Torque');
    torqueAccFitFw = idealV2Torque(idealVAccFitFw, varargin{:});
    torqueAccFitBw = idealV2Torque(idealVAccFitBw, varargin{:});
    meanTorqueAccFitFw = mean(torqueAccFitFw);
    meanTorqueAccFitBw = mean(torqueAccFitBw);
    out(verbose, 'Finished Acceleration Ideal Voltage Conversion to Torque');
    
    numVarycolor = numVarycolor + 2;
end

%% Plot results
figure(1);
clf;
hold on;
colorSet = varycolor(numVarycolor);
plotNum = 1;
if performPosition
    plot(xPosFw,torquePosFitFw - meanTorquePosFitFw,'color',colorSet(plotNum,:));
    legendStrings{plotNum} = 'Pos Fw';
    plotNum = plotNum + 1;
    plot(xPosBw,torquePosFitBw - meanTorquePosFitBw,'color',colorSet(plotNum,:));
    legendStrings{plotNum} = 'Pos Bw';
    plotNum = plotNum + 1;
end
if performSpeed
    plot(xAccFw,torqueAccFitFw - meanTorqueAccFitFw,'color',colorSet(plotNum,:));
    legendStrings{plotNum} = 'Acc Fw';
    plotNum = plotNum + 1;
    plot(xAccBw,torqueAccFitBw - meanTorqueAccFitBw,'color',colorSet(plotNum,:));
    legendStrings{plotNum} = 'Acc Bw';
    plotNum = plotNum + 1;
end
if performForce
    plot(xForceFw,torqueForceFitFw - meanTorqueForceFitFw,'color',colorSet(plotNum,:));
    legendStrings{plotNum} = 'DAQ Fw';
    plotNum = plotNum + 1;
    plot(xForceBw,torqueForceFitBw - meanTorqueForceFitBw,'color',colorSet(plotNum,:));
    legendStrings{plotNum} = 'DAQ Bw';
%     indVarycolor = indVarycolor + 1;
end
title('Various Torque Ripple Sensing Methods Vs. Position');
xlabel('Position (degrees)');
ylabel('Torque (Nm)');
set(gca, 'xlim', [0 2*pi]);
ylimit = max(abs(get(gca, 'ylim')));
set(gca, 'ylim', [-ylimit ylimit]);
grid on;
legend(legendStrings{:});
set(gcf,'Color','white');

figure(2);
clf;
hold on;
colorSet = varycolor(numVarycolor/2);
plotNum = 1;
legendStrings = {};
if performPosition
    plot(xPosFw,mean([torquePosFitFw - meanTorquePosFitFw, torquePosFitBw - meanTorquePosFitBw],2),'color',colorSet(plotNum,:));
    posSTD = std(mean([torquePosFitFw - meanTorquePosFitFw, torquePosFitBw - meanTorquePosFitBw],2))
    legendStrings{plotNum} = 'Pos';
    plotNum = plotNum + 1;
end
if performSpeed
    plot(xAccFw,mean([torqueAccFitFw - meanTorqueAccFitFw, torqueAccFitBw - meanTorqueAccFitBw],2),'color',colorSet(plotNum,:));
    legendStrings{plotNum} = 'Acc';
    plotNum = plotNum + 1;
end
if performForce
    plot(xForceFw,mean([torqueForceFitFw - meanTorqueForceFitFw, torqueForceFitBw - meanTorqueForceFitBw],2),'color',colorSet(plotNum,:));
    legendStrings{plotNum} = 'DAQ';
    plotNum = plotNum + 1;
end
title('Various Torque Ripple Sensing Methods Vs. Position');
xlabel('Position (degrees)');
ylabel('Torque (Nm)');
set(gca, 'xlim', [0 2*pi]);
ylimit = max(abs(get(gca, 'ylim')));
set(gca, 'ylim', [-ylimit ylimit]);
grid on;
legend(legendStrings{:});
set(gcf,'Color','white');
% 
% figure(3);
% clf;
% hold on;
% colorSet = varycolor(numVarycolor);
% plotNum = 1;
% legendStrings = {};
% if performPosition
%     plot(xPosFw,mean([idealVPosFitFw, idealVPosFitBw],2),'color',colorSet(plotNum,:));
%     legendStrings{plotNum} = 'Pos';
%     plotNum = plotNum + 1;
%     plot(xPosFw,floor(mean([idealVPosFitFw, idealVPosFitBw],2)*300/5)/300*5,'color',colorSet(plotNum,:));
%     legendStrings{plotNum} = 'Pos Descrete';
%     plotNum = plotNum + 1;
% end
% if performSpeed
%     plot(xAccFw,mean([idealVAccFitFw, idealVAccFitBw],2),'color',colorSet(plotNum,:));
%     legendStrings{plotNum} = 'Acc';
%     plotNum = plotNum + 1;
%     plot(xAccFw,floor(mean([idealVAccFitFw, idealVAccFitBw],2)*1000)/1000,'color',colorSet(plotNum,:));
%     legendStrings{plotNum} = 'Acc Descrete';
%     plotNum = plotNum + 1;
% end
% title('Various Torque Ripple Sensing Methods Vs. Position');
% xlabel('Position (degrees)');
% ylabel('Ideal Voltage (V)');
% set(gca, 'xlim', [0 2*pi]);
% ylimit = max(abs(get(gca, 'ylim')));
% set(gca, 'ylim', [-ylimit ylimit]);
% grid on;
% legend(legendStrings{:});
% set(gcf,'Color','white');

if performPosition
    figure(4);
    clf;
    hold on;
    plot(trialData.dataPosFw.posCtrl.motorData(2,:),trialData.dataPosFw.posCtrl.motorData(1,:),'.',trialData.dataPosBw.posCtrl.motorData(2,:),trialData.dataPosBw.posCtrl.motorData(1,:),'.')
    title('V_{cg} Vs. Position');
    xlabel('Position (radians)');
    ylabel('Voltage (V)');
    set(gca, 'xlim', [0 2*pi]);
    ylimit = max(abs(get(gca, 'ylim')));
    set(gca, 'ylim', [-ylimit ylimit]);
    grid on;
    legend('Forward', 'Backward');
    set(gcf,'Color','white');
    title('Position Raw Data')
end

if performSpeed
    figure(5);
    clf;
    hold all;
    encoderTicks = getArgVal('encoder_counts', varargin, []);
    if isempty(encoderTicks)
        encoderTicks = input(['The number of encoder counts is required but is unable to ask the driver directly.' char(10) 'Please type the number of encoder counts then press Enter: ']);
    end
    x = linspace(0,2*pi-2*pi/encoderTicks,encoderTicks);
    plot(x,idealVAccFitFw,x,idealVAccFitBw)
    title('V_{cg} Vs. Position');
    xlabel('Position (radians)');
    ylabel('Voltage (V)');
    set(gca, 'xlim', [0 2*pi]);
    ylimit = max(abs(get(gca, 'ylim')));
    set(gca, 'ylim', [-ylimit ylimit]);
    grid on;
    legend('Forward', 'Backward');
    set(gcf,'Color','white');
    title('Speed Fit Data');
end

%% Save data?
if newTestsPerformed && ~nonstop
    beep;
    disp(['Outputting all data in .mat format:' char(10) outPath]);
    reply = input('Is this alright?  Y/N [N]: ', 's');
    if isempty(reply)
        reply = 'N';
    end
    if strcmpi(reply,'Y') % Save data
        save(outPath,'trialData');
        out(verbose,'Data written');
    else
        out(verbose,'No data written');
    end
end

%% Write data?
vAc = [];
uploaded = [];

if ~nonstop
    if isempty(writeReply)
        reply = input(['Would you like to write the data to the microcontroller?' char(10) 'Type P for position data, S for speed data, T for torque sensor, other for none then press Enter' char(10) 'Combinations are accepted (ex. PS Enter)']', 's');
    else
        reply = writeReply;
    end
    if (~isempty(strfind(reply,'p')) || ~isempty(strfind(reply,'P'))) && performPosition
        vAc = [vAc, idealVPosFitFw, idealVPosFitBw];
    end
    if (~isempty(strfind(reply,'s')) || ~isempty(strfind(reply,'S'))) && performSpeed
        vAc = [vAc, idealVAccFitFw, idealVAccFitBw];
    end
    if (~isempty(strfind(reply,'t')) || ~isempty(strfind(reply,'T'))) && performForce
        vAc = [vAc, torque2IdealV(torqueForceFitFw,varargin{:},'RPM',mean(trialData.dataSpeedFw.constantVoltage.motorData(:,2))), torque2IdealV(torqueForceFitBw,varargin{:},'RPM',mean(trialData.dataSpeedBw.constantVoltage.motorData(:,2)))];
    end
    uploaded = vAc;
    if ~isempty(vAc)
        uploaded = uploadAnticogging(vAc,varargin{:});
    end
end