#ifndef _UTILS_H_
#define _UTILS_H_

void write_plot_data(const char* fn, float* data, int size);
float* read_float_bin_data(const char* file_name, int* num_elems);

#endif
