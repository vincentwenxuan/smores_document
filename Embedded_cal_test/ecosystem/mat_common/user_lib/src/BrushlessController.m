classdef BrushlessController < handle
    properties (SetAccess = private, Hidden)
        electric_lead_rate  % (electric rad/(electric rad/s))
        calibration_angle   % (rad)
        anticog_amplitude   % cog smoothing amplitude (unimplemented on microcontroller 2014/1/6)
        anticog_data        % cog smoothing data(unimplemented on microcontroller 2014/1/6)
        permute_wires       % permute wires so motor spins backwards
        commutation_hz
        
        speed_kp
        speed_ki
        speed_kd
        
        motor_properties
%         pole_pairs % motor number of pole pairs
%         Kv % motor torque constant
%         R % motor resistance (unimplemented on microcontroller 2014/1/6)
%         V0 % no load voltage (unimplemented on microcontroller 2014/1/6)
%         I0 % no load current (unimplemented on microcontroller 2014/1/6)
%         bemf_shape % back emf shape 0,1,2 = trapezoidal, sinusoidal, quadrature

        state
%         pwm % current applied pwm
%         position % current position
%         speed % current speed
%         current % current dc current
%         voltage % current supply voltage
        
        current_control % current control group
        position_control % position control group
        
        com % handle to this module's communicationMonitor
        
        message_timeout % timeout given to receive a message
        
        made_communication_monitor % true if this class made the communication monitor, false otherwise
    end
    
    properties (Dependent)
        pwm_cmd
        voltage_cmd
        current_cmd
        torque_cmd
        speed_cmd
        speed_pos_cmd
        position_cmd
        pwm_pulse_amplitude_cmd
        pwm_pulse_phase_cmd
        voltage_pulse_amplitude_cmd
        voltage_pulse_phase_cmd
    end    
    
    properties
        verbose % verbose mode
    end
    
    properties (SetAccess = private)
        pwm_feedback
        position_feedback
        speed_feedback
        current_feedback
        voltage_feedback
        
    end
    
    properties (Constant, Hidden)
        
        
        % Access
        kGet = 0;
        kSet = 1;
        kSave = 2;
        kReply = 3;

        % Type
        kTypeBrushlessController = 10;
        
        % Subtype
        kSubSpeedKp    =  20;
        kSubSpeedKi    =  21;
        kSubSpeedKd    =  22;
        
        % specs for standard one-value messages
        kSpecGet = struct('Type', BrushlessController.kTypeBrushlessController,...
            'Fields', struct('sub','uint8','access','uint8'));
        kSpecSetSingle = struct('Type', BrushlessController.kTypeBrushlessController,...
            'Fields', struct('sub','uint8','access','uint8','value','single'));
        kSpecSave = struct('Type', BrushlessController.kTypeBrushlessController,...
            'Fields', struct('sub','uint8','access','uint8'));
        kSpecReplySingle = struct('Type', BrushlessController.kTypeBrushlessController,...
            'Fields', struct('sub','uint8','access','uint8','value','single'));
        
        % specs for requesting / parsing custom messages with multiple values
        KTypePwmCmdSpec = struct('Type', BrushlessController.kTypeBrushlessController,...
            'Fields', struct('msg_type', 'uint8','pwm_cmd','single')); %0
        KTypeSpeedPosCmdSpec = struct('Type', BrushlessController.kTypeBrushlessController,...
            'Fields', struct('msg_type','uint8','speed_cmd','single','pos_cmd','single')); %5
        KTypeSpeedCmdSpec = struct('Type', BrushlessController.kTypeBrushlessController,...
            'Fields', struct('msg_type','uint8','speed_cmd','single')); %4
        kTypeCalCmdSpec = struct('Type', BrushlessController.kTypeBrushlessController,...
            'Fields', struct('msg_type','uint8','cal_pwm','single','cal_time','uint32')); %9
        kTypeFlashWriteSpec = struct('Type', BrushlessController.kTypeBrushlessController,...
            'Fields', struct('msg_type','uint8','write_var','uint8')); %10
        kTypeCalSpec = struct('Type', BrushlessController.kTypeBrushlessController,...
            'Fields', struct('msg_type','uint8','get_set','uint8','cal_ang','single')); %11
        kTypeElectricLeadRateSpec = struct('Type', BrushlessController.kTypeBrushlessController,...
            'Fields', struct('msg_type','uint8','get_set','uint8','electric_lead_rate','single')); %12
        kTypeMotorSpec = struct('Type',BrushlessController.kTypeBrushlessController,...
            'Fields',struct('msg_type','uint8','get_set','uint8','pole_pairs','uint16','Kv','single','R','single','I0','single','V0','single','bemf_shape','uint8')); %13
        kTypePIDSpec = struct('Type',BrushlessController.kTypeBrushlessController,...
            'Fields',struct('msg_type','uint8','get_set','uint8','Kp','single','Ki','single','Kd','single'));%15, 16, 17
        kTypePermuteSpec = struct('Type',BrushlessController.kTypeBrushlessController,...
            'Fields',struct('msg_type','uint8','get_set','uint8','permute','uint8')); %18
        kTypeStateSpec = struct('Type',BrushlessController.kTypeBrushlessController,...
            'Fields',struct('msg_type','uint8','get_set','uint8','pwm','single','position','single','speed','single','current','single','voltage','single')); %19
        kTypeRequestMsgSpec = struct('Type', BrushlessController.kTypeBrushlessController,...
            'Fields', struct('msg_type', 'uint8','get_set','uint8')); %Light weight version of the get/set messages for speed and reduced bandwidth
        kTypeCommutationHzSpec = struct('Type', BrushlessController.kTypeBrushlessController,...
            'Fields', struct('msg_type','uint8','get_set','uint8','commutation_hz','uint32')); %23
    end
    
    methods
        function mtr = BrushlessController(varargin) % constructor
            try
                com        = getArgVal('com',   varargin, []);
            catch err
                if strcmp(err.identifier,'MATLAB:UndefinedFunction')
                    error('Cannot find getArgVal in the path.  It might be at http://svn.modlabupenn.org/libraries/Matlab_Utilities (rev 67). Find the directory it is in and use addpath() to make it visible.')
                end
            end
            mtr.electric_lead_rate      = getArgVal('electric_lead_rate', varargin, .00028); % Default for E-flite Park 400 740Kv
            mtr.calibration_angle = getArgVal('calibration_angle', varargin, []);
            mtr.anticog_amplitude = getArgVal('anticog_amplitude', varargin, []);
            mtr.anticog_data = getArgVal('anticog_data', varargin, []);
            mtr.permute_wires = getArgVal('permute_wires', varargin, 1);

            mtr.motor_properties.pole_pairs = getArgVal('motor_pole_pairs', varargin, 7); % Default for E-flite Park 400 740Kv
            mtr.motor_properties.Kv = getArgVal('motor_Kv', varargin, 740); % Default for E-flite Park 400 740Kv
            mtr.motor_properties.R = getArgVal('motor_R', varargin, 0.26); % Default for E-flite Park 400 740Kv
            mtr.motor_properties.V0 = getArgVal('motor_Vo', varargin, 10); % Default for E-flite Park 400 740Kv
            mtr.motor_properties.I0 = getArgVal('motor_Io', varargin, 0.55); % Default for E-flite Park 400 740Kv
            mtr.motor_properties.bemf_shape = getArgVal('motor_bemf_shape', varargin, 0); % Default is trapezoidal
            
            temp_PID.Kp = 0;
            temp_PID.Ki = 0;
            temp_PID.Kd = 0;
            
            mtr.current_control = getArgVal('current_control', varargin, temp_PID);
            mtr.position_control = getArgVal('position_control', varargin, temp_PID);

            mtr.verbose = getArgVal('verbose', varargin, false);
            
            mtr.message_timeout = getArgVal('timeout', varargin, .25);
            
            if isempty(com)
                com = input(['The BrushlessController class requires a communication_monitor.' char(10) 'Please enter the com port used to talk to the module: '],'s');
            end
            
            if ischar(com)
                mtr.com = MessageInterface(com,460800); %115200
                mtr.made_communication_monitor = true;
            elseif isa(com, 'MessageInterface')
                mtr.com = com;
                mtr.made_communication_monitor = false;
            else
                mtr.made_communication_monitor = false;
                error('No communication monitor properly defined.');
            end
            
            out(mtr.verbose, 'Finished initializing motor controller properties.');
        end % motor constructor
        
        function delete(mtr) %motor destructor
           if mtr.made_communication_monitor
               mtr.com.delete();
           end
        end %motor destructor        
        
        %% Setters
        function set_electric_lead_rate(mtr,val)
            mtr.electric_lead_rate = val;
            msg.msg_type = 12;%lead
            msg.get_set = 1; %set
            msg.electric_lead_rate = val;
            mtr.com.SendMsg(mtr.kTypeElectricLeadRateSpec,msg);
        end
        
        function set_calibration_angle(mtr,val)
            if val < 2*pi && val > -2*pi
               mtr.calibration_angle = val;
               msg.msg_type = 11; %cal
               msg.get_set = 1; %set
               msg.cal_ang = val;
               mtr.com.SendMsg(mtr.kTypeCalSpec,msg);
            end
        end % set.calibration_angle
        
        function set_commutation_hz(mtr,val)
            if val < 72000000 && val > 0
               mtr.commutation_hz = val;
               msg.msg_type = 23; %cal
               msg.get_set = 1; %set
               msg.commutation_hz = val;
               mtr.com.SendMsg(mtr.kTypeCommutationHzSpec,msg);
            end
        end % set.commutation_hz


        function set_permute_wires(mtr,val)
            if val == 1 || val == 0
               mtr.permute_wires = val;
               msg.msg_type = 18; %permute
               msg.get_set = 1; %set
               msg.permute = val;
               mtr.com.SendMsg(mtr.kTypePermuteSpec,msg);
            end
        end % set.permute_wires
        
        function set_motor_properties(mtr,val)
            flag = false;
            field_names = fieldnames(val);
            if max(strcmp(field_names,'pole_pairs')) == 1 %if any field is pole_pairs
                if val.pole_pairs > 0 % we MUST know how many pole pairs and MUST be positive integer
                    mtr.motor_properties.pole_pairs = val.pole_pairs;
                    flag = true;
                end
            end
            if max(strcmp(field_names,'Kv')) == 1 %if any field is Kv
                if (val.Kv >= 0.0)
                    mtr.motor_properties.Kv = val.Kv;
                    flag = true;
                end
            end
            if max(strcmp(field_names,'R')) == 1 %if any field is R
                if val.R >= 0
                    mtr.motor_properties.R = val.R;
                    flag = true;
                end
            end
            if max(strcmp(field_names,'V0')) == 1 %if any field is V0
                if val.V0 >= 0
                    mtr.motor_properties.V0 = val.V0;
                    flag = true;
                end
            end
            if max(strcmp(field_names,'I0')) == 1 %if any field is I0
                if val.I0 >= 0
                    mtr.motor_properties.I0 = val.I0;
                    flag = true;
                end
            end
            if max(strcmp(field_names,'bemf_shape')) == 1 %if any field is bemf_shape
                if val.bemf_shape >= 0 && val.bemf_shape <= 2
                    mtr.motor_properties.bemf_shape = val.bemf_shape;
                    flag = true;
                end
            end
            if flag
               msg.msg_type = 13; %motor properties
               msg.get_set = 1; %set
               msg.pole_pairs = mtr.motor_properties.pole_pairs;
               msg.Kv = mtr.motor_properties.Kv;
               msg.R = mtr.motor_properties.R;
               msg.V0 = mtr.motor_properties.V0;
               msg.I0 = mtr.motor_properties.I0;
               msg.bemf_shape = mtr.motor_properties.bemf_shape;
               mtr.com.SendMsg(mtr.kTypeMotorSpec,msg);
            end
        end % set.motor_properties
        
        function set_current_control(mtr,val)
            flag = false;
            field_names = fieldnames(val);
            if max(strcmp(field_names,'Kp')) == 1 %if any field is Kp
                if val.Kp >= 0
                    mtr.current_control.Kp = val.Kp;
                    flag = true;
                end
            end
            if max(strcmp(field_names,'Ki')) == 1 %if any field is Ki
                if (val.Ki >= 0.0)
                    mtr.current_control.Ki = val.Ki;
                    flag = true;
                end
            end
            if max(strcmp(field_names,'Kd')) == 1 %if any field is Kd
                if val.Kd >= 0
                    mtr.current_control.Kd = val.Kd;
                    flag = true;
                end
            end
            if flag
               msg.msg_type = 15; %current control
               msg.get_set = 1; %set
               msg.Kp = mtr.current_control.Kp;
               msg.Ki = mtr.current_control.Ki;
               msg.Kd = mtr.current_control.Kd;
               mtr.com.SendMsg(mtr.kTypePIDSpec,msg);
            end
        end % set.current_control
        
        function set_position_control(mtr,val)
            flag = false;
            field_names = fieldnames(val);
            if max(strcmp(field_names,'Kp')) == 1 %if any field is Kp
                if val.Kp >= 0
                    mtr.position_control.Kp = val.Kp;
                    flag = true;
                end
            end
            if max(strcmp(field_names,'Ki')) == 1 %if any field is Ki
                if (val.Ki >= 0.0)
                    mtr.position_control.Ki = val.Ki;
                    flag = true;
                end
            end
            if max(strcmp(field_names,'Kd')) == 1 %if any field is Kd
                if val.Kd >= 0
                    mtr.position_control.Kd = val.Kd;
                    flag = true;
                end
            end
            if flag
               msg.msg_type = 17; %position control
               msg.get_set = 1; %set
               msg.Kp = mtr.position_control.Kp;
               msg.Ki = mtr.position_control.Ki;
               msg.Kd = mtr.position_control.Kd;
               mtr.com.SendMsg(mtr.kTypePIDSpec,msg);
            end
        end % set.position_control
        
        function set.pwm_cmd(mtr,val)
            if val >= 1
                pwm_cmd = .999999999999;
            elseif val <= -1
                pwm_cmd = -.999999999999;
            else
                pwm_cmd = val;
            end
            msg.msg_type = 0;
            msg.pwm_cmd = pwm_cmd;
            mtr.com.SendMsg(mtr.KTypePwmCmdSpec,msg);
        end
        
        function set.voltage_cmd(mtr,val)
            
        end
        
        function set.current_cmd(mtr,val)
            
        end
        
        function set.torque_cmd(mtr,val)
            
        end
        
        function set.speed_cmd(mtr,val)
            msg.msg_type = 4;
            msg.speed_cmd = val;
            mtr.com.SendMsg(mtr.KTypeSpeedCmdSpec, msg);
        end
        
        function set.speed_pos_cmd(mtr,val) % val must be struct with fields speed_cmd and pos_cmd
            msg = val;
            msg.msg_type = 5;
            mtr.com.SendMsg(mtr.KTypeSpeedPosCmdSpec,msg);
        end
        
        function set.position_cmd(mtr,val)
            msg.msg_type = 6;
            msg.pwm_cmd = val; % this is actually a pos_cmd, but the message structure is the same
            mtr.com.SendMsg(mtr.KTypePwmCmdSpec,msg);
        end
        
        function set_calibration_cmd(mtr,duty,time)
            msg.msg_type = 9;
            msg.cal_pwm = duty;
            msg.cal_time = time;
            mtr.com.SendMsg(mtr.kTypeCalCmdSpec,msg);
        end
        
        function set_single(mtr, sub, val)
        % set_single(mtr, sub, val) sends a parameter set message with type
        % kTypeBrushlessController, subtype sub, and value val formated as 'single'.
            msg.sub = sub;
            msg.access = BrushlessController.kSet;
            msg.value = val;
            mtr.com.SendMsg(mtr.kSpecSetSingle, msg);
        end
        
        function set_speed_kp(mtr, val)
            mtr.set_single(BrushlessController.kSubSpeedKp, val)
        end
        
        function set_speed_ki(mtr, val)
            mtr.set_single(BrushlessController.kSubSpeedKi, val)
        end
        
        function set_speed_kd(mtr, val)
            mtr.set_single(BrushlessController.kSubSpeedKd, val)
        end
        
        %% Getters
        
        function data = get_and_wait(mtr, sub)
        % get_and_wait(mtr, sub) sends a parameter get message with type
        % kTypeBrushlessController and subtype sub.  It then waits for a
        % corresponding reply and returns the received value.
            msg.sub = sub;
            msg.access = BrushlessController.kGet;
            mtr.com.SendMsg(mtr.kSpecGet, msg);
            data = [];
            if mtr.WaitForSubtype(sub) % if we got a reply
                switch(sub)
                    case BrushlessController.kSubSpeedKp
                        data = mtr.speed_kp;
                    case BrushlessController.kSubSpeedKi
                        data = mtr.speed_ki;
                    case BrushlessController.kSubSpeedKd
                        data = mtr.speed_kd;
                end
            end
        end
        
        function data = get_speed_kp(mtr)
            data = mtr.get_and_wait(BrushlessController.kSubSpeedKp);
        end
        
        function data = get_speed_ki(mtr)
            data = mtr.get_and_wait(BrushlessController.kSubSpeedKi);
        end
        
        function data = get_speed_kd(mtr)
            data = mtr.get_and_wait(BrushlessController.kSubSpeedKd);
        end
        
        function data = get_electric_lead_rate(mtr)
            msg.msg_type = 12;%lead
            msg.get_set = 0;
            mtr.com.SendMsg(mtr.kTypeRequestMsgSpec,msg);
            if WaitForMessage(mtr,12) % if we got a reply
                data = mtr.electric_lead_rate;
            else
                data = [];
            end
        end
        
        function data = get_commutation_hz(mtr)
            msg.msg_type = 23;%commutation hz
            msg.get_set = 0;
            mtr.com.SendMsg(mtr.kTypeRequestMsgSpec,msg);
            if WaitForMessage(mtr,23) % if we got a reply
                data = mtr.commutation_hz;
            else
                data = [];
            end
        end
        
        function data = get_state(mtr)
            msg.msg_type = 19; %state
            msg.get_set = 0;
            mtr.com.SendMsg(mtr.kTypeRequestMsgSpec,msg);
            if WaitForMessage(mtr,19) % if we got a reply
                data = mtr.state;
            else
                data = [];
                disp('!');
            end
        end
        
        function data = get_calibration_angle(mtr)
           msg.msg_type = 11; %cal
           msg.get_set = 0; %get
           mtr.com.SendMsg(mtr.kTypeRequestMsgSpec,msg);
           if WaitForMessage(mtr,11) % if we got a reply
                data = mtr.calibration_angle;
            else
                data = [];
            end
        end
        
        function data = get_motor_properties(mtr)
           msg.msg_type = 13; %motor
           msg.get_set = 0; %get
           mtr.com.SendMsg(mtr.kTypeRequestMsgSpec,msg);
           if WaitForMessage(mtr,13) % if we got a reply
                data = mtr.motor_properties;
            else
                data = [];
            end
        end
        
        function data = get_permute_wires(mtr)
           msg.msg_type = 18; %permute
           msg.get_set = 0; %get
           mtr.com.SendMsg(mtr.kTypeRequestMsgSpec,msg);
           if WaitForMessage(mtr,18) % if we got a reply
                data = mtr.permute_wires;
            else
                data = [];
            end
        end
        
        function data = get_position_control(mtr)
            msg.msg_type = 17; %state
            msg.get_set = 0;
            mtr.com.SendMsg(mtr.kTypeRequestMsgSpec,msg);
            if WaitForMessage(mtr,17)
                data = mtr.position_control;
            else
                data = [];
            end
        end
        %TODO:: finish all getters

        %% Savers
        
        function save(mtr, sub)
        % save(mtr, sub) sends a parameter save message of type
        % kTypeBrushlessController with subtype sub.
            msg.sub = sub;
            msg.access = BrushlessController.kSave;
            mtr.com.SendMsg(mtr.kSpecSave, msg);
        end

        function save_speed_kp(mtr)
            mtr.save(BrushlessController.kSubSpeedKp);
        end
        
        function save_speed_ki(mtr)
            mtr.save(BrushlessController.kSubSpeedKi);
        end
        
        function save_speed_kd(mtr)
            mtr.save(BrushlessController.kSubSpeedKd);
        end
        
        function save_all(mtr)
            msg.msg_type = 10;
            msg.write_var = 255; %all
            mtr.com.SendMsg(mtr.kTypeFlashWriteSpec,msg);
        end
        
        function save_electric_lead_rate(mtr)
            msg.msg_type = 10;
            msg.write_var = 12; %lead
            mtr.com.SendMsg(mtr.kTypeFlashWriteSpec,msg);
        end
        
        function save_calibration_angle(mtr)
            msg.msg_type = 10;
            msg.write_var = 11; %cal
            mtr.com.SendMsg(mtr.kTypeFlashWriteSpec,msg);
        end
        
        
        function save_permute_wires(mtr)
            msg.msg_type = 10;
            msg.write_var = 18; %permute
            mtr.com.SendMsg(mtr.kTypeFlashWriteSpec,msg);
        end
        
        function save_commutation_hz(mtr)
            msg.msg_type = 10;
            msg.write_var = 23; %commutation hz
            mtr.com.SendMsg(mtr.kTypeFlashWriteSpec,msg);
        end
        
        function save_motor_properties(mtr)
            msg.msg_type = 10;
            msg.write_var = 13; %motor properties
            mtr.com.SendMsg(mtr.kTypeFlashWriteSpec,msg);
        end
        
        function save_current_control(mtr)
            msg.msg_type = 10;
            msg.write_var = 15; %current control
            mtr.com.SendMsg(mtr.kTypeFlashWriteSpec,msg);
        end
        
        function save_position_control(mtr)
            msg.msg_type = 10;
            msg.write_var = 17; %position control
            mtr.com.SendMsg(mtr.kTypeFlashWriteSpec,msg);
        end
        
        %% Parsing
        
        function success = WaitForSubtype(mtr, sub)
        % parse messages until type kTypeBrushlessController with target subtype is found
        % return 1 if message found, else 0 on timeout
            type = BrushlessController.kTypeBrushlessController;
            com = mtr.com;
            end_time = toc + mtr.message_timeout; % tic in MessageInterface
            while(toc < end_time)
                com.GetBytes();
                [rx_type, rx_pkt] = com.PeekPacket();               % check for messages
                if ~isempty(rx_type)                                % if we got a message
                    rx_sub = mtr.ParseMessage(rx_type, rx_pkt);	% parse that message
                    if rx_type == type && rx_sub == sub             % check type/subtype
                        success = true;
                        return;
                    end
                end
            end
            success = false;
        end
        
        function val = UnpackSingleReply(mtr, data)
            val = [];
            msg = UnpackMsg(mtr.kSpecReplySingle, data);
            if msg.access == BrushlessController.kReply
                val = msg.value;
            end
        end
        
        % Get bytes from serial until target sub_id is receieved once, then
        % flush out any remaining complete packets from the parser.
        function success = WaitForMessage(mtr, sub_id)
            com = mtr.com;
            success = 0;
            
            start_time = toc;                       % tic in MessageInterface
            end_time = start_time + mtr.message_timeout;
            while(toc < end_time)
                com.GetBytes();
                while true
                    [msg_type, pkt] = com.PeekPacket();                 % check for messages
                    if ~isempty(msg_type)                               % if we got a message
                        last_sub = ParseMessage(mtr, msg_type, pkt);    % parse that message
                        if last_sub == sub_id                           % if it's the message we've been waiting for
                            success = 1;
                        end
                    else
                        if success
                            return;
                        end;
                        break;
                    end
                end
            end
        end
        
        function sub_id = ParseMessage(mtr, msg_id, data)
            % parseMessage(mtr, msg_id, data) takes a message and assignes its data to its proper parameter
            sub_id = [];
            try
                if msg_id == BrushlessController.kTypeBrushlessController
                    sub_id = data(1);
                    switch(sub_id) %sub id
                        case 19 % state, first for speed!
                            % this is ugly -- you can't unpack it
                            % before checking if it's a get/set/reply/save
                            if(data(2) == 3)
                                msg = UnpackMsg(mtr.kTypeStateSpec,data);
