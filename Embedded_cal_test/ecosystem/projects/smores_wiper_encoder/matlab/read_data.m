%% 
% in order to use a Linux machine to talk to the microcontroller, place 
% java.opts in /usr/local/MATLAB/R2014b/bin/glnxa64(modify this directory
% according to your matlab version), modify the port value(eg: /dev/ttyACM0) 
% in the file and run sudo chmod 0777 port_number(eg.: /dev/ttyACM0)

%% init
clc
clear all
s = serial('/dev/tty.usbmodem1411');
%s.BaudRate= 9600;
%s.InputBufferSize = 512;


%% read data
fopen(s)
num_packets = 1000 * 10;
buffer = zeros(1, 60);
packets = cell(1,num_packets);

disp('wait')
while fread(s, 1, 'char') ~= 10     % wait until receive \n
end
disp('done')
for packnum = 1:num_packets
    for i = 1:60
        character = fread(s, 1, 'char');
        if(character == 10)
            break;
        end
        buffer(i) = character;
    end
    % store buffer contents as a packet:
    packets{packnum} = buffer(1:i-1);
end
disp('finish')
fclose(s)

%% delete unintegral data. I have to do this on my Linux machine. Try not to use this part first.
for i = 1:num_packets
    length_data(i) = length(packets{i});
end
diff_length = diff(length_data);
index = find(diff_length > 1);
if diff_length(end) < 0
    index = [index, num_packets];
end
packets(index) = [];

%% convert the data
for i = 1:(num_packets-length(index))
    data(i,:) = str2num(char(packets{i}));   % time_second, time_nsecond, adc_1, adc_2, encoder
end

%% plot

subplot(3,1,1);plot(data(:,3))
subplot(3,1,2);plot(data(:,4))
subplot(3,1,3);plot(data(:,5))


