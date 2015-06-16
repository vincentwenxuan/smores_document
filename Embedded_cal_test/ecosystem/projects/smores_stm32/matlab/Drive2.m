% Drive2.m
% Tarik Tosun, Upenn
% Created 5/6/15
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

classdef Drive2
    properties
        frontModule
        backModule
        epsilon
    end
    
    methods
        function obj = Drive2(frontModule, backModule)
            % Object constructor.  frontModule and backModule should be
            % Smores objects.
            obj.frontModule = frontModule;
            obj.backModule = backModule;
            obj.epsilon = 1; % parameter for about-equals checking, in degrees.
        end
        function neutralPosition(obj)
            % Sends the cluster to its neutral position, ready to drive.
            % tilts:
            obj.frontModule.goToAngle('tilt', 75);
            obj.backModule.goToAngle('tilt', 75);
            % pans:
            obj.frontModule.goToAngle('pan', 0);
            obj.backModule.goToAngle('pan', 0);
        end
        function drive(obj, thetaDot, theta, direction)
            % Makes the car drive with wheel angular velocity thetaDot (in
            % degrees/sec) for theta degrees.  Direction must be 'forward'
            % or 'backward'.
            %% Argument checking:
            assert(thetaDot>0, 'thetaDot must be >0');
            assert(theta>0, 'theta Must be >0');
            if( strcmp(direction,'forward') )
                dir = 1;
            elseif( strcmp(direction, 'backward') )
                dir = -1;
            else
                assert(false, 'direction must be "forward" or "backward"');
            end
            %%
            % Set velocities:
            obj.frontModule.setDofVelocity('left',thetaDot*dir);
            obj.frontModule.setDofVelocity('right',thetaDot*dir);
            obj.backModule.setDofVelocity('left', thetaDot*dir);
            obj.backModule.setDofVelocity('right',thetaDot*dir);
            % start wheels moving:
            obj.frontModule.moveDofVelocity('left');
            obj.frontModule.moveDofVelocity('right');
            obj.backModule.moveDofVelocity('left');
            obj.backModule.moveDofVelocity('right');
            % wait for it to get there:
            obj.waitUntilPositionReached(obj.front_module, 'left', current_angle + theta*dir);
            % stop all wheels
            obj.stopAllWheels(); 
        end
        function turn(obj, theta)
            % turns the steering column to specified angle (in degrees)
            obj.frontModule.goToAngle('pan', theta, 1);
            obj.backModule.goToAngle('pan', theta, 1);
            % Wait for it to get there:
            obj.waitUntilPositionReached(obj.frontModule, 'pan', theta);
        end
        %
        function stopAllWheels(obj)
            % Stops all wheels spinning
            obj.frontModule.stopVelocity('left');
            obj.frontModule.stopVelocity('right');
            obj.backModule.stopVelocity('left');
            obj.backModule.stopVelocity('right');
        end
        %
        function waitUntilPositionReached(obj, module, dof_name, targetAngleDegrees)
            % waitUntilDofPositionReached(dof_name, targetAngleDegrees): This
            % function waits (does nothing) until specified dof of 
            % specified module reaches specified angle in degrees, then returns.
            angle = module.getDofState(dof_name);
            angle = angle.current_position;
            while( abs( angle-targetAngleDegrees ) > obj.epsilon)
                angle = obj.frontModule.getDofState(dof_name);
                angle = angle.current_position;
            end
        end
    end
    
    
end