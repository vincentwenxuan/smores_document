classdef TestStandReporter < handle
    
%  type | sub  | direction    | value
%  -------------------------------------------------------------
%  X    |  0   | 0            | target_freq     (float)
%  X    |  2   | 0            | send_motor      (uint8)
%  X    |  3   | 0            | send_battery    (uint8)
%  X    |  4   | 0            | send_stand      (uint8)
%  'access' is 0-get, 1-set, 2-save, 3-reply
    
properties (SetAccess = private)
    % embedded parameters
    target_freq
    stream_enabled
    send_test_stand
end

properties (SetAccess = private, Hidden)
    com
    made_MessageInterface
    message_timeout
    
    % PC_time, uC_time, volts, amps, watts, joules, pwm, speed, position
    test_stand_log
    test_stand_log_index
end

properties (Dependent)

end    

properties (Hidden)
    verbose
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
    kTypeTestStandReporter      = 170;
    kTypeTestStand              = 174;
    kTypeRequestMessage         =   3;

    % Subtype
    kSubTargetFreq              = 0;
    kSubStreamEnabled           = 1;
    kSubSendTestStand           = 5;

    % specs for standard one-value messages
    kSpecGet = struct('Type', TestStandReporter.kTypeTestStandReporter,...
        'Fields', struct('sub','uint8','access','uint8'));
    kSpecSetSingle = struct('Type', TestStandReporter.kTypeTestStandReporter,...
        'Fields', struct('sub','uint8','access','uint8','value','single'));
    kSpecSetUint8 = struct('Type', TestStandReporter.kTypeTestStandReporter,...
        'Fields', struct('sub','uint8','access','uint8','value','uint8'));
    kSpecSave = struct('Type', TestStandReporter.kTypeTestStandReporter,...
        'Fields', struct('sub','uint8','access','uint8'));
    kSpecReplySingle = struct('Type', TestStandReporter.kTypeTestStandReporter,...
        'Fields', struct('sub','uint8','access','uint8','value','single'));
    kSpecReplyUint8 = struct('Type', TestStandReporter.kTypeTestStandReporter,...
        'Fields', struct('sub','uint8','access','uint8','value','uint8'));

    % specs for requesting / parsing custom messages with multiple values
    kSpecRequestMessage = struct('Type', TestStandReporter.kTypeRequestMessage,...
        'Fields', struct('type', 'uint8'));
    kSpecTestStand = struct('Type', TestStandReporter.kTypeTestStand,...
        'Fields', struct('time','single',...
        'volts','single','amps','single', 'watts', 'single', 'joules', 'single',...
        'pwm', 'single', 'speed', 'single', 'position', 'single'));
end




