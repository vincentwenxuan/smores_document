%%initialize udp
%scan for module ids
addpath ../../../mat_common/user_lib/src/
addpath ../../../mat_common/user_lib/mex/
MsgDof = struct('Type', 104, 'Fields', struct('DoF', 'uint8', 'direction', 'uint8', 'speed', 'uint8'));

% system('ping -t 2 192.168.10.255; arp -a'); 
% [state, result] = system('arp -a'); 
prompt= 'Module Ip number'; 
id = input(prompt); 
UDP_send = udp(id, 2222); 
% UDP_recv = udp(self_ip, 8888); 
% fopen(UDP_recv);
fopen(UDP_send); 
pkt_out = PackMsg(MsgDof, msg);
fwrite(UDP_send,pkt_out); 
%% all of this is untest 

while(1) 
    fwrite(UDP_send,pkt_out); 
    disp('taking up time'); 
end 
