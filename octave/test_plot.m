#!/usr/bin/octave -qf

x = load("./dump/nccf.dat");
len = length(x)

plot(x);

## fid = fopen("./dump/sig_bin.dump","r");
## x = fread(fid, Inf, "int32", 0);
## fclose(fid);

## len = length(x)
## figure(1)
## plot(x)
## grid on;

## Fs = 200;
## N = 256;
## spect = fft(x, N);
## spect = spect / N;
## spect = abs(spect);

## ##
## f_axis = Fs/2 * linspace(0, 1, N/2 + 1);

## printf("f_axis len = %d\n", length(f_axis));
## printf("delta f    = %f\n", Fs / N);

## figure(2);
## plot(f_axis, spect(1 : length(f_axis)));
## grid on;

input("");
