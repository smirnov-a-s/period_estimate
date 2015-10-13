#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <math.h>

#include "utils.h"

#define PERMS 0666

void write_plot_data(const char* fn, float* data, int size)
{
	int fd;

	if ((fd = creat(fn, PERMS)) == -1)
		printf("Can't create output file.\n");

	FILE* filePtr;
	filePtr = fopen(fn, "w");

	int i;
	for (i = 0; i < size; i++) {
		fprintf(filePtr, "%.10f\n", data[i]);
	}

	fclose(filePtr);
}

/* generates the sin wave of specified freq for some testing */
void generate_sin(int samplerate, float freq, float* sig, int size)
{
	float phase = 0.0f;
	float two_pi = 2.0f * M_PI;
	float phase_inc = two_pi * ((float) freq / (float) samplerate);

	for (int i = 0; i < size; i++) {
		sig[i] = sinf(phase);
		phase += phase_inc;
		if (phase >= two_pi)
			phase -= two_pi;
	}
}

int* read_int_bin_data(const char* file_name, int* num_elems)
{
	FILE* file = fopen(file_name, "r");
	if (file == NULL) {
		printf("Can't open file.\n");
		exit(EXIT_FAILURE);
	}

	int fd = fileno(file);
	if (fd == -1) {
		printf("Error getting descriptor.\n");
		exit(EXIT_FAILURE);
	}

	struct stat st;
	fstat(fd, &st);
	unsigned int sz = st.st_size;
	unsigned int elems_num = sz / sizeof(int);

	printf("File size: %d\n", sz);
	printf("Num elems: %d\n", elems_num);

	int* buf = (int*) malloc(sz);

	size_t elems_read = fread(buf, sizeof(int), elems_num, file);
	printf("Elems read: %d\n", elems_read);

	fclose(file);

	if (elems_num != elems_read) {
		printf("Error reading all data.\n");
		exit(EXIT_FAILURE);
	}

	*num_elems = elems_num;
	for (int i = 0; i < elems_read; i++) {
		printf("%d\n", buf[i]);
	}

	return buf;
}
