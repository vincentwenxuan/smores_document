function [Data,RC] = read_rc()
radio = serial('/dev/tty.usbserial-A6009Ezc','BaudRate',115200);
finishup = onCleanup(@() fclose(radio));

% start roll  flt_mode  pitch    yaw   throttle  gear    aux2
% 0301  05FE    1752     09EE    0DFF    009F    1356    1B07

radio.InputBufferSize = 16;

fopen(radio);

N=inf;
%%
for i=1:N
    A = fread(radio,16)';
    
    start = strfind(A, [3 1]);
    
    if ~isempty(start)
        A = [A(start:end),A(1:start-1)];
    else
        if A(1)==1 && A(end)==3
            A = [A(end),A(1:end-1)];
        end
    end
    
    Data.i = i;
    Data.raw      = A;
    Data.error    = A(1);
    Data.bind_typ = A(2);
    Data.roll     = A(3) * 256 + A(4) - 1 * 1024;
    Data.flt_mode = A(5) * 256 + A(6) - 5 * 1024;
    Data.pitch    = A(7) * 256 + A(8) - 2 * 1024;
    Data.yaw      = A(9) * 256 + A(10) - 3 * 1024;
    Data.throttle = A(11) * 256 + A(12) - 0 * 1024;
    Data.gear     = A(13) * 256 + A(14) - 4 * 1024;
    Data.aux2     = A(15) * 256 + A(16) - 6 * 1024;
    
    %% Value Mapping
    
    final = [-1000 1000];
    
    roll = [168 856];
    RC.roll = (Data.roll-roll(1))/(roll(2)-roll(1)) * (final(2)-final(1)) + final(1);
    
    pitch = [170 853];
    RC.pitch = (Data.pitch-pitch(1))/(pitch(2)-pitch(1)) * (final(2)-final(1)) + final(1);
    
    yaw = [163 854];
    RC.yaw = (Data.yaw-yaw(1))/(yaw(2)-yaw(1)) * (final(2)-final(1)) + final(1);
    
    throttle = [158 856];
    RC.throttle = (Data.throttle-throttle(1))/(throttle(2)-throttle(1)) * (final(2)-final(1)) + final(1);
    
    
    if Data.gear > 500
        RC.gear = 0;
    else
        RC.gear = 1;
    end
    
    if Data.flt_mode > 800
        RC.flt_mode = 0;
    elseif Data.flt_mode > 300 && Data.flt_mode < 700
        RC.flt_mode = 1;
    else
        RC.flt_mode = 2;
    end
    
    if Data.aux2 > 500
        RC.aux2 = 0;
    else
        RC.aux2 = 1;
    end
    
end
end