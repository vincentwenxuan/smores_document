classdef vehicle < handle
    %vehicle is an abstract class that describes a specific vehicle
    % This class keeps track of a single vehicle's controllers and message 
    % types.  
    
    properties
        verbose
        
        enabled % Motor enable
        activated % vehicle controllable
        
        stateVicon
        stateVehicle
        
        gains
        
        joySupported
        joyMap
        joyAxisMaps
        joyButtonMaps
        joyPOVMaps
        joyAxisTrim
    end
    
    methods (Abstract)
        % constructor
        
        % destructor
        
        [type, data] = getEmergencyStopCmd(obj);
        
        parseMessage(obj, type, data);
        
        [type, data] = processInputs(obj, joyAxes, joyButtons, joyPOVs, state)

    end
    
    methods
        function obj = vehicle(varargin)
            try
                obj.verbose = getArgVal('verbose', varargin, false); % Boolean determines if the user receives updates via text on the console
            catch err
                obj.verbose = false;
                if strcmp(err.identifier,'MATLAB:UndefinedFunction')
                    error('Cannot find getArgVal in the path.  It might be at http://svn.modlabupenn.org/libraries/Matlab_Utilities (rev 67). Find the directory it is in and use addpath() to make it visible.')
                end
            end
            obj.enabled = getArgVal('enabled', varargin, false);
            obj.activated = getArgVal('activated', varargin, false);
            
            obj.stateVicon = getArgVal('stateVicon', varargin, []);
            obj.stateVehicle = getArgVal('stateVehicle', varargin, []);
            
            obj.joyMap = 1;
            obj.joyAxisTrim = getArgVal('joyAxisTrim', varargin, []);
        end
        
        function enable(obj)
            obj.enabled = true;
        end
        
        function disable(obj)
            obj.enabled = false;
        end
        
        function activate(obj)
            obj.activated = true;
        end
        
        function deactivate(obj)
            obj.activated = false;
        end
        
        function gains = getGains(obj)
            gains = obj.gains;
        end
        
        function setJoyMap(obj,newJoyMap)
            obj.joyMap = find(strcmp(newJoyMap,obj.joySupported));
        end
        
        function setGains(obj,gains)
            obj.gains = gains;
        end
    end
    
end

