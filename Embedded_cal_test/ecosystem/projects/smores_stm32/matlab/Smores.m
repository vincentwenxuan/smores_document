classdef Smores
    
    properties
        UsbCom
        dof_names       % DoF names and ID's
        dof_ids
        dof_name_map
        face_names      % Face names and ID's
        face_ids
        face_name_map
        magnet_state_names % magnet state names and ID's
        magnet_state_ids
        magnet_state_map
        dir_names
        dir_ids
        dir_name_map
    end
    
    methods
        function obj = Smores(com)
            % Smores(com): Constructor for Smores object.  com is string
            % specifying serial object for a Smores muscle module.  On
            % Linux, this is usually '/dev/ttyACM0' or '/dev/ttyACM1'.
            addpath ../../../mat_common/user_lib/src/
            addpath ../../../mat_common/user_lib/mex/
            addpath TrajectoryGenerator_3rd_5th_
            %obj.UsbCom = usb_com('com', '/dev/ttyACM0');
            obj.UsbCom = usb_com('com', com);
            % DoF names and ID's
            obj.dof_names = {'left', 'right', 'pan', 'tilt'};
            obj.dof_ids = [21, 22, 24, 23];
            obj.dof_name_map = containers.Map( obj.dof_names, obj.dof_ids );
            % Face names and ID's
            obj.face_names = {'left','right','top','bottom'};
            obj.face_ids = [21, 22, 23, 24]; 
            obj.face_name_map = containers.Map(obj.face_names, obj.face_ids);
            % magnet state names and ID's
            obj.magnet_state_names = {'off', 'on'};
            obj.magnet_state_ids = [0, 1];
            obj.magnet_state_map = containers.Map(obj.magnet_state_names,...
                                    obj.magnet_state_ids);
            % torque control
            obj.dir_names = {'ccw', 'cw'};
            obj.dir_ids = [1, 0];
            obj.dir_name_map = containers.Map(obj.dir_names, obj.dir_ids);
        end
        
        function data = pingFace(obj, face_name)
        % Requests that the specified face send back a message indicating
        % life.
        % Face-name-request commands are all greater than 100.
        % 121 = face 1, 122 = face 2, 123 = face 3, 124 = face 4.
        cmd = 100 + obj.face_name_map(face_name);
        data = obj.UsbCom.get_usb_data(cmd);
        % Note: I am using Chao's existing MsgCmd and MsgModule messages.
        end
        
        function magnetControl(obj, face_name, state_name)
            Mag.face = obj.face_name_map(face_name);
            Mag.state = obj.magnet_state_map(state_name);
            obj.UsbCom.send_usb_magnet(Mag);
        end
        %
        function initDof(obj, dof_name)
            % initDof(dof_name): sends the specified dof to the zero
            % position.
            obj.UsbCom.init_wheel(obj.dof_name_map(dof_name));
        end
        %
        function goToAngle(obj, dof_name, angleDegrees, duration)
           % goToAngle(dof_name, angleDegrees): Sends specified dof from
           % current position to specified angle (in degrees). 
           startPosition = obj.getDofState(dof_name);
           startPosition = double(startPosition.current_position);
           %duration = abs(mod(startPosition-angleDegrees+180,360))/speed;
           obj.moveDofPosition(dof_name, startPosition, angleDegrees,...
                                0, 0, 0, 0, duration);
        end
        function moveDofPosition(obj, dof_name, startPosition, endPosition, startVelocity, endVelocity, startAcc, endAcc, duration)
            % Sends a polynomial trajectory to module, moving specified DoF
            % with specified starting/ending positions, velocities, and
            % accelerations.  Duration specifies the time duration of the
            % trajectory.
            %% Argument checking:
            % Ensure safety for tilt dof (don't command angle out of
            % range.)
            if(obj.dof_name_map(dof_name) == 23)
                assert(startPosition<80, 'startPosition must be < 80');
                assert(startPosition>-80, 'startPosition must be > -80');
                assert(endPosition<80, 'endPosition must be < 80');
                assert(endPosition>-80, 'endPosition must be > -80');
            end
            % ensure 0 < duration < 5:
            assert(duration>0, 'duration must be > 0');
            assert(duration<5, 'duration must be < 5');
            %%
            startPosition_new = startPosition/180*32767;
            endPosition_new = endPosition/180*32767;
            [a5,a4,a3,a2,a1,a0] = createTraj5(startPosition_new,endPosition_new,startVelocity,endVelocity,startAcc,endAcc,0,duration);
            coeff = [a5,a4,a3,a2,a1,a0];
            obj.UsbCom.send_trajectory_position(obj.dof_name_map(dof_name), coeff, duration);
        end
        %
        function setDofVelocity(obj, dof_name, velocity)
            % velocity unit is degree/s
            % setDofVelocity(dof_name, velocity): Generates a 
            % velocity trajectory (polynomial) for the specified dof, which
            % will make it spin at the specified velocity.  The trajectory
            % does not actually begin until moveDofVelocity() is called.
            stateStruct = obj.getDofState(dof_name);
            cp = stateStruct.current_position;
            coeff = [velocity/180*32767, cp/180*32767];
            obj.UsbCom.send_trajectory_velocity(obj.dof_name_map(dof_name), coeff);
        end
        %
        function moveDofVelocity(obj, dof_name)
            % moveDofVelocity(dof_name): Commands the specified dof to
            % begin executing a velocity trajectory (previously specified
            % using setDofVelocity(dof_name).
            obj.UsbCom.trajectory_velocity_begin(obj.dof_name_map(dof_name));
        end
        %
        function stopDofVelocity(obj, dof_name)
            % Stops the the specified dof from spinning.
            obj.UsbCom.trajectory_velocity_stop(obj.dof_name_map(dof_name));
        end
        %
        function [stateStruct] = getDofState(obj, dof_name)
            % getDofState(dof_name): Returns a struct with fields
            % goal_position, goal_velocity, current_position,
            % current_velocity, and motor_cmd.
            pause(0.1);
            [goal_position, goal_velocity, current_position, current_velocity, motor_cmd] = obj.UsbCom.get_current_state(obj.dof_name_map(dof_name));
            stateStruct = struct();
            stateStruct.goal_position = double(goal_position)/32767*180;
            stateStruct.goal_velocity = double(goal_velocity)/32767*180;
            stateStruct.current_position = double(current_position)/32767*180;
            stateStruct.current_velocity = double(current_velocity)/32767*180;
            stateStruct.motor_cmd = motor_cmd;
        end
        function moveDofTime(obj, dof_name, direction, speed, duration)
        % moveDofTime: Moves the specified dof in direction at speed 
        % (0-100, pwm percentage) for duration (seconds)
            D.DoF = obj.dof_name_map(dof_name);
            D.speed = speed;
            D.direction = obj.dir_name_map(direction);
            obj.UsbCom.send_usb_torque(D);
            pause(duration);
            D.speed = 0;
            obj.UsbCom.send_usb_torque(D);
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
    end
end