%                             if msg.get_set == 3 % response
                                mtr.state.pwm = msg.pwm;
                                mtr.state.position = msg.position;
                                mtr.state.speed = msg.speed;
                                mtr.state.current = msg.current;
                                mtr.state.voltage = msg.voltage;
                            end
                        case 11 % calibration angle
                            msg = UnpackMsg(mtr.kTypeCalSpec,data);
                            if msg.get_set == 3 % response
                                mtr.calibration_angle = msg.cal_ang;
                            end
                        case 12 % lead per velocity
                            msg = UnpackMsg(mtr.kTypeElectricLeadRateSpec,data);
                            if msg.get_set == 3 % response
                                mtr.electric_lead_rate = msg.electric_lead_rate;
                            end
                        case 13 % motor data
                            msg = UnpackMsg(mtr.kTypeMotorSpec,data);
                            if msg.get_set == 3 % response
                                msg = rmfield(msg,{'msg_type','get_set'});
                                mtr.motor_properties = msg;
                            end
                        case 15 % current control parameters
                            msg = UnpackMsg(mtr.kTypePIDSpec,data);
                            if msg.get_set == 3 % response
                                msg = rmfield(msg,{'msg_type','get_set'});
                                mtr.current_control = msg;
                            end
                        case 17 % position control parameters
                            msg = UnpackMsg(mtr.kTypePIDSpec,data);
                            if msg.get_set == 3 % response
                                msg = rmfield(msg,{'msg_type','get_set'});
                                mtr.position_control = msg;
                            end
                        case 18 % permute wires
                            msg = UnpackMsg(mtr.kTypePermuteSpec,data);
                            if msg.get_set == 3 % response
                                mtr.permute_wires = msg.permute;
                            end
                        case 23 % commutation hz
                            msg = UnpackMsg(mtr.kTypeCommutationHzSpec,data);
                            if msg.get_set == 3 % response
                                mtr.commutation_hz = msg.commutation_hz;
                            end

                        case BrushlessController.kSubSpeedKp
                            mtr.speed_kp = mtr.UnpackSingleReply(data);
                        case BrushlessController.kSubSpeedKi
                            mtr.speed_ki = mtr.UnpackSingleReply(data);
                        case BrushlessController.kSubSpeedKd
                            mtr.speed_kd = mtr.UnpackSingleReply(data);
                    end
                end
            catch err
                warning('ParseMessage error.')
                rethrow(err);
            end
        end

