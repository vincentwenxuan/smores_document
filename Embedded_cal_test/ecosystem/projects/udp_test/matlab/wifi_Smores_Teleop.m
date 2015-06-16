classdef wifi_Smores_Teleop
    %UNTITLED Summary of this class goes here
    %   Detailed explanation goes here
    
    properties
        UDP
        dof_names       % DoF names and ID's
        dof_ids
        dof_name_map
        dir_names       % Direction names and ID's
        dir_ids
        dir_name_map
        face_names      % Face names and ID's
        face_ids
        face_name_map
        magnet_state_names % magnet state names and ID's
        magnet_state_ids
        magnet_state_map
        
    end
    
    methods
        %%%%%%%%%%% Constructor:
        function obj = wifi_Smores_Teleop()
            addpath ../../../mat_common/user_lib/src/
            addpath ../../../mat_common/user_lib/mex/
            %obj.UsbCom = usb_com('com', '/dev/ttyACM0');
            obj.UDP = udp('10.0.0.3', 2222); 
            % DoF names and ID's
            obj.dof_names = {'left', 'right', 'pan', 'tilt'};
            obj.dof_ids = [11, 12, 13, 14];
            obj.dof_name_map = containers.Map( obj.dof_names, obj.dof_ids );
            % Direction names and ID's
            obj.dir_names = {'ccw', 'cw'};
            obj.dir_ids = [1, 0];
            obj.dir_name_map = containers.Map(obj.dir_names, obj.dir_ids);
            % Face names and ID's
            obj.face_names = {'left','right','top','bottom'};
            obj.face_ids = [21,22,23,24]; %TODO: maybe not right!
            obj.face_name_map = containers.Map(obj.face_names, obj.face_ids);
            % magnet state names and ID's
            obj.magnet_state_names = {'off', 'on'};
            obj.magnet_state_ids = [0, 1];
            obj.magnet_state_map = containers.Map(obj.magnet_state_names,...
                                    obj.magnet_state_ids);
        end
        %%%%%%%%%%% Motor Commands   %%%%%%%%%%%%
        function moveDofTime(obj, dof_name, direction, speed, duration)
        % moveDofTime: Moves the specified dof in direction at speed 
        % (0-100, pwm percentage) for duration (seconds)
            D.DoF = obj.dof_name_map(dof_name);
            D.speed = speed;
            D.direction = obj.dir_name_map(direction);
            obj.UsbCom.send_usb_dof(D);
            pause(duration);
            D.speed = 0;
            obj.UsbCom.send_usb_dof(D);
        end
        %
        function stopAllMotors(obj)
        % stopAllMotors: Stops all motors immediatedly
            for dof = obj.dof_ids
                D.direction = 0;
                D.DoF = dof;
                D.speed = 0;
                obj.UsbCom.send_usb_dof(D);
            end 
        end
        %%%%%%%%%%   Face Board Commands   %%%%%%%%%%
        function data = pingFace(obj, face_name)
        % Requests that the specified face send back a message indicating
        % life.
        % Face-name-request commands are all greater than 100.
        % 121 = face 1, 122 = face 2, 123 = face 3, 124 = face 4.
        cmd = 100 + obj.face_name_map(face_name);
        data = obj.UsbCom.get_usb_data(cmd);
        % Note: I am using Chao's existing MsgCmd and MsgModule messages.
        end
        %%%
        function magnetControl(obj, face_name, state_name)
            Mag.face = obj.face_name_map(face_name);
            Mag.state = obj.magnet_state_map(state_name);
            obj.UsbCom.send_usb_magnet(Mag);
        end
    end
    
end

