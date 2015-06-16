classdef smart_copter < vehicle
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
        currentState
        id
    end
    
    methods
        function obj = smart_copter(varargin)            
            obj = obj@vehicle(varargin{:});
            
            obj.tOld = 0;
            
            obj.currentState.x = [0 0 0];
            obj.currentState.xd = [0 0 0];
            obj.currentState.motorx = 0;
            obj.currentState.motorxd = 0;
            
            obj.stateVicon.x = [0 0 0];
            obj.stateVicon.xDes = [0 0 0];
            obj.stateVicon.xd = [0 0 0];
            obj.stateVicon.xdDes = [0 0 0];
            obj.stateVicon.R = [0 0 0];
            obj.stateVicon.time = 0;
            obj.useExternYaw = false;
            obj.stateVicon.enabled = false;
            obj.viconKp = getArgVal('viconKp', varargin, [1 1 1]);
            obj.viconKd = getArgVal('viconKd', varargin, [.1 .1 .1]);
            
            obj.stateVehicle.xdd = [0 0 0];
            obj.stateVehicle.R = [0 0 0];
            obj.stateVehicle.Rd = [0 0 0];
            obj.stateVehicle.pwm = [0 0 0 0];
            obj.stateVehicle.time = 0;
            
            obj.maxRP = getArgVal('maxRP',varargin,pi/9);
            obj.maxT = getArgVal('maxT',varargin,.9); % N
            obj.maxXYVel = getArgVal('maxXYVel',varargin,.5); % m/s
            obj.maxZVel = getArgVal('maxZVel',varargin,.5); % m/s
            
            obj.xdAlpha = getArgVal('xdAlpha',varargin,.5);
            
            obj.m = getArgVal('m',varargin,.04);
            
            obj.joySupported = {'xbox360_default'; 'xbox360_RC'; 'GP_RC'};
            % [ T, R, P, Y ]
            obj.joyAxisMaps = {...
              [ 0, 1, 0, 0; ...% x
                0, 0, -1, 0; ...% y
                -1, 0, 0, 0; ...% z
                0, 0, 0, 1; ...% rx
                0, 0, 0, 0 ], ...% ry
              [ 0, 0, 0, 0; ...% x
                -1, 0, 0, 0; ...% y
                0, 0, 0, -1; ...% z
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
                switch(type(i))
                    case 82 % Rotations feedback
                        obj.currentState.motorxd = typecast(data{i}(1:4),'single');
                        obj.currentState.xd(3) = typecast(data{i}(5:8),'single');
                end
            end
        end
        
        function [type, data] = calculateJoyViconRelPosCmd(obj, joyAxes)
%             obj.useExternYaw = true;
%             
%             % Generate new command
%             obj.stateVicon.xdDes = [obj.maxXYVel*joyAxes(2) obj.maxXYVel*joyAxes(3) obj.maxZVel*joyAxes(1)];
%             obj.stateVicon.xDes = obj.stateVicon.xDes + obj.stateVicon.xdDes*(state.time - obj.stateVicon.time);
%             
%             % Calculate full state
%             obj.stateVicon.xd = obj.stateVicon.xd*(1-obj.xdAlpha) + (state.x - obj.stateVicon.x)/(state.time - obj.stateVicon.time)*obj.xdAlpha;
%             
%             % Assign rest of state
%             obj.stateVicon.x = state.x;
%             obj.stateVicon.R = state.R;
%             obj.stateVicon.time = state.time;
%             
%             % Calculate errors
%             xDesErr = obj.stateVicon.xDes - obj.stateVicon.x;
%             xdDesErr = obj.stateVicon.xdDes - obj.stateVicon.xd;
%             
%             % Calculate desired force direction
%             forceDes = obj.viconKp.*xDesErr + obj.viconKd*xdDesErr + obj.m*[0, 0, 9.81];
%             
%             % Assign values
%             commands = [forceDes(3), forceDes(1)*sin(state.R(3)) - forceDes(2)*cos(state.R(3)), forceDes(1)*cos(state.R(3)) + forceDes(1)*sin(state.R(3)), obj.stateVicon.xDes(3)];
%             
%             type = 'p';
%             data = [typecast(int16([commands, obj.stateVicon.R(3)]*10000),'uint8'), uint8([obj.enabled, obj.useExternYaw])];
        end
        
        function [type, data] = calculateJoyVehicleOrientationCmd(obj, joyAxes)
%             obj.useExternYaw = false;
%             obj.desYaw = obj.desYaw + pi*joyAxes(4)/300;
%             if obj.desYaw > pi
%                 obj.desYaw = obj.desYaw - 2*pi;
%             elseif obj.desYaw < -pi
%                 obj.desYaw = obj.desYaw + 2*pi;
%             end
%             obj.desYaw = 0;
%             commands = [obj.maxT*joyAxes(1) obj.maxRP*joyAxes(2) obj.maxRP*joyAxes(3) obj.desYaw];
%             
            type = 'T';
%             data = [typecast(int16([commands, obj.stateVicon.R(3)]*10000),'uint8'), uint8([obj.enabled, obj.useExternYaw])];
            if obj.enabled
%                 data = uint8(typecast(single(max(joyAxes(1),0)),'uint8'));
%                 [phase,amplitude] = cart2pol(joyAxes(2),joyAxes(3));
%                 data = uint8([obj.id, typecast(single([joyAxes(1) amplitude phase pi*joyAxes(4)/300]),'uint8')]);
                data = uint8([0 typecast(single(joyAxes(1)),'uint8')]);
            else
%                 data = uint8([obj.id typecast(single([0 0 0 0]),'uint8')]);
                data = uint8([0 typecast(single(0),'uint8')]);
            end
        end
        
        function [type, data] = calculateJoyVehicleGainsCmd(obj)
            type = 'g';
            data = typecast(int16([obj.gains.kpxy,obj.gains.kdxy,obj.gains.kpz,obj.gains.kdz,]),'uint8');
        end
        
        function [type, data] = calibrateGyro(obj)
            type = char(8);
            data = typecast(2,'uint8');
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
            
            if joyButtons(4) == 1 + obj.joyButtonsOld(4)
                [tempType, tempData] = obj.calibrateGyro();
                type = [tempType;type];
                data = [tempData;data];
                out(obj.verbose,'Calibrating gyro')
            end
            
            if obj.stateVicon.enabled
                [tempType, tempData] = obj.calculateJoyViconRelPosCmd(joyAxes);
                type = [tempType;type];
                data = [tempData;data];
            else
                [tempType, tempData] = obj.calculateJoyVehicleOrientationCmd(joyAxes);
                type = [tempType;type];
                data = [tempData;data];
            end
            if sendGains == true
                [tempType, tempData] = obj.calculateJoyVehicleGainsCmd();
                type = [tempType;type];
                data = [tempData;data];
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

