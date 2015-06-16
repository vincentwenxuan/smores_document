pwm_cmd = .1;
time = 4;

cog.Disable();
for i = 1:10*time;
    mot.pwm_cmd = pwm_cmd; 
    pause(.1); 
end;

cog.Enable();
for i = 1:10*time;
    mot.pwm_cmd = pwm_cmd; 
    pause(.1); 
end;

cog.Demo();