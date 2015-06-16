function waveErrors = calcWaveErrors(wave,ticks)
wave2 = [wave, wave];
waveErrors = zeros(1,ticks+1);
for i = 1:ticks
    waveErrors(i+1) = std(wave2(1:length(wave)) - wave2((1+i):(length(wave)+i)));
end