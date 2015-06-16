classdef BatteryMonitor < handle
    
%  type | sub | direction    | value
%  -------------------------------------------------------------
%  X    |  0   | 0            | volts_raw     (int32)
%  X    |  1   | 0            | amps_raw      (int32)
%  X    |  2   | 0            | volts         (float)
%  X    |  3   | 0            | amps          (float)
%  X    |  4   | 0            | watts         (float)
%  X    |  5   | 0            | joules        (float)
%  X    |  6   | 0            | volts_filt    (float)
%  X    |  7   | 0            | amps_filt     (float)
%  X    |  8   | 0            | watts_filt    (float)
%  X    |  9   | 0            | joules_filt   (float) // unused
%  X    | 10   | uint8 access | volts_bias    (float) // unused
%  X    | 11   | uint8 access | volts_gain    (float)
%  X    | 12   | uint8 access | amps_bias     (float)
%  X    | 13   | uint8 access | amps_gain     (float)
%  X    | 14   | 1            | true_voltage  (float)   // single point voltage calibration
%  X    | 15   | 1            | N/A                     // reset joule counter to zero
%  'access' is 0-get, 1-set, 2-save, 3-reply
    
properties (SetAccess = private)
    % embedded measurements
    time
    volts_raw
    volts
    volts_filt
    amps_raw
    amps
    amps_filt
    watts
    watts_filt
    joules

    % embedded parameters
    volts_gain
    amps_bias
    amps_gain
end

properties (SetAccess = private, Hidden)
    com
    made_MessageInterface
    message_timeout
end

properties (Dependent)

end    

properties (Hidden)
    verbose % verbose mode
end

properties (SetAccess = private)

end

properties (Constant, Hidden)
    
    % Access
    kGet    = 0;
    kSet    = 1;
    kSave   = 2;
    kReply  = 3;
    
    % Type
    kTypeBatteryMonitor      = 40;
    kTypeBatteryMonitorState = 41;
    kTypeRequestMessage      =  3;

    % Subtype
    kSubtypeVoltsRaw    =  0;
    kSubtypeAmpsRaw     =  1;
    kSubtypeVolts       =  2;
    kSubtypeAmps        =  3;
    kSubtypeWatts       =  4;
    kSubtypeJoules      =  5;
    kSubtypeVoltsFilt   =  6;
    kSubtypeAmpsFilt    =  7;
    kSubtypeWattsFilt   =  8;
%     kSubtypeJoulesFilt  = 9;
%     kSubtypeVoltsBias   = 10;
    kSubtypeVoltsGain   = 11;
    kSubtypeAmpsBias    = 12;
    kSubtypeAmpsGain    = 13;

    % specs for standard one-value messages
    kSpecGet = struct('Type', BatteryMonitor.kTypeBatteryMonitor,...
        'Fields', struct('sub','uint8','access','uint8'));
    kSpecSetSingle = struct('Type', BatteryMonitor.kTypeBatteryMonitor,...
        'Fields', struct('sub','uint8','access','uint8','value','single'));
    kSpecSave = struct('Type', BatteryMonitor.kTypeBatteryMonitor,...
        'Fields', struct('sub','uint8','access','uint8'));
    kSpecReplySingle = struct('Type', BatteryMonitor.kTypeBatteryMonitor,...
        'Fields', struct('sub','uint8','access','uint8','value','single'));
    kSpecReplyInt32 = struct('Type', BatteryMonitor.kTypeBatteryMonitor,...
        'Fields', struct('sub','uint8','access','uint8','value','int32'));

    % specs for requesting / parsing custom messages with multiple values
    kSpecRequestMessage = struct('Type', BatteryMonitor.kTypeRequestMessage,...
        'Fields', struct('type', 'uint8'));
    kSpecBatteryMonitorState = struct('Type', BatteryMonitor.kTypeBatteryMonitorState,...
        'Fields', struct('time','single','volts','single','amps','single', 'watts', 'single', 'joules', 'single'));
