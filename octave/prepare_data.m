#!/usr/bin/octave -qf

x = load("./dump/sig.dump");
len = length(x)

x_no_dc = x .- mean(x);

plot(x)
hold on;
plot(x_no_dc)
grid on;

fid = fopen("./dump/sig_bin.dump","w");
count = fwrite(fid, x_no_dc, "int32")

input("");
