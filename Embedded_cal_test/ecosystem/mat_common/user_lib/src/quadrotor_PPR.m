classdef quadrotor_PPR < vehicle
    %quadrotor_PPR is the class that describes a PPR quadrotor
    
    properties
        useExternYaw
        maxRP
        maxT
        maxXYVel
        maxZVel
        xdAlpha
        m
        calculateCmdSupported
        calculateCmd
        joyButtonsOld
        joyTrimDelta
        tOld
        viconKp
        viconKd
        desYaw
    end
    
    methods
        function obj = quadrotor_PPR(varargin)
            obj = obj@vehicle(varargin{:});
            
            obj.tOld = 0;
            
            obj.stateVicon.x = [0; 0; 0];
            obj.stateVicon.xDes = [0; 0; 0];
            obj.stateVicon.xd = [0; 0; 0];
            obj.stateVicon.xdDes = [0; 0; 0];
            obj.stateVicon.R = [0; 0; 0];
            obj.stateVicon.time = 0;
            obj.useExternYaw = false;
            obj.stateVicon.enabled = false;
            obj.viconKp = getArgVal('viconKp', varargin, [0.6; 0.6; 1.2]);
            obj.viconKd = getArgVal('viconKd', varargin, [0.5; 0.5; 0.8]);
            
            obj.stateVehicle.xdd = [0 0 0];
            obj.stateVehicle.R = [0 0 0];
            obj.stateVehicle.Rd = [0 0 0];
            obj.stateVehicle.pwm = [0 0 0 0];
            obj.stateVehicle.time = 0;
            obj.stateVehicle.desThrust = 0;
            obj.stateVehicle.desRoll = 0;
            obj.stateVehicle.desPitch = 0;
            obj.stateVehicle.desYaw = 0;
            obj.stateVehicle.estRoll = 0;
            obj.stateVehicle.estPitch = 0;
            obj.stateVehicle.estYaw = 0;
            obj.stateVehicle.estAngVelX = 0;
            obj.stateVehicle.estAngVelY = 0;
            obj.stateVehicle.estAngVelZ = 0;
            obj.stateVehicle.estAccX = 0;
            obj.stateVehicle.estAccY = 0;
            obj.stateVehicle.estAccZ = 0;
            obj.stateVehicle.PWMs = 0;
            
            obj.maxRP = getArgVal('maxRP',varargin,pi/9);
            obj.maxT = getArgVal('maxT',varargin,1.0); % N
            obj.maxXYVel = getArgVal('maxXYVel',varargin,.5); % m/s
            obj.maxZVel = getArgVal('maxZVel',varargin,.5); % m/s
            
            obj.xdAlpha = getArgVal('xdAlpha',varargin,.9);
            
            obj.m = getArgVal('m',varargin,.047);
            
            obj.joySupported = {'xbox360_default'; 'xbox360_RC'; 'GP_RC'};
            % [ T, R, P, Y ]
            obj.joyAxisMaps = {...
                [ 0, 1, 0, 0; ...% x
                0, 0, -1, 0; ...% y
                -1, 0, 0, 0; ...% z
                0, 0, 0, 1; ...% rx
                0, 0, 0, 0 ], ...% ry
                [ 0, 0, 0, -1; ...% x
                -1, 0, 0, 0; ...% y
                0, 0, 0, 0; ...% z
                0, 1, 0, 0; ...% rx
                0, 0, -1, 0 ], ...% ry
                [ 0, 1, 0, 0; ... % 1
                0, 0, -1, 0; ... % 2
                -1, 0, 0, 0; ... % 3
                0, 0, 0, 0; ... % 4
                0, 0, 0, -1 ] ... % 5
                };
            
            % [start, enable, disable, vicon, T trim down, T trim up]
            obj.joyButtonMaps = {...
                [ 0, 1, 0, 0, 0, 0; ... % 1
                0, 0, 1, 0, 0, 0; ... % 2
                0, 0, 0, 1, 0, 0; ... % 3
                0, 0, 0, 0, 0, 0; ... % 4
                0, 0, 0, 0, 1, 0; ... % 5
                0, 0, 0, 0, 0, 1; ... % 6
                0, 0, 0, 0, 0, 0; ... % 7
                1, 0, 0, 0, 0, 0; ... % 8
                0, 0, 0, 0, 0, 0; ... % 9
                0, 0, 0, 0, 0, 0], ... % 10
                [ 0, 1, 0, 0, 0, 0; ... % 1
                0, 0, 1, 0, 0, 0; ... % 2
                0, 0, 0, 1, 0, 0; ... % 3
                0, 0, 0, 0, 0, 0; ... % 4
                0, 0, 0, 0, 1, 0; ... % 5
                0, 0, 0, 0, 0, 1; ... % 6
                0, 0, 0, 0, 0, 0; ... % 7
                1, 0, 0, 0, 0, 0; ... % 8
                0, 0, 0, 0, 0, 0; ... % 9
                0, 0, 0, 0, 0, 0], ... % 10
                [ 0, 0, 0, 1, 0, 0; ... % 1
                0, 1, -1, 0, 0, 0; ... % 2
                0, 0, 0, 0, 0, 0; ... % 3
                1, 0, 0, 0, 0, 0; ... % 4
                0, 0, 0, 0, 0, 0] ... % 5
                };
            obj.joyPOVMaps = {1, 1, 0};
            
            obj.setJoyMap(getArgVal('joyMap',varargin,obj.joySupported(1)));
            
            obj.joyTrimDelta = getArgVal('joyTrimDelta', varargin, .1);
            obj.joyAxisTrim = getArgVal('joyAxisTrim',varargin,zeros(1,size(obj.joyAxisMaps{obj.joyMap},2)));
            obj.joyButtonsOld = zeros(1,size(obj.joyButtonMaps{obj.joyMap},2));
            
            obj.desYaw = 0;
        end
        
        function parseMessage(obj, type, data)
            for i = 1:length(type)
                datai = data{i};
                switch type(i)
                    case 't'
                        obj.stateVehicle.time = typecast(datai(1:2),'uint16');
                        obj.stateVehicle.desThrust = typecast(datai(3:4),'int16');
                        obj.stateVehicle.desRoll = typecast(datai(5:6),'int16');
                        obj.stateVehicle.desPitch = typecast(datai(7:8),'int16');
                        obj.stateVehicle.desYaw = typecast(datai(9:10),'int16');
                        obj.stateVehicle.estRoll = typecast(datai(11:12),'int16');
                        obj.stateVehicle.estPitch = typecast(datai(13:14),'int16');
                        obj.stateVehicle.estYaw = typecast(datai(15:16),'int16');
                        obj.stateVehicle.estAngVelX = typecast(datai(17:18),'int16');
                        obj.stateVehicle.estAngVelY = typecast(datai(19:20),'int16');
                        obj.stateVehicle.estAngVelZ = typecast(datai(21:22),'int16');
                        obj.stateVehicle.estAccX = typecast(datai(23:24),'int16');
                        obj.stateVehicle.estAccY = typecast(datai(25:26),'int16');
                        obj.stateVehicle.estAccZ = typecast(datai(27:28),'int16');
                        obj.stateVehicle.PWMs = typecast(datai(29:36),'uint16');
                end
            end
        end
        
        function [type, data] = calculateJoyViconRelPosCmd(obj, joyAxes, state)
            % Generate new command
            obj.stateVicon.xdDes = [obj.maxXYVel*joyAxes(2); obj.maxXYVel*joyAxes(3); obj.maxZVel*joyAxes(1)];
            obj.stateVicon.xDes = obj.stateVicon.xDes + obj.stateVicon.xdDes*(state.time - obj.stateVicon.time);
            obj.desYaw = obj.desYaw + pi*joyAxes(4)/300;
            if obj.desYaw > pi
                obj.desYaw = obj.desYaw - 2*pi;
            elseif obj.desYaw < -pi
                obj.desYaw = obj.desYaw + 2*pi;
            end
            %disp(obj.stateVicon.xDes)
            [type, data] = calculateViconRelPosCmd(obj, state);
        end
        
        function [type, data] = calculateViconRelPosCmd(obj, state)
            obj.useExternYaw = true;
            
            % Calculate full state
            obj.stateVicon.xd = obj.stateVicon.xd*(1-obj.xdAlpha) + (state.stateVicon.x - obj.stateVicon.x)/(state.time - obj.stateVicon.time)*obj.xdAlpha;
            
            % Assign rest of state
            obj.stateVicon.x = state.stateVicon.x;
            obj.stateVicon.R = state.stateVicon.R;
            obj.stateVicon.time = state.time;
            
            % Calculate errors
            xDesErr = obj.stateVicon.xDes - obj.stateVicon.x;
            xdDesErr = obj.stateVicon.xdDes - obj.stateVicon.xd;
            %             disp(obj.stateVicon.xDes);
            %             disp(obj.stateVicon.xd);
            % Calculate desired force direction
            forceDes = obj.viconKp.*xDesErr + obj.viconKd.*xdDesErr + obj.m*[0; 0; 9.81];
            %             disp(forceDes);
            % Assign values
            yaw = atan2(obj.stateVicon.R(2,1), obj.stateVicon.R(1,1));
            commands = [forceDes(3), forceDes(1)*sin(yaw) - forceDes(2)*cos(yaw), forceDes(1)*cos(yaw) + forceDes(2)*sin(yaw), obj.desYaw];
            
            type = 'p';
            data = [typecast(int16([commands, obj.stateVicon.R(3)]*10000),'uint8'), uint8([obj.enabled, obj.useExternYaw])];
        end
        
        function [type, data] = calculateJoyVehicleOrientationCmd(obj, joyAxes)
            obj.useExternYaw = false;
            obj.desYaw = obj.desYaw + pi*joyAxes(4)/300;
            if obj.desYaw > pi
                obj.desYaw = obj.desYaw - 2*pi;
            elseif obj.desYaw < -pi
                obj.desYaw = obj.desYaw + 2*pi;
            end
            %             obj.desYaw = 0;
            commands = [obj.maxT*joyAxes(1) obj.maxRP*joyAxes(2) obj.maxRP*joyAxes(3) obj.desYaw];
            
            type = 'p';
            data = [typecast(int16([commands, obj.stateVicon.R(3)]*10000),'uint8'), uint8([obj.enabled, obj.useExternYaw])];
        end
        
        function [type, data] = calculateJoyVehicleGainsCmd(obj)
            type = 'g';
            data = typecast(int16([obj.gains.kpxy,obj.gains.kdxy,obj.gains.kpz,obj.gains.kdz,]),'uint8');
        end
        
        function [type, data] = processInputs(obj, state)
            type = [];
            data = {};
            sendGains = false;
            tCurrent = state.time;
            joyAxes = state.joyAxes*obj.joyAxisMaps{obj.joyMap};
            joyButtons = state.joyButtons*obj.joyButtonMaps{obj.joyMap};
            joyPOVs = state.joyPOVs*obj.joyPOVMaps{obj.joyMap};
            if isempty(joyPOVs)
                joyPOVs = -1;
            end
            
            if joyButtons(1) == 1 + obj.joyButtonsOld(1) % Activate/deactvate vehicle
                if obj.activated
                    obj.deactivate();
                    out(obj.verbose, 'Vehicle deactivated');
                else
                    obj.activate();
                    out(obj.verbose, 'Vehicle activated');
                end
            end
            
            if joyButtons(2) == 1 + obj.joyButtonsOld(2) % Enable motors button press
                obj.enable();
                sendGains = true;
                out(obj.verbose, 'Vehicle enabled');
            end
            
            if joyButtons(3) == 1 + obj.joyButtonsOld(3) % Disable motors button press
                obj.disable();
                out(obj.verbose, 'Vehicle disabled');
            end
            
            if joyButtons(6)
                trimZ = 1;
            elseif joyButtons(5)
                trimZ = -1;
            else
                trimZ = 0;
            end
            % TODO::Cannot trim all axes!!!
            if trimZ
                obj.joyAxisTrim = obj.joyAxisTrim + [trimZ, 0, 0, 0]*(tCurrent - obj.tOld)*obj.joyTrimDelta;
                out(obj.verbose, ['Trimmed to: ' num2str(obj.joyAxisTrim)]);
            elseif joyPOVs(1) ~= -1
                obj.joyAxisTrim = obj.joyAxisTrim + [0, sin(deg2rad(joyPOVs(1))), cos(deg2rad(joyPOVs(1))), 0]*(tCurrent - obj.tOld)*obj.joyTrimDelta;
                out(obj.verbose, ['Trimmed to: ' num2str(obj.joyAxisTrim)]);
            end
            joyAxes = joyAxes + obj.joyAxisTrim;
            
            if obj.stateVicon.enabled
                [type, data] = obj.calculateJoyViconRelPosCmd(joyAxes,state);
            else
                [type, data] = obj.calculateJoyVehicleOrientationCmd(joyAxes);
            end
            if sendGains == true
                [type, data] = obj.calculateJoyVehicleGainsCmd();
            end
            
            if joyButtons(4) == 1 + obj.joyButtonsOld(4) % Disable motors button press
                if obj.stateVicon.enabled
                    obj.stateVicon.enabled = false;
                    out(obj.verbose, 'Vicon enabled');
                else
                    obj.stateVicon.enabled = true;
                    out(obj.verbose, 'Vicon enabled');
                end
            end
            
            obj.joyButtonsOld = joyButtons;
            obj.tOld = tCurrent;
        end
        
    end
    
    methods(Static)
        function [type data] = getEmergencyStopCmd()
            type = 'p';
            data = zeros(12,1,'uint8');
        end
    end
end