%% Display
        function dispParams(mtr)
            disp(['electric lead rate:         ' num2str(mtr.electric_lead_rate)]);
            disp(['calibration angle:          ' num2str(mtr.calibration_angle)]);
            disp(['cog smoothing amount:       ' num2str(mtr.anticog_amplitude)]);
            disp(['permute wires:              ' num2str(mtr.permute_wires)]);
            disp(['Kv:                         ' num2str(mtr.motor_properties.Kv)]);
            disp(['R:                          ' num2str(mtr.motor_properties.R)]);
            disp(['V0:                         ' num2str(mtr.motor_properties.V0)]);
            disp(['I0:                         ' num2str(mtr.motor_properties.I0)]);
            disp(['bemf_shape:                 ' num2str(mtr.motor_properties.bemf_shape)]);
            disp(['commutation hz              ' num2str(mtr.commutation_hz)]);
        end % dispParams
        
        function dispCtrl(mtr)
            disp('Speed Control');
            disp('~~~~~~~~~~~~~~~~');
            disp(['  kp: ' num2str(mtr.speed_kp)]);
            disp(['  ki: ' num2str(mtr.speed_ki)]);
            disp(['  kd: ' num2str(mtr.speed_kd)]);
%             disp(['  kc: ' num2str(get(mtr.spdCtrl,'kc'))]);
%             disp(['  p1: ' num2str(get(mtr.spdCtrl,'p1'))]);
%             disp(['  p2: ' num2str(get(mtr.spdCtrl,'p2'))]);
%             disp(['  p3: ' num2str(get(mtr.spdCtrl,'p3'))]);
            disp('Current Control');
            disp('~~~~~~~~~~~~~~~~');
            disp(['  kp: ' num2str(mtr.current_control.Kp)]);
            disp(['  ki: ' num2str(mtr.current_control.Ki)]);
            disp(['  kd: ' num2str(mtr.current_control.Kd)]);
