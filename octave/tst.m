#!/usr/bin/octave -qf

## x = load('./dump/nccf.dat');

x = load("./dump/sig.dat");

len = length(x)

Fs = 200;

plot(x)
## grid on;

## step = ceil(50*Fs/1000);
## window = ceil(1000*Fs/1000);

## [S, f, t] = specgram(x);
## specgram(x, 2^nextpow2(window), Fs, window, window-step);

fid = fopen("sig_bin.dump","w");
count = fwrite(fid, x)

input("");