methods
    function stand = TestStandReporter(varargin) % constructor
    % explicitly create the MessageInterface and then pass it in (so it can be
    % shared by both the battery and the motor objects.)
    %       com = MessageInterface('COM18', 115200);
    %       stand = TestStandReporter('com', com);
        try
            com        = getArgVal('com',   varargin, []);
        catch err
            if strcmp(err.identifier,'MATLAB:UndefinedFunction')
                error('Cannot find getArgVal in the path.  It might be at http://svn.modlabupenn.org/libraries/Matlab_Utilities (rev 67). Find the directory it is in and use addpath() to make it visible.')
            end
        end

        stand.message_timeout = 0.25;
        stand.verbose = getArgVal('verbose', varargin, false);

        if isempty(com)
            error('The TestStandReporter class requires a MessageInterface object.');
        end
        
        stand.com = com;
        
        stand.test_stand_log = [];
        stand.test_stand_log_index = 1;
    end

    function delete(stand)
       if stand.made_MessageInterface
           stand.com.delete();
       end
    end

    function init_test_stand_log(stand, n)
        stand.test_stand_log = zeros(n, 9);
        stand.test_stand_log_index = 1;
    end
    
    function log = get_test_stand_log(stand)
        log = stand.test_stand_log(1:(stand.test_stand_log_index-1), :);
    end
    
    
    %% Setters
    
    function set_single(stand, sub, val)
    % set_single(batt, sub, val) sends a parameter set message with type
    % kTypeTestStandReporter, subtype sub, and value val formated as 'single'.
        msg.sub = sub;
        msg.access = TestStandReporter.kSet;
        msg.value = val;
        stand.com.SendMsg(stand.kSpecSetSingle, msg);
    end
    
    function set_uint8(stand, sub, val)
    % set_uint8(batt, sub, val) sends a parameter set message with type
    % kTypeTestStandReporter, subtype sub, and value val formated as 'uint8'.
        msg.sub = sub;
        msg.access = TestStandReporter.kSet;
        msg.value = val;
        stand.com.SendMsg(stand.kSpecSetUint8, msg);
    end
    
    function set_target_freq(stand, val)
        stand.set_single(TestStandReporter.kSubTargetFreq, val)
    end
   
    function set_stream_enabled(stand, val)
        stand.set_uint8(TestStandReporter.kSubStreamEnabled, val)
    end
    
    function set_send_test_stand(stand, val)
        stand.set_uint8(TestStandReporter.kSubSendTestStand, val)
    end


    %% Getters
    
    function data = get_and_wait(stand, sub)
    % get_and_wait(batt, sub) sends a parameter get message with type
    % kTypeTestStand and subtype sub.  It then waits for a
    % corresponding reply and returns the received value.
        msg.sub = sub;
        msg.access = TestStandReporter.kGet;
        stand.com.SendMsg(TestStandReporter.kSpecGet, msg);
        data = [];
        if stand.WaitForSubtype(sub) % if we got a reply
            switch(sub)
                case TestStandReporter.kSubTargetFreq
                    data = stand.target_freq;
                case TestStandReporter.kSubStreamEnabled
                    data = stand.stream_enabled;
                case TestStandReporter.kSubSendTestStand
                    data = stand.send_test_stand;
            end
        end
    end
    
    function data = get_target_freq(stand)
        data = stand.get_and_wait(TestStandReporter.kSubTargetFreq);
    end
    
    function data = get_stream_enabled(stand)
        data = stand.get_and_wait(TestStandReporter.kSubStreamEnabled);
    end
    
    function data = get_send_test_stand(stand)
        data = stand.get_and_wait(TestStandReporter.kSubSendTestStand);
    end

    
    %% Savers 
    
    function save(stand, sub)
    % save(stand, sub) sends a parameter save message of type
    % kTypeTestStandReporter with subtype sub.
        msg.sub = sub;
        msg.access = TestStandReporter.kSave;
        stand.com.SendMsg(stand.kSpecSave, msg);
    end
    
    function save_target_freq(stand)
        stand.save(TestStandReporter.kSubTargetFreq);
    end
    
    function save_stream_enabled(stand)
        stand.save(TestStandReporter.kSubStreamEnabled);
    end
    
    function save_send_test_stand(stand)
        stand.save(TestStandReporter.kSubSendTestStand);
    end
    


    %% Parsing

    function success = WaitForSubtype(stand, sub)
    % parse messages until type kTypeTestStandReporter with target subtype is found
    % return 1 if message found, else 0 on timeout
        type = TestStandReporter.kTypeTestStandReporter;
        com = stand.com;
        end_time = toc + stand.message_timeout; % tic in MessageInterface
        while(toc < end_time)
            com.GetBytes();
            [rx_type, rx_pkt] = com.PeekPacket();               % check for messages
            if ~isempty(rx_type)                                % if we got a message
                rx_sub = stand.ParseMessage(rx_type, rx_pkt);	% parse that message
                if rx_type == type && rx_sub == sub             % check type/subtype
                    success = true;
                    return;
                end
            end
        end
        success = false;
    end

    function success = WaitForType(stand, type)
    % parse messages until target type is found
    % return 1 if message found, else 0 on timeout
        com = stand.com;
        end_time = toc + stand.message_timeout; % tic in MessageInterface
        while(toc < end_time)
            com.GetBytes();
            [rx_type, rx_pkt] = com.PeekPacket();       % check for messages
            if ~isempty(rx_type)                        % if we got a message
                stand.ParseMessage(rx_type, rx_pkt);     % parse that message
                if rx_type == type                      % check type
                    success = true;
                    return;
                end
            end
        end
        success = false;
    end
    
    
    % Get bytes from serial until target sub_id is receieved once, then
    % flush out any remaining complete packets from the parser.
    function is_msg = ParseMessages(stand)
        is_msg = false;
        stand.com.GetBytes();
        while true
            [msg_type, pkt] = stand.com.PeekPacket();
            if ~isempty(msg_type)
                ParseMessage(stand, msg_type, pkt);
                is_msg = true;
            else
                return;
            end
        end
    end
    
    
    
    function val = UnpackSingleReply(stand, data)
        val = [];
        msg = UnpackMsg(stand.kSpecReplySingle, data);
        if msg.access == TestStandReporter.kReply
            val = msg.value;
        end
    end
    
    function val = UnpackUint8Reply(stand, data)
        val = [];
        msg = UnpackMsg(stand.kSpecReplyUint8, data);
        if msg.access == TestStandReporter.kReply
            val = msg.value;
        end
    end
    
    function sub = ParseMessage(stand, type, data)
    % ParseMessage(stand, type, data) inteprets message data according to
    % type and stores the result in stand member variables
        sub = [];
        try
            if type == stand.kTypeTestStandReporter
                sub = data(1);
                switch(sub)
                    case TestStandReporter.kSubTargetFreq
                        stand.target_freq = stand.UnpackSingleReply(data);
                    case TestStandReporter.kSubStreamEnabled
                        stand.stream_enabled = stand.UnpackUint8Reply(data);
                    case TestStandReporter.kSubSendTestStand
                        stand.send_test_stand = stand.UnpackUint8Reply(data);
                end
            elseif type == stand.kTypeTestStand
                msg = UnpackMsg(stand.kSpecTestStand, data);
                stand.test_stand_log(stand.test_stand_log_index, 1) = toc;
                stand.test_stand_log(stand.test_stand_log_index, 2) = msg.time;
                stand.test_stand_log(stand.test_stand_log_index, 3) = msg.volts;
                stand.test_stand_log(stand.test_stand_log_index, 4) = msg.amps;
                stand.test_stand_log(stand.test_stand_log_index, 5) = msg.watts;
                stand.test_stand_log(stand.test_stand_log_index, 6) = msg.joules;
                stand.test_stand_log(stand.test_stand_log_index, 7) = msg.pwm;
                stand.test_stand_log(stand.test_stand_log_index, 8) = msg.speed;
                stand.test_stand_log(stand.test_stand_log_index, 9) = msg.position;
                stand.test_stand_log_index = stand.test_stand_log_index + 1;
            else
                sub = [];
            end
        catch err
            warning('ParseMessage error.')
            rethrow(err);
        end
    end

end % methods

end