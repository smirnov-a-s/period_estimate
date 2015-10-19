#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include "nccf.h"

#define THRESHOLD 0.55f
#define MAX_CAND_NUM 50

float* get_test_data(int samplerate)
{
	float freq = 0.2f;
	int size = samplerate * 20;
	float *sig = malloc(size * sizeof(float));

	/* generate test data */
	generate_sin(samplerate, freq, sig, size);
	write_plot_data("./octave/dump/sig.dat", sig, samplerate);

	return sig;
}

int main(int argc, char *argv[])
{
	int samplerate = 200;

	int buf_size;
	float* data = read_float_bin_data("./octave/dump/main_input_norm_float_bin.dat", &buf_size);
	printf("Samples read: %d\n", buf_size);

	NccfData nccf;
	/* correlation win size in sec */
	float w = 5.0f;
	/* correlation longest lag in sec */
	float tlag = 50.0f;

	init_nccf(&nccf, samplerate, w, tlag);

	printf("corr win    = %d samp\n", nccf.n);
	printf("longest lag = %d samp\n", nccf.llag);

	process_nccf(&nccf, data);
	write_plot_data("./octave/dump/nccf.dat", nccf.nccf, nccf.llag);

	estimate_freq(&nccf, THRESHOLD);

	free(data);

	return 0;
}
