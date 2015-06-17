classdef usb_com < handle
    
    properties 
        data
        time_sec
        time_nsec
        encoder_counter
        velocity
        position
        adc0
        adc1
    end
    
    properties (SetAccess = private, Hidden)
        com % handle to this module's communicationMonitor
        message_timeout % timeout given to receive a message
        made_communication_monitor % true if this class made the communication monitor, false otherwise
    end
    
    properties (Constant)
        MsgCmd = struct('Type', 101, 'Fields', struct('cmd', 'uint16'));
        MsgStates = struct('Type', 102, 'Fields', struct('time_sec', 'uint16', 'time_nsec', 'uint16', 'encoder_counter', 'int16', 'velocity', 'int16', 'position', 'int16', 'adc0', 'int16', 'adc1', 'int16'));
    end
    
    methods
        function UsbCom = usb_com(varargin) %constructor
            try
                com        = getArgVal('com',   varargin, []);
            catch err
                if strcmp(err.identifier,'MATLAB:UndefinedFunction')
                    error('Cannot find getArgVal in the path.')
                end
            end
            %initialize
            UsbCom.message_timeout = getArgVal('timeout', varargin, .25);
            UsbCom.data = getArgVal('data', varargin, []);
            
            if isempty(com)
                com = input(['The usb_com class requires a communication_monitor.' char(10) 'Please enter the com port used to talk to the module: '],'s');
            end
            
            if ischar(com)
                UsbCom.com = MessageInterface(com,460800); %115200
                UsbCom.made_communication_monitor = true;
            elseif isa(com, 'MessageInterface')
                UsbCom.com = com;
                UsbCom.made_communication_monitor = false;
            else
                UsbCom.made_communication_monitor = false;
                error('No communication monitor properly defined.');
            end
        end
        
        function delete(UsbCom) %motor destructor
           if UsbCom.made_communication_monitor
               UsbCom.com.delete();
           end
        end 
        
        function send_usb_cmd(UsbCom, cmd)
            msg.cmd = cmd;
            UsbCom.com.SendMsg(UsbCom.MsgCmd, msg);
        end
        
        function data = get_usb_data(UsbCom, cmd) % get the data from usb device
            msg.cmd = cmd;%get data command
            UsbCom.com.SendMsg(UsbCom.MsgCmd, msg);
            if WaitForMessage(UsbCom,UsbCom.MsgCmd.Type) % if we got a reply
                data = UsbCom.data; % store the data
            end
        end
        
        function [time_sec, time_nsec, encoder_counter, velocity, position, adc0, adc1] = get_usb_data_continuous(UsbCom)
            if WaitForMessage(UsbCom, UsbCom.MsgStates.Type)
                time_sec = UsbCom.time_sec;
                time_nsec = UsbCom.time_nsec;
                encoder_counter = UsbCom.encoder_counter;
                velocity = UsbCom.velocity;
                position = UsbCom.position;
                adc0 = UsbCom.adc0;
                adc1 = UsbCom.adc1;
            end
        end    
        
        %% Parsing
        function success = WaitForMessage(UsbCom,msg_id)
            com = UsbCom.com;
            start_time = toc; % Guaranteed tic in message interface
            while(toc < start_time + UsbCom.message_timeout) % while we're within out time limit
                com.GetBytes();
                [msg_type, pkt] = com.PeekPacket(); % check for messages
                if ~isempty(msg_type) %if we got a message
                    [last_id] = ParseMessage(UsbCom,msg_type,pkt); % parse that message
                    if last_id == msg_id % if it's the message we've been waiting for
                        success = 1;
                        return;
                    end
                end
            end
            success = 0; % sad, we didn't find our message
        end
        
        function [msg_id] = ParseMessage(UsbCom, msg_id, data)
            % parseMessage(mtr, msg_id, data) takes a message and assignes its data to its proper parameter
            if msg_id == UsbCom.MsgStates.Type %MsgMyData
                msg = UnpackMsg(UsbCom.MsgStates,data);
                UsbCom.time_sec = msg.time_sec;
                UsbCom.time_nsec = msg.time_nsec;
                UsbCom.encoder_counter = msg.encoder_counter;
                UsbCom.velocity = msg.velocity;
                UsbCom.position = msg.position;
                UsbCom.adc0 = msg.adc0;
                UsbCom.adc1 = msg.adc1;
            end
        end
    end
    
end
