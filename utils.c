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

	FILE* file = fopen(fn, "w");

	int i;
	for (i = 0; i < size; i++) {
		fprintf(file, "%.10f\n", data[i]);
	}

	fclose(file);
}

float* read_float_bin_data(const char* file_name, int* num_elems)
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
	unsigned int elems_num = sz / sizeof(float);

	printf("File size: %d\n", sz);
	printf("Num elems: %d\n", elems_num);

	float* buf = (float*) malloc(sz);

	int elems_read = fread(buf, sizeof(float), elems_num, file);
	printf("Elems read: %d\n", elems_read);

	fclose(file);

	if (elems_num != elems_read) {
		printf("Error reading all data.\n");
		exit(EXIT_FAILURE);
	}

	*num_elems = elems_num;
//	for (int i = 0; i < elems_read; i++) {
//		printf("%f\n", buf[i]);
//	}

	return buf;
}
