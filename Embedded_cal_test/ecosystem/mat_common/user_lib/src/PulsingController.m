classdef PulsingController < handle
    
%  type | sub  | direction        | value
%  -------------------------------------------------------------
%  X    |  0   | access (no save) | CmdPwmPulseAmp     (float)
%  X    |  1   | access (no save) | CmdPwmPulsePhase   (float)
%  'access' is 0-get, 1-set, 2-save, 3-reply
    
properties (SetAccess = private)

end

properties (SetAccess = private, Hidden)
    com
    made_MessageInterface
    message_timeout
    
    cmd_pwm_pulse_amp
    cmd_pwm_pulse_phase
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
    kTypePulsingController      = 28;
    kTypeRequestMessage         =  3;

    % Subtype
    kSubCmdPwmPulseAmp          = 0;
    kSubCmdPwmPulsePhase        = 1;

    % specs for standard one-value messages
    kSpecGet = struct('Type', PulsingController.kTypePulsingController,...
        'Fields', struct('sub','uint8','access','uint8'));
    kSpecSetSingle = struct('Type', PulsingController.kTypePulsingController,...
        'Fields', struct('sub','uint8','access','uint8','value','single'));
    kSpecSave = struct('Type', PulsingController.kTypePulsingController,...
        'Fields', struct('sub','uint8','access','uint8'));
    kSpecReplySingle = struct('Type', PulsingController.kTypePulsingController,...
        'Fields', struct('sub','uint8','access','uint8','value','single'));

    % specs for requesting / parsing custom messages with multiple values
    kSpecRequestMessage = struct('Type', PulsingController.kTypeRequestMessage,...
        'Fields', struct('type', 'uint8'));
end




methods
    function motor = PulsingController(varargin) % constructor
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

        motor.message_timeout = 0.25;
        motor.verbose = getArgVal('verbose', varargin, false);

        if isempty(com)
            error('The TestStandReporter class requires a MessageInterface object.');
        end
        
        motor.com = com;
    end

    function delete(motor)
       if motor.made_MessageInterface
           motor.com.delete();
       end
    end

    
    
    %% Setters
    
    function set_single(motor, sub, val)
    % set_single(batt, sub, val) sends a parameter set message with type
    % kTypeTestStandReporter, subtype sub, and value val formated as 'single'.
        msg.sub = sub;
        msg.access = PulsingController.kSet;
        msg.value = val;
        motor.com.SendMsg(motor.kSpecSetSingle, msg);
    end
    
    function set_cmd_pwm_pulse_amp(motor, val)
        motor.set_single(PulsingController.kSubCmdPwmPulseAmp, val)
    end
   
    function set_cmd_pwm_pulse_phase(motor, val)
        motor.set_single(PulsingController.kSubCmdPwmPulsePhase, val)
    end
    
    %% Setters with Verify
    function verify_cmd_pwm_pulse_amp(motor, val)
        motor.com.Flush();
        val = single(val);
        ret = [];
        while isempty(ret) || ret ~= val
            motor.set_cmd_pwm_pulse_amp(val);
            ret = motor.get_cmd_pwm_pulse_amp();
        end
    end
   
    function verify_cmd_pwm_pulse_phase(motor, val)
        motor.com.Flush();
        val = single(val);
        ret = [];
        while isempty(ret) || ret ~= val
            motor.set_cmd_pwm_pulse_phase(val);
            ret = motor.get_cmd_pwm_pulse_phase();
        end
    end


    %% Getters
    
    function data = get_and_wait(motor, sub)
    % get_and_wait(motor, sub) sends a parameter get message with type
    % kTypePulsingController and subtype sub.  It then waits for a
    % corresponding reply and returns the received value.
        msg.sub = sub;
        msg.access = PulsingController.kGet;
        motor.com.SendMsg(PulsingController.kSpecGet, msg);
        data = [];
        if motor.WaitForSubtype(sub) % if we got a reply
            switch(sub)
                case PulsingController.kSubCmdPwmPulseAmp
                    data = motor.cmd_pwm_pulse_amp;
                case PulsingController.kSubCmdPwmPulsePhase
                    data = motor.cmd_pwm_pulse_phase;
            end
        end
    end
    
    function data = get_cmd_pwm_pulse_amp(motor)
        data = motor.get_and_wait(PulsingController.kSubCmdPwmPulseAmp);
    end
    
    function data = get_cmd_pwm_pulse_phase(motor)
        data = motor.get_and_wait(PulsingController.kSubCmdPwmPulsePhase);
    end


    %% Parsing

    function success = WaitForSubtype(motor, sub)
    % parse messages until type kTypePulsingController with target subtype is found
    % return 1 if message found, else 0 on timeout
        type = PulsingController.kTypePulsingController;
        com = motor.com;
        end_time = toc + motor.message_timeout; % tic in MessageInterface
        while(toc < end_time)
            com.GetBytes();
            [rx_type, rx_pkt] = com.PeekPacket();               % check for messages
            if ~isempty(rx_type)                                % if we got a message
                rx_sub = motor.ParseMessage(rx_type, rx_pkt);	% parse that message
                if rx_type == type && rx_sub == sub             % check type/subtype
                    success = true;
                    return;
                end
            end
        end
        success = false;
    end

    function success = WaitForType(motor, type)
    % parse messages until target type is found
    % return 1 if message found, else 0 on timeout
        com = motor.com;
        end_time = toc + motor.message_timeout; % tic in MessageInterface
        while(toc < end_time)
            com.GetBytes();
            [rx_type, rx_pkt] = com.PeekPacket();       % check for messages
            if ~isempty(rx_type)                        % if we got a message
                motor.ParseMessage(rx_type, rx_pkt);     % parse that message
                if rx_type == type                      % check type
                    success = true;
                    return;
                end
            end
        end
        success = false;
    end
    
    
    function val = UnpackSingleReply(motor, data)
        val = [];
        msg = UnpackMsg(motor.kSpecReplySingle, data);
        if msg.access == PulsingController.kReply
            val = msg.value;
        end
    end
    
    function val = UnpackUint8Reply(motor, data)
        val = [];
        msg = UnpackMsg(motor.kSpecReplyUint8, data);
        if msg.access == PulsingController.kReply
            val = msg.value;
        end
    end
    
    function sub = ParseMessage(motor, type, data)
    % ParseMessage(stand, type, data) inteprets message data according to
    % type and stores the result in stand member variables
        sub = [];
        try
            if type == motor.kTypePulsingController
                sub = data(1);
                switch(sub)
                    case PulsingController.kSubCmdPwmPulseAmp
                        motor.cmd_pwm_pulse_amp = motor.UnpackSingleReply(data);
                    case PulsingController.kSubCmdPwmPulsePhase
                        motor.cmd_pwm_pulse_phase = motor.UnpackSingleReply(data);
                end
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