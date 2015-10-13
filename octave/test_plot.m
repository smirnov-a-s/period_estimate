#!/usr/bin/octave -qf

fid = fopen("./dump/main_input_norm_float_bin.dat","r");
x = fread(fid, Inf, "float32", 0);
fclose(fid);

len = length(x)
## figure(1)
## plot(x)
## grid on;

corr_win = 256;
max_lag = 1024;

w = x(1:1024);

res = [];
stop = length(x);
i = 1;
while (i < stop - corr_win)
  win = x(i : i + corr_win - 1)';
  length(win);
  res = [res, win];
  i = i + corr_win;
endwhile

## cc = normxcorr2(x, w);

## [corr, lag] = xcorr(x, max_lag);

## plot(corr)
## plot(lag)

## plot(res);
## hold on;
## plot(x, 'r');

## input("");
