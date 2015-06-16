%% STM32F303 MOTOR CONTROL VIA TCP SOCKET
clc
clear all

%setup plotting
h1 = figure(1);
%setup a function to be called when a keyboard key is pressed
set(h1, 'KeyPressFcn', @(h,evt) MyKeyPress_Callback(evt.Key) );
%keyboard interrupt
global key;
key = -1;

t=tcpip('192.168.10.118', 2222, 'NetworkRole', 'client');
fopen(t);

JoyMEX('init', 1);

y_initial = 0;
ry_initial = 0;
index = 1;
deadband=0.1;
period = 21;
offset = 22;

while key ~= 'q'
    figure(1);
    %sticks(1)=horizontal direction of left stick
    %sticks(2)=vertical direction of left stick
    %sticks(3)=horizontal direction of right stick
    %sticks(6)=vertical direction of right stick
    [sticks, buttons] = JoyMEX(1);
    if sticks(2)<deadband && sticks(2)>-deadband
        sticks(2)=0;
    end
    
    if sticks(6)<deadband && sticks(6)>-deadband
        sticks(6)=0;
    end
    
    sticks(2)=sticks(2) .* period;
    y = int8(round(y_initial + sticks(2)));
    
    sticks(6)=sticks(6) .* period;
    if sticks(6)>-1
        sticks(6)=sticks(6)+offset;
    else
        sticks(6)=sticks(6)-offset;
    end
    ry = int8(round(ry_initial + sticks(6)));
    
    

    y = y+43;
    ry = ry+43;
    
  
    try
        fwrite(t, y);
        pause(0.005);

        fwrite(t,ry);
        pause(0.005);
    end
 
    if(t.BytesAvailable)
        data = fread(t, t.BytesAvailable)
    end
end

fclose(t);
clear JoyMEX
close all;
    
    