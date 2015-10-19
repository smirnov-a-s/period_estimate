#ifndef _UTILS_H_
#define _UTILS_H_

void write_plot_data(const char* fn, float* data, int size);
/* generates the sin wave of specified freq for some testing */
void generate_sin(int samplerate, float freq, float* sig, int size);

int* read_int_bin_data(const char* file_name, int* num_elems);
float* read_float_bin_data(const char* file_name, int* num_elems);

#endif
