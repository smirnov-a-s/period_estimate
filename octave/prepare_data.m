#!/usr/bin/octave -qf

x = load("./dump/main_input.dat");
len = length(x)

x_no_dc = x .- mean(x);
x_norm = x_no_dc ./ max(x);

## plot(x)
## hold on;
## plot(x_no_dc(1:length(x_no_dc) - 150))
## grid on;
plot(x_norm)

fid = fopen("./dump/main_input_norm_float_bin.dat", "w");
## count = fwrite(fid, x_no_dc, "int32")
count = fwrite(fid, x_norm, "float32")

input("");
