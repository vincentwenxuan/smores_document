function filteredData = notchFilter(notch_hz, sample_hz,data)
qualityFactor = 2;
wo = notch_hz/(sample_hz/2);
bw = wo/qualityFactor;

% [b,a] = iirnotch(wo,bw);
% 
% filteredData = filter(b,a,data);

% fvtool(b,a);  % show filter

d  = fdesign.notch('N,F0,BW',10,wo,bw);
hd = design(d);
filteredData = filter(hd,data);
% fvtool(hd);