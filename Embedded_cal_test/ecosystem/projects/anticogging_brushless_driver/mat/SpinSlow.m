cog.Disable();
time = 4;
gain.Kp = 1;
gain.Kd = 0;
gain.Ki = 0;
res = 500;

angs = 0:2*pi/res:2*pi;
angs = mod(angs + mot.get_state().position, 2*pi);
mot.set_position_control(gain);
for i = angs
    mot.position_cmd = i;
    pause(time/res);
end

cog.Enable();
for i = angs
    mot.position_cmd = i;
    pause(time/res);
end

cog.Demo();