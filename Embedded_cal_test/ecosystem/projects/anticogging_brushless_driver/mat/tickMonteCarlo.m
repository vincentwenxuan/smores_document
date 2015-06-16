function numbers = tickMonteCarlo(tps,cps)
% ticks per second and commutations per second
numRands = 1000000;

% if one time unit is seconds/tick
% then commutations take tps/cps time units

starts = rand(numRands,1); % in one time unit
intervals = rand(numRands,1)*tps/cps; % locations relative to start in time units

times = starts+intervals;

for i = 1:11
    numbers(i) = sum(times < i & times >= i-1)/numRands;
end
