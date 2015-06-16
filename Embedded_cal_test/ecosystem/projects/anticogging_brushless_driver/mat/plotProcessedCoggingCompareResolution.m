function plotProcessedCoggingCompareResolution(data)

minFrequency = 1100;
maxFrequency = 80000;
inds = [data.compRes(:).frequency] > minFrequency & [data.compRes(:).frequency] < maxFrequency;

figure(1);
% clf;
hold all;
plot([data.compRes(inds).pwmFrequency],[data.compRes(inds).stdTorque]);
plot([data.compRes(inds).pwmFrequency],[data.compRes(inds).resTorque]);
plot([data.compRes(inds).pwmFrequency],[data.compRes(inds).pwmTorque]);
plot([data.compRes(inds).pwmFrequency],[data.compRes(inds).encTorque]);
plot([data.compRes(inds).pwmFrequency],[data.compRes(inds).antiTorque]);
plot([data.compRes(inds).pwmFrequency],[data.compRes(inds).mutualTorque]);
plot([data.compRes(inds).pwmFrequency],[data.compRes(inds).cogTorque]);
plot([data.compRes(inds).pwmFrequency],sqrt([data.compRes(inds).resTorque].^2 + [data.compRes(inds).pwmTorque].^2 + [data.compRes(inds).encTorque].^2+ [data.compRes(inds).antiTorque].^2+ [data.compRes(inds).mutualTorque].^2+ [data.compRes(inds).cogTorque].^2))
legend('Measured','Resolution Calculated', 'PWM Calculated','Encoder Calculated','Anti Error Calculated','Mutual Calculated','Cog Calculated','All Calculated');
xlabel('PWM frequency (hz)');
ylabel('Torque STD (Nmm)');

% figure(2);
% clf;
% hold all;
% plot([data.compRes(inds).pwmFrequency],[data.compRes(inds).meanSpeed]);
% xlabel('PWM Frequency (hz)');
% ylabel('Mean Speed (rad/s)');
% 
% figure(3);
% clf;
% hold all;
% plot([data.compRes(inds).pwmFrequency],sqrt([data.compRes(inds).stdTorque].^2 - [data.compRes(inds).resTorque].^2 - [data.compRes(inds).pwmTorque].^2 - [data.compRes(inds).encTorque].^2 - [data.compRes(inds).antiTorque].^2))