end




methods
    function batt = BatteryMonitor(varargin) % constructor
% explicily create the MessageInterface and then pass it in (so it can be
% shared by both the battery and the motor objects.)
%       com = MessageInterface('COM18', 115200);
%       batt = BatteryMonitor('com', com);
%       motor = BrushlessController('com', com);        
        try
            com        = getArgVal('com',   varargin, []);
        catch err
            if strcmp(err.identifier,'MATLAB:UndefinedFunction')
                error('Cannot find getArgVal in the path.  It might be at http://svn.modlabupenn.org/libraries/Matlab_Utilities (rev 67). Find the directory it is in and use addpath() to make it visible.')
            end
        end

        batt.message_timeout = 0.25;
        batt.verbose = getArgVal('verbose', varargin, false);

        if isempty(com)
            com = input(['The BrushlessController class requires a MessageInterface.' char(10) 'Please enter the com port used to talk to the module: '],'s');
        end

        if ischar(com)
            batt.com = MessageInterface(com,460800); %115200
            batt.made_MessageInterface = true;
        elseif isa(com, 'MessageInterface')
            batt.com = com;
            batt.made_MessageInterface = false;
        else
            batt.made_MessageInterface = false;
            error('No communication monitor properly defined.');
        end

        out(batt.verbose, 'Finished initializing battery monitor properties.');
    end % battery monitor constructor

    function delete(batt) %battery monitor destructor
       if batt.made_MessageInterface
           batt.com.delete();
       end
    end %battery monitor destructor        


    
    
    %% Setters
    
    function set_single(batt, sub, val)
    % set_single(batt, sub, val) sends a parameter set message with type
    % kTypeBatteryMonitor, subtype sub, and value val formated as 'single'.
        msg.sub = sub;
        msg.access = BatteryMonitor.kSet;
        msg.value = val;
        batt.com.SendMsg(batt.kSpecSetSingle, msg);
    end
    
    function set_volts_gain(batt, val)
        batt.set_single(BatteryMonitor.kSubtypeVoltsGain, val)
    end
    
    function set_amps_bias(batt, val)
        batt.set_single(BatteryMonitor.kSubtypeAmpsBias, val)
    end
    
    function set_amps_gain(batt, val)
        batt.set_single(BatteryMonitor.kSubtypeAmpsGain, val)
    end
    
    %% Getters
    
    function data = get_and_wait(batt, sub)
    % get_and_wait(batt, sub) sends a parameter get message with type
    % kTypeBatteryMonitor and subtype sub.  It then waits for a
    % corresponding reply and returns the received value.
        msg.sub = sub;
        msg.access = BatteryMonitor.kGet;
        batt.com.SendMsg(batt.kSpecGet, msg);
        data = [];
        if batt.WaitForSubtype(sub) % if we got a reply
            switch(sub)
                case BatteryMonitor.kSubtypeVoltsRaw
                    data = batt.volts_raw;
                case BatteryMonitor.kSubtypeVolts
                    data = batt.volts;
                case BatteryMonitor.kSubtypeVoltsFilt
                    data = batt.volts_filt;
                case BatteryMonitor.kSubtypeAmpsRaw
                    data = batt.amps_raw;
                case BatteryMonitor.kSubtypeAmps
                    data = batt.amps;
                case BatteryMonitor.kSubtypeAmpsFilt
                    data = batt.amps_filt;
                case BatteryMonitor.kSubtypeWatts
                    data = batt.watts;
                case BatteryMonitor.kSubtypeWattsFilt
                    data = batt.watts_filt;
                case BatteryMonitor.kSubtypeJoules
                    data = batt.joules;

                case BatteryMonitor.kSubtypeVoltsGain
                    data = batt.volts_gain;
                case BatteryMonitor.kSubtypeAmpsBias
                    data = batt.amps_bias;
                case BatteryMonitor.kSubtypeAmpsGain
                    data = batt.amps_gain;
            end
        end
    end
    
    function data = get_volts_raw(batt)
        data = batt.get_and_wait(BatteryMonitor.kSubtypeVoltsRaw);
    end

    function data = get_volts(batt)
        data = batt.get_and_wait(BatteryMonitor.kSubtypeVolts);
    end

    function data = get_volts_filt(batt)
        data = batt.get_and_wait(BatteryMonitor.kSubtypeVoltsFilt);
    end

    function data = get_amps_raw(batt)
        data = batt.get_and_wait(BatteryMonitor.kSubtypeAmpsRaw);
    end

    function data = get_amps(batt)
        data = batt.get_and_wait(BatteryMonitor.kSubtypeAmps);
    end

    function data = get_amps_filt(batt)
        data = batt.get_and_wait(BatteryMonitor.kSubtypeAmpsFilt);
    end

    function data = get_watts(batt)
        data = batt.get_and_wait(BatteryMonitor.kSubtypeWatts);
    end

    function data = get_watts_filt(batt)
        data = batt.get_and_wait(BatteryMonitor.kSubtypeWattsFilt);
    end

    function data = get_joules(batt)
        data = batt.get_and_wait(BatteryMonitor.kSubtypeJoules);
    end
    
    function data = get_volts_gain(batt)
        data = batt.get_and_wait(BatteryMonitor.kSubtypeVoltsGain);
    end

    function data = get_amps_bias(batt)
        data = batt.get_and_wait(BatteryMonitor.kSubtypeAmpsBias);
    end
    
    function data = get_amps_gain(batt)
        data = batt.get_and_wait(BatteryMonitor.kSubtypeAmpsGain);
    end
    
    %% Special Getters
    
    function raw = get_volts_raw_mean(batt)
        raw = [];
        for k=1:1000; raw = [raw, double(batt.get_volts_raw())]; end;
        raw = mean(raw);
    end
    
    function raw = get_amps_raw_mean(batt)
        raw = [];
        for k=1:1000; raw = [raw, double(batt.get_amps_raw())]; end;
        raw = mean(raw);
    end
    
    %% Savers 
    
    function save(batt, sub)
    % save(batt, sub) sends a parameter save message of type
    % kTypeBatteryMonitor with subtype sub.
        msg.sub = sub;
        msg.access = BatteryMonitor.kSave;
        batt.com.SendMsg(batt.kSpecSave, msg);
    end
    
    function save_volts_gain(batt)
        batt.save(BatteryMonitor.kSubtypeVoltsGain);
    end
    
    function save_amps_bias(batt)
        batt.save(BatteryMonitor.kSubtypeAmpsBias);
    end
    
    function save_amps_gain(batt)
        batt.save(BatteryMonitor.kSubtypeAmpsGain);
    end
    

    %% Message Request
    
    function get_state(batt)
        msg.type = BatteryMonitor.kTypeBatteryMonitorState;
        batt.com.SendMsg(batt.kSpecRequestMessage, msg);
        WaitForType(batt, BatteryMonitor.kTypeBatteryMonitorState);
    end


    %% Parsing

    function success = WaitForSubtype(batt, sub)
    % parse messages until type kTypeBatteryMonitor with target subtype is found
    % return 1 if message found, else 0 on timeout
        type = BatteryMonitor.kTypeBatteryMonitor;
        com = batt.com;
        end_time = toc + batt.message_timeout; % tic in MessageInterface
        while(toc < end_time)
            com.GetBytes();
            [rx_type, rx_pkt] = com.PeekPacket();               % check for messages
            if ~isempty(rx_type)                                % if we got a message
                rx_sub = batt.ParseMessage(rx_type, rx_pkt);	% parse that message
                if rx_type == type && rx_sub == sub             % check type/subtype
                    success = true;
                    return;
                end
            end
        end
        success = false;
    end

    function success = WaitForType(batt, type)
    % parse messages until target type is found
    % return 1 if message found, else 0 on timeout
        com = batt.com;
        end_time = toc + batt.message_timeout; % tic in MessageInterface
        while(toc < end_time)
            com.GetBytes();
            [rx_type, rx_pkt] = com.PeekPacket();       % check for messages
            if ~isempty(rx_type)                        % if we got a message
                batt.ParseMessage(rx_type, rx_pkt);     % parse that message
                if rx_type == type                      % check type
                    success = true;
                    return;
                end
            end
        end
        success = false;
    end
    
    function val = UnpackSingleReply(batt, data)
        val = [];
        msg = UnpackMsg(batt.kSpecReplySingle, data);
        if msg.access == BatteryMonitor.kReply
            val = msg.value;
        end
    end
    
    function val = UnpackInt32Reply(batt, data)
        val = [];
        msg = UnpackMsg(batt.kSpecReplyInt32, data);
        if msg.access == BatteryMonitor.kReply
            val = msg.value;
        end
    end

    function sub = ParseMessage(batt, type, data)
    % ParseMessage(batt, type, data) inteprets message data according to
    % type and stores the result in batt member variables
        sub = [];
        try
            if type == batt.kTypeBatteryMonitor
                sub = data(1);
                switch(sub)
                    case BatteryMonitor.kSubtypeVoltsRaw
                        batt.volts_raw = batt.UnpackInt32Reply(data);
                    case BatteryMonitor.kSubtypeVolts
                        batt.volts = batt.UnpackSingleReply(data);
                    case BatteryMonitor.kSubtypeVoltsFilt
                        batt.volts_filt = batt.UnpackSingleReply(data);
                    case BatteryMonitor.kSubtypeAmpsRaw
                        batt.amps_raw = batt.UnpackInt32Reply(data);
                    case BatteryMonitor.kSubtypeAmps
                        batt.amps = batt.UnpackSingleReply(data);
                    case BatteryMonitor.kSubtypeAmpsFilt
                        batt.amps_filt = batt.UnpackSingleReply(data);
                    case BatteryMonitor.kSubtypeWatts
                        batt.watts = batt.UnpackSingleReply(data);
                    case BatteryMonitor.kSubtypeWattsFilt
                        batt.watts_filt = batt.UnpackSingleReply(data);
                    case BatteryMonitor.kSubtypeJoules
                        batt.joules = batt.UnpackSingleReply(data);
                        
                    case BatteryMonitor.kSubtypeVoltsGain
                        batt.volts_gain = batt.UnpackSingleReply(data);
                    case BatteryMonitor.kSubtypeAmpsBias
                        batt.amps_bias = batt.UnpackSingleReply(data);
                    case BatteryMonitor.kSubtypeAmpsGain
                        batt.amps_gain = batt.UnpackSingleReply(data);
                end
            elseif type == batt.kTypeBatteryMonitorState
                msg = UnpackMsg(batt.kSpecBatteryMonitorState, data);
                batt.time       = toc;
                batt.volts_filt = msg.volts;
                batt.amps_filt  = msg.amps;
                batt.watts_filt = msg.watts;
                batt.joules     = msg.joules;
            else
                sub = [];
            end
        catch err
            warning('ParseMessage error.')
            rethrow(err);
        end
    end
    
%% Convenience Functions
    function [gain] = calcVoltsGain(batt, meas1, raw1)
        gain = meas1/raw1;
    end

    function [gain, bias] = calcAmpsGainAndBias(batt, meas1, raw1, meas2, raw2)
        gain = (meas2 - meas1)/(raw2 - raw1);
        bias = meas1/gain - raw1;
    end


%% Display
    function dispParams(batt)
        disp(['voltage gain:' num2str(batt.volts_gain)]);            
    end

end % methods

end