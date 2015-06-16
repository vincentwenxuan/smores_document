function [viconClient, viconConnected] = initializeVicon(viconHost)

fprintf( 'Loading SDK...' );
Client.LoadViconDataStreamSDK();
fprintf( 'done\n' ); %Load SDK
viconClient = Client();
fprintf( 'Connecting to %s ...', viconHost );
try 
    t_start = toc;
catch
    tic;
    t_start = toc;
end
while (~viconClient.IsConnected().Connected && toc - t_start < 5)
    % Direct connection
    viconClient.Connect( viconHost );
    fprintf( '.' );
end
if viconClient.IsConnected().Connected
    fprintf( 'done\n' ); %Load SDK
    viconConnected = true;
else
    fprintf( 'failed\n' );
    viconConnected = false;
    return;
end
% Enable some different data types
viconClient.EnableSegmentData();
% viconClient.EnableMarkerData();
% viconClient.EnableUnlabeledMarkerData();
% viconClient.EnableDeviceData();

fprintf( 'Segment Data Enabled: %d\n',          viconClient.IsSegmentDataEnabled().Enabled );
fprintf( 'Marker Data Enabled: %d\n',           viconClient.IsMarkerDataEnabled().Enabled );
fprintf( 'Unlabeled Marker Data Enabled: %d\n', viconClient.IsUnlabeledMarkerDataEnabled().Enabled );
fprintf( 'Device Data Enabled: %d\n',           viconClient.IsDeviceDataEnabled().Enabled );

% Set the streaming mode
viconClient.SetStreamMode( StreamMode.ClientPull );
% viconClient.SetStreamMode( StreamMode.ClientPullPreFetch );
% viconClient.SetStreamMode( StreamMode.ServerPush );

% Set the global up axis
viconClient.SetAxisMapping( Direction.Forward, ...
    Direction.Left,    ...
    Direction.Up );    % Z-up
% viconClient.SetAxisMapping( Direction.Forward, ...
%                          Direction.Up,      ...
%                          Direction.Right ); % Y-up

Output_GetAxisMapping = viconClient.GetAxisMapping();
fprintf( 'Axis Mapping: X-%s Y-%s Z-%s\n', Output_GetAxisMapping.XAxis.ToString(), ...
    Output_GetAxisMapping.YAxis.ToString(), ...
    Output_GetAxisMapping.ZAxis.ToString() );
