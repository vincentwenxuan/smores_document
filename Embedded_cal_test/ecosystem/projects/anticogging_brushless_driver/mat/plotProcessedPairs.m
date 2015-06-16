function plotProcessedPairs(varargin)
minFrequency = 1100;
maxFrequency = 80000;

if mod(nargin,2) == 1
    error('You must input pairs of processed cogging compare data');
end

figure(1);
clf;
hold all;
for i = 1:nargin;
    data = varargin{i};
    inds = [data.compRes(:).frequency] > minFrequency & [data.compRes(:).frequency] < maxFrequency;
    plot([data.compRes(inds).pwmFrequency],[data.compRes(inds).stdTorque]);
    plot([data.compRes(inds).pwmFrequency],sqrt([data.compRes(inds).resTorque].^2 + [data.compRes(inds).pwmTorque].^2 + [data.compRes(inds).encTorque].^2+ [data.compRes(inds).antiTorque].^2+ [data.compRes(inds).mutualTorque].^2+ [data.compRes(inds).cogTorque].^2))
end
xlabel('PWM frequency (hz)');
ylabel('Torque STD (Nmm)');

for i = 1:nargin;
    data = varargin{i};
    inds = [data.compRes(:).frequency] > minFrequency & [data.compRes(:).frequency] < maxFrequency;
    if mod(i,2) == 0
        plot([data.compRes(inds).pwmFrequency],[data.compRes(inds).cogTorque]);
        legendString = {legendString{:},'\tau_{cog}','\tau_{nom,est}','\tau_{nom}'};
    else
        figure(1+ceil(i/2));
        clf;
        hold all;
        legendString = {'\tau_{res}', '\tau_{frq}','\tau_{mtl}','\tau_{enc}','\tau_{anti,est}','\tau_{anti}'};
        plot([data.compRes(inds).pwmFrequency],[data.compRes(inds).resTorque]);
        plot([data.compRes(inds).pwmFrequency],[data.compRes(inds).pwmTorque]);
        plot([data.compRes(inds).pwmFrequency],[data.compRes(inds).mutualTorque]);
        plot([data.compRes(inds).pwmFrequency],[data.compRes(inds).encTorque]);
    end
    plot([data.compRes(inds).pwmFrequency],sqrt([data.compRes(inds).resTorque].^2 + [data.compRes(inds).pwmTorque].^2 + [data.compRes(inds).encTorque].^2+ [data.compRes(inds).antiTorque].^2+ [data.compRes(inds).mutualTorque].^2+ [data.compRes(inds).cogTorque].^2))
    plot([data.compRes(inds).pwmFrequency],[data.compRes(inds).stdTorque]);
    legend(legendString);
    xlabel('PWM frequency (hz)');
    ylabel('Torque STD (Nmm)');
end