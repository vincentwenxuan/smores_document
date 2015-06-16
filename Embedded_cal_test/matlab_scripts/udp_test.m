%% STM32F303 MOTOR CONTROL VIA UDP SOCKET
clc
clear all

%setup plotting
h1 = figure(1);
%setup a function to be called when a keyboard key is pressed
set(h1, 'KeyPressFcn', @(h,evt) MyKeyPress_Callback(evt.Key) );
%keyboard interrupt
global key;
key = -1;
testdata = struct('Type','int','mot0vel','uint8','mot1vel','uint8','mot0dir','uint8','mot1dir','float');
u=udp('192.168.10.101', 2222, 'LocalPort', 8888);
u2 = udp('192.168.10.210',2222);
fopen(u);
fopen(u2);
data=1;
recv_bytes=0;
while key ~= 'q'
    figure(1);

    if (u.BytesAvailable>0)

        data = uint8(fread(u, u.BytesAvailable));
        testdata.Type = typecast(data(1:4),'int32');
        if testdata.Type == 10 && length(data)>10
            testdata.mot0vel = data(5);
            testdata.mot1vel = data(6);
            testdata.mot0dir = data(7);
            testdata.mot1dir = typecast(data(8:11),'single');
            testdata
        end
        recv_bytes = recv_bytes+1;
        
    end
    %pause(0.001);

    data = round(rand()*21);
    try
        fwrite(u2, data);
        %pause(0.001);

    end
   
    
    
end

fclose(u);
fclose(u2);
close all;
    
    