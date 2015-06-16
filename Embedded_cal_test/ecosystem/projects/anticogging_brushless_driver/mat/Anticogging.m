classdef Anticogging < handle
    %Anticogging is a class that helps talk to the anticogging c code.
    
    properties 
        cog_data %in volts
        encoder_counts
    end
    
    properties (SetAccess = private, Hidden)
        com % handle to this module's communicationMonitor
        message_timeout % timeout given to receive a message
        made_communication_monitor % true if this class made the communication monitor, false otherwise
    end
    
    properties (Constant)
        KTypeAnticogDataSpec = struct('Type', 128, 'Fields', struct('cmd', 'uint8','index','uint16','value','single'));
        KTypeAnticogSpec = struct('Type', 129, 'Fields', struct('cmd', 'uint8'));
    end
    
    methods
        function cog = Anticogging(varargin) %constructor
            try
                com        = getArgVal('com',   varargin, []);
            catch err
                if strcmp(err.identifier,'MATLAB:UndefinedFunction')
                    error('Cannot find getArgVal in the path.  It might be at http://svn.modlabupenn.org/libraries/Matlab_Utilities (rev 67). Find the directory it is in and use addpath() to make it visible.')
                end
            end
            %initialize
            cog.message_timeout = getArgVal('timeout', varargin, .25);
            %need to know ENCODER_COUNTS
            cog.encoder_counts = getArgVal('encoder_counts', varargin, []);
            if isempty(cog.encoder_counts)
                cog.encoder_counts = input(['The anticogging class requires number of encoder counts but is unable to ask the driver directly.' char(10) 'Please type the number of encoder counts then press Enter: ']);
            end
            cog.cog_data = getArgVal('cog_data', varargin, nan(1,cog.encoder_counts));
            
            if isempty(com)
                com = input(['The Anticogging class requires a communication_monitor.' char(10) 'Please enter the com port used to talk to the module: '],'s');
            end
            
            if ischar(com)
                cog.com = MessageInterface(com,460800); %115200
                cog.made_communication_monitor = true;
            elseif isa(com, 'MessageInterface')
                cog.com = com;
                cog.made_communication_monitor = false;
            else
                cog.made_communication_monitor = false;
                error('No communication monitor properly defined.');
            end
        end
        
        function delete(cog) %motor destructor
           if cog.made_communication_monitor
               cog.com.delete();
           end
        end %motor destructor     
        
        function Enable(cog)
            msg.cmd = 1; %enable
            cog.com.SendMsg(cog.KTypeAnticogSpec,msg);
        end
        
        function Disable(cog)
            msg.cmd = 2; %disable
            cog.com.SendMsg(cog.KTypeAnticogSpec,msg);
        end
        
        function Demo(cog)
            msg.cmd = 3; %demo
            cog.com.SendMsg(cog.KTypeAnticogSpec,msg);
        end
        
        function Erase(cog)
            msg.cmd = 0;
            cog.com.SendMsg(cog.KTypeAnticogSpec,msg);
        end
        
        function set_cog_data_radian(cog,position, value) % position in radians
            %convert position in radians to index (this could be an array)
            %send each index/voltage pair
            cog.set_cog_data_index(round(position./(2*pi).*cog.encoder_counts),value);
        end
        
        function set_cog_data_index(cog,index,value)
            if(length(index) ~= length(value))
                error('The size of index and value must be equal but are not.');
            end
            msg.cmd = 1; %set
            for i = 1:length(index)
                msg.index = index(i);
                msg.value = value(i);
                cog.com.SendMsg(cog.KTypeAnticogDataSpec,msg);
                pause(.0000001);
            end
        end
        
        function values = get_cog_data_radian(cog,position) % position in radians
            %convert position in radians to index (this could be an array)
            %get each voltage from microcontroller
            values = cog.get_cog_data_index(round(position./(2*pi).*cog.encoder_counts));
        end
        
        function values = get_cog_data_index(cog,index) % gets the zero based anticog data at index
            msg.cmd = 0;%get
            msg.value = 0;
            values(length(index)) = NaN;
            for i = 1:(length(index))
                msg.index = index(i);
                cog.com.SendMsg(cog.KTypeAnticogDataSpec,msg);
                if WaitForMessage(cog,128,index(i)) % if we got a reply
                    values(i) = cog.cog_data(index(i)+1); % cog_data is 1 based index
                else
                    values(i) = NaN;
                end
            end
        end
        
        function values = get_all_cog_data(cog) % ask for all the positions and store in this object
            j = 0:(cog.encoder_counts-1);
            values = cog.get_cog_data_index(j);
        end
        
        %% Parsing
        function success = WaitForMessage(cog,msg_id, index)
            com = cog.com;
            start_time = toc; % Guaranteed tic in message interface
            while(toc < start_time + cog.message_timeout) % while we're within out time limit
                com.GetBytes();
                [msg_type, pkt] = com.PeekPacket(); % check for messages
                if ~isempty(msg_type) %if we got a message
                    [last_id, last_index] = ParseMessage(cog,msg_type,pkt); % parse that message
                    if last_id == msg_id % if it's the message we've been waiting for
                        if(index == last_index)
                            success = 1;
                            return;
                        end
                    end
                end
            end
            success = 0; % sad, we didn't find our message
        end
        
        function [msg_id index] = ParseMessage(cog, msg_id, data)
            index = [];
            % parseMessage(mtr, msg_id, data) takes a message and assignes its data to its proper parameter
            if msg_id == 128 %KTypeAnticogDataSpec
                msg = UnpackMsg(cog.KTypeAnticogDataSpec,data);
                if(msg.cmd == 3) %RESPONSE
                    index = msg.index;
                    cog.cog_data(index+1) = msg.value;
                end
%             elseif msg_id == 129 %KTypeAnticogSpec
%                 msg = UnpackMsg(cog.KTypeAnticogSpec,data);
%                 if(msg.cmd == ?? % we don't care about these types
            end
        end
    end
    
end