%             disp(['  kc: ' num2str(get(mtr.trqCtrl,'kc'))]);
%             disp(['  p1: ' num2str(get(mtr.trqCtrl,'p1'))]);
%             disp(['  p2: ' num2str(get(mtr.trqCtrl,'p2'))]);
%             disp(['  p3: ' num2str(get(mtr.trqCtrl,'p3'))]);            
            disp('Position Control');
            disp('~~~~~~~~~~~~~~~~');
            disp(['  kp: ' num2str(mtr.position_control.Kp)]);
            disp(['  ki: ' num2str(mtr.position_control.Ki)]);
            disp(['  kd: ' num2str(mtr.position_control.Kd)]);
%             disp(['  kc: ' num2str(get(mtr.posCtrl,'kc'))]);
%             disp(['  p1: ' num2str(get(mtr.posCtrl,'p1'))]);
%             disp(['  p2: ' num2str(get(mtr.posCtrl,'p2'))]);
%             disp(['  p3: ' num2str(get(mtr.posCtrl,'p3'))]);            
        end % dispCtrl
        
        function dispState(mtr)
            disp('State');
            disp('~~~~~~~~~~~~~~~~');
            disp(['PWM: ' num2str(mtr.state.pwm) ' (PU)']);
            disp(['Position: ' num2str(mtr.state.position) ' (radians)']);
            disp(['Speed: ' num2str(mtr.state.speed) ' (rad/s)']);
            disp(['Current: ' num2str(mtr.state.current) ' A']);
            disp(['Voltage: ' num2str(mtr.state.voltage) ' V']);
        end % dispState
        
    end % methods
    
end