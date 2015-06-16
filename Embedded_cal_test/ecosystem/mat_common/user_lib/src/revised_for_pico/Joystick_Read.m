%% Script to read joystick
clear all
close all
clc

joy = vrjoystick(1,'forcefeedback');

while 1
    if strcmp(get(gcf,'currentkey'),'q'); break; end; drawnow;
    [axes, buttons, povs] = read(joy)
    
end
clear joy
