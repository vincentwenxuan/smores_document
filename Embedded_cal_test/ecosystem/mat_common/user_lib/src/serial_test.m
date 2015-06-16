function serial_test()
radio = serial('COM9','BaudRate',115200);
finishup = onCleanup(@() fclose(radio));

radio.InputBufferSize = 16;

fopen(radio);

while(1)
    fread(radio,16)'
end
end