try
fclose(instrfindall);
catch
end
pause(.5)
serialHandle = initializeSerial('COM6'); 
disp('I am ready to write, are you?');
pause(); 
dataCal = uint8([6 67 typecast(single(.4),'uint8') typecast(uint16(500),'uint8')]); fwrite(serialHandle,[85;append_crc(dataCal')]);
% dataPwm = uint8([4 99 typecast(single(-2.35),'uint8')]);fwrite(serialHandle,[85;append_crc(dataPwm')]);
pause(.75);
dataPwm = uint8([4 80 typecast(single(0),'uint8')]); fwrite(serialHandle,[85;append_crc(dataPwm')]); 
pause(.75); 
dataPwm = uint8([4 108 typecast(single(.00028*7),'uint8')]);fwrite(serialHandle,[85;append_crc(dataPwm')]);
pause(.75);
bytesAvailable = serialHandle.BytesAvailable;
serialBufferNew = [];
while bytesAvailable
    serialBufferNew = [serialBufferNew fread(serialHandle,bytesAvailable)'];
    bytesAvailable = serialHandle.BytesAvailable;
end
disp(cast(serialBufferNew,'char'))
