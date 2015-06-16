comPort = 'COM8';
serialBuffer = uint8([]);

% Set gains to 0
testPacket1 = zeros(8,1,'uint8');
testType1 = uint8(103); % 'g'

% Set gains to defaults
testPacket2 = typecast(uint16([900; 100; 300; 150]),'uint8'); % Kp, Kd, Kp_yaw, Kd_yaw
testType2 = uint8(103); % 'g'

% Set cmds to 0 and disable motors
testPacket3 = zeros(12,1,'uint8');
testType3 = uint8(112); % 'p'

% Set cmds to vertical orientation w/ throttle and enable motors
testPacket4 = uint8(uint16([.4*10000 0 0 0 0 1 1]));
testType4 = uint8(112); % 'p'

[ packet1 ] = encodeSerial( testType1, testPacket1 );
[ packet2 ] = encodeSerial( testType2, testPacket2 );
[ packet3 ] = encodeSerial( testType3, testPacket3 );
[ packet4 ] = encodeSerial( testType4, testPacket4 );

[ serialHandle ] = initializeSerial( comPort,  921600);

logValues = [ 'time', 'desThrust', 'desRoll', 'desPitch', 'desYaw', ...
    'estRoll', 'estpitch', 'estYaw', 'estAngvelX', 'estAngvelY', 'estAngvelZ', 'estAccX', ... 
    'estAccy', 'estAccz', 'pwm1', 'pwm2', 'pwm3', 'pwm4'];
log = [];

fwrite(serialHandle,packet1);
pause(.1);
serialBuffer = [serialBuffer fread(serialHandle,serialHandle.BytesAvailable)];
[type1, data1, serialBuffer] = decodeSerial(serialBuffer);
class(data1{1})
log = [log, typecast(data1{1},'uint16')];
pause(.1);
fwrite(serialHandle,packet2);
pause(.1);
serialBuffer = [serialBuffer fread(serialHandle,serialHandle.BytesAvailable)];
[type2, data2, serialBuffer] = decodeSerial(serialBuffer);
log = [log, typecast(data2{1},'uint16')];
pause(.1);
fwrite(serialHandle,packet3);
pause(.1);
serialBuffer = [serialBuffer fread(serialHandle,serialHandle.BytesAvailable)];
[type3, data3, serialBuffer] = decodeSerial(serialBuffer);
log = [log, typecast(data3{1},'uint16')];
pause(.1);
fwrite(serialHandle,packet4);
pause(.1);
serialBuffer = [serialBuffer fread(serialHandle,serialHandle.BytesAvailable)];
[type4, data4, serialBuffer] = decodeSerial(serialBuffer);
log = [log, typecast(data4{1},'uint16')];
pause(.1);
fwrite(serialHandle,packet3);
pause(.1);
serialBuffer = [serialBuffer fread(serialHandle,serialHandle.BytesAvailable)];
[type5, data5, serialBuffer] = decodeSerial(serialBuffer);
log = [log, typecast(data5{1},'uint16')];

fclose(serialHandle);
clear serialHandle