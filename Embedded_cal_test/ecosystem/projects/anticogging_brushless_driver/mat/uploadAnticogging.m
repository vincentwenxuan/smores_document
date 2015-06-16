function waveformOut = uploadAnticogging(voltageIn, varargin)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%             Motor Anticogging Uploading
% Upload Anticogging takes in data from a coggingXXXBased sample(s),
% averages them, and uploads them to the microcontroller.
%  
%  Contact: Matthew Piccoli, piccoli@seas.upenn.edu
%  Date: January 22, 2013
%
% OPTIONS:
% verbose: Boolean determines if the user receives updates via text on the console
% id: Motor id of the *MOTOR CONTROLLER BOARD*
% switchTicks: Number of PWM ticks that both FETs are turned off during switching (can be found using coggingPositionBased)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

try
verbose     = getArgVal('verbose', varargin, false); % Boolean determines if the user receives updates via text on the console
catch err
    if strcmp(err.identifier,'MATLAB:UndefinedFunction')
        error('Cannot find getArgVal in the path.  It might be at http://svn.modlabupenn.org/libraries/Matlab_Utilities (rev 67). Find the directory it is in and use addpath() to make it visible.')
    end
end
addReply     = getArgVal('addReply', varargin, []); % Boolean determines if the user receives updates via text on the console


%% Make sure we can see everything we need that is not builtin to Matlab
cog = Anticogging(varargin{:});

%% Gather necessary data from motor controller
encoderTicks = getArgVal('encoder_counts', varargin, []);
if isempty(encoderTicks)
    encoderTicks = input(['The number of encoder counts is required but is unable to ask the driver directly.' char(10) 'Please type the number of encoder counts then press Enter: ']);
end
% Check size of voltageIn
if size(voltageIn, 1) ~= encoderTicks
    warning(['Input voltage is incorrect length.  No data is written.  Expected: ' num2str(encoderTicks) ' but got: ' num2str(size(voltageIn,1))]);
    return;
end

% Get current cogSmFlag for magnitude
if isempty(addReply)
    reply = input(['Would you like to add the data or overwrite the data' char(10) 'Type A for add or anything else for overwrite, then press Enter: '], 's');
else
    reply = addReply;
end
if strcmpi(reply,'a')
    out(verbose,'Collecting current smoothing data');
    % Get current anticogging data
    waveformCurrent = cog.get_all_cog_data();
    waveformCurrent(isnan(waveformCurrent)) = 0; % all nans become zeros
else
    waveformCurrent = zeros(1,encoderTicks);
end

%% Clear the old data
out(verbose,'Erasing old data');
cog.Erase();
pause(1);

%% Generate new data
% anticogAmount*ff(theta)*pwmSp/batteryV = pwm
% ff(theta) = pwm/pwmSp*batteryV
% ff passed as Q15 in voltages as an int (16 bit), thus, max/min V is +-1
% cogSmFlag is 32 bit and scales voltages, thus voltage split between
% waveform and magnitude
waveformOut = mean(voltageIn,2)' + waveformCurrent;

% send data
out(verbose,'Uploading new data');
cog.set_cog_data_index(0:(length(waveformOut)-1),waveformOut);

%% Confirm upload
out(verbose,'Checking uploaded data');
confirmation_data = cog.get_cog_data_index(0:(length(waveformOut)-1));
correct_data = (confirmation_data == cast(waveformOut,'single'));
out(verbose,[num2str(sum(correct_data)/length(correct_data)*100) '% uploaded successfully']);
% if(min(correct_data) == 0) % had unsuccessful sends
    