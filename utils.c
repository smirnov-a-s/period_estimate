#include <stdio.h>
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
        filePtr = fopen(fn,"w");

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

char* get_raw_data(const char* file_name)
{
    int fd;
    if ((fd = open(file_name, O_RDONLY, 0)) == -1)
        printf("Can't open file.\n");

    struct stat st;
    fstat(fd, &st);
    unsigned int sz = st.st_size;

    printf("File size: %d\n", sz);

    /* char* buf = (char*) malloc(sz * sizeof(char)); */
    /* int n = read(fd, buf, fs); */
    /* printf("Bytes read: %d\n", n); */

    /* return buf; */
    return NULL;
}
