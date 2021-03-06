#!/usr/bin/octave -qf

fid = fopen("./dump/main_input_norm_float_bin.dat","r");
x = fread(fid, Inf, "float32", 0);
fclose(fid);

len = length(x)
figure(1)
plot(x)
grid on;

Fs = 200;
N = 128;
spect = fft(x, N);
spect = spect / N;
spect = abs(spect);

##
f_axis = Fs/2 * linspace(0, 1, N/2 + 1);

printf("f_axis len = %d\n", length(f_axis));
printf("delta f    = %f\n", Fs / N);

figure(2);
plot(f_axis, spect(1 : length(f_axis)));
grid on;

set(gca,'xtick',[0 : 1 : 100])

spect_db = 0;
for i = 2 : length(spect)
  spect_db(i) = 10 * log10(spect(i)/spect(1));
endfor

spect(1) = 0;

figure(3);
plot(f_axis, spect_db(1 : length(f_axis)));
grid on;

set(gca,'xtick',[0 : 1 : 100])

input("");
