function [log] = joyVehicle_ID_v2(varargin)
% joyVehicle controls a vehicle.
% A vehicleObject should be passed in.  The user will be prompted to load 
%   one if one is not specified.

    try
        %% Extract values from varargin
        try
        	verbose     = getArgVal('verbose', varargin, false); % Boolean determines if the user receives updates via text on the console
        catch err
            verbose = false;
            if strcmp(err.identifier,'MATLAB:UndefinedFunction')
                error('Cannot find getArgVal in the path.  It might be at http://svn.modlabupenn.org/libraries/Matlab_Utilities (rev 67). Find the directory it is in and use addpath() to make it visible.')
            end
        end
        comPort = getArgVal('comPort', varargin, []);
        viconHost = getArgVal('viconHost', varargin, 'alkaline');
        viconVehicle = getArgVal('viconVehicle', varargin, []);
        joyDeadzone = getArgVal('joyDeadzone', varargin, .1);
        joySensitivity = getArgVal('joySensitivity', varargin, [1 1 1 1 1]);
        vehicleObject = getArgVal('vehicleObject',varargin, []);
        doLog = getArgVal('doLog',varargin,true);
        %% Load vehicle object
        if isempty(vehicleObject)
            vehicleObject = input('Type the path to the vehicle object, then press Enter: ');
        end
        if ischar(vehicleObject)
            vehicleObject = load(vehicleObject);
        end
        %% Enable serial
        if isempty(comPort)
            comPortNumString = input('Please enter the com port number, then press Enter: ');
            if isunix
                comPort = ['ttyS' comPortNumString];
            else
                comPort = ['COM' comPortNumString];
            end
        end
        serialHandle = initializeSerial(comPort);
        %% Enable joystick
        joyHandle = vrjoystick(1);
        %% Check for Vicon
        if ~isempty(viconVehicle) && ~isunix()
            [viconObject, viconConnected] = initializeVicon(viconHost);
        else
            viconObject = [];
            viconConnected = false;
        end
        %% Generate cleanup
        cleanupObj = onCleanup(@()flyVehicleCleanup(vehicleObject, serialHandle, joyHandle, viconObject));
        %% Initialize all other variables
        serialBuffer = '';
        log = {};
        tic
        %% Main loop
        vehicleObject.activate();
        while(vehicleObject.activated)
            %% Loop initialization
            messagesTypeOut = [];
            messagesDataOut = {};
            messagesTypeIn = [];
            messagesDataIn = {};
            state = [];
            messagesOut = '';
            tCurrent = toc;
            %% Get and deal with joystick data
            % Get joystick data
            [joyAxes, joyButtons, joyPOVs] = read(joyHandle);
            % Ignore axes in deadzone
            joyAxes = (abs(joyAxes) > joyDeadzone).*(joyAxes - sign(joyAxes).*joyDeadzone)./(1-joyDeadzone).*joySensitivity;
            %% Get vicon data if enabled
            if viconConnected && vehicleObject.stateVicon.enabled
                state.stateVicon = getViconState(viconObject, viconVehicle);
            end
            %% Generate commands
            state.time = tCurrent;
            state.joyAxes = joyAxes;
            state.joyButtons = joyButtons;
            state.joyPOVs = joyPOVs;
            [messagesTypeTemp, messagesDataTemp] = vehicleObject.processInputs(state);
            messagesTypeOut = [messagesTypeOut; messagesTypeTemp];
            messagesDataOut = cat(1,messagesDataOut, messagesDataTemp);
            %% Send messages
            for i = 1:length(messagesTypeOut)
                messagesOut = [messagesOut; encodeSerial_pico(messagesTypeOut(i),messagesDataOut{i})];
            end
            % disp(num2str(uint8(messagesOut'),'%x'));
            fwrite(serialHandle,messagesOut);
            state.messagesTypeOut = messagesTypeOut;
            state.messagesDataOut = messagesDataOut;
            %% Get and parse messages
            bytesAvailable = serialHandle.BytesAvailable;
            if bytesAvailable
                
                serialBufferNew = fread(serialHandle,bytesAvailable);
                serialBuffer = uint8([serialBuffer; serialBufferNew]);
                [messagesTypeIn, messagesDataIn, serialBuffer] = decodeSerial(serialBuffer);
            end
            state.messagesTypeIn = messagesTypeIn;
            state.messagesDataIn = messagesDataIn;
            %% Log state
            if doLog
                log{end+1} = state;
            end
%             pause(.01);
        end
        %% Shutdown Vehicle
        vehicleObject.disable();
        [eStopMessageType, eStopMessageData] = vehicleObject.getEmergencyStopCmd();
        fwrite(serialHandle, encodeSerial_pico(eStopMessageType,eStopMessageData));
    catch mainErr
        try
            %% Call the cleanup callback
            flyVehicleCleanup
            
            warning(['Error caught:' ...
                getReport(mainErr,'extended')]);
        catch cleanupErr
            try
            %% In case the callback has not been initialized, manually kill everything
            fclose(serialHandle);
            close(joyHandle);
            if ~isempty(viconObject)
                viconObject.Disconnect();
            end

            warning(['Multiple errors caught:' ...
                getReport(cleanupErr,'extended') ...
                getReport(mainErr,'extended')]);
            catch manualCleanupErr
                warning(['Multiple errors caught:' ...
                    getReport(manualCleanupErr,'extended') ...
                    getReport(cleanupErr,'extended') ...
                    getReport(mainErr,'extended')]);
            end
        end
    end
        
    %% Cleanup function
    %Call me when you're done with this function
    function flyVehicleCleanup(vehicleObject, serialHandle, joyHandle, viconObject)
        [eStopMessageType, eStopMessageData] = vehicleObject.getEmergencyStopCmd();
        fwrite(serialHandle, encodeSerial_pico(eStopMessageType,eStopMessageData));
        pause(.1);
        fclose(serialHandle);
        close(joyHandle);
        if ~isempty(viconObject)
            viconObject.Disconnect();
        end
        disp('Cleaned up flyVehicle');
    end
end % function log = flyVehicle(varargin)