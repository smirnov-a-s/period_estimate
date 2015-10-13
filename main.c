#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include "inccf.h"

int main(int argc, char *argv[])
{
	int samplerate = 8000;

	float freq = 100.0f;
	int size = samplerate;
	float *sig1 = malloc(size * sizeof(float));
	float *sig2 = malloc(size * sizeof(float));

	/* audio data */
	/* int audio_size; */
	/* const char* fn = "../testaudio/announcer8kHz.wav"; */
	/* float *audio1 = read_audio_data(fn, &audio_size); */
	/* float *audio2 = read_audio_data(fn, &audio_size); */

	/* generate test data */
	generate_sin(samplerate, freq, sig1, size);
	generate_sin(samplerate, freq, sig2, size);

	write_plot_data("./octave/dump/sig.dat", sig1, samplerate);

	InccfData inccf;
	int fmin = 50;
	int fmax = 400;
	/* correlation win size in sec */
	float w = 512.0f / (float) samplerate;
	/* correlation lag in samples */
	float tlag = 512.0f / (float) samplerate;

	init_inccf(&inccf, samplerate, fmin, fmax, w, tlag);

	printf("corr win    = %d samp\n", inccf.n);
	printf("longest lag = %d samp\n", inccf.llag);

	process_inccf(&inccf, sig1);
	/* process_inccf(&inccf, audio1); */
	write_plot_data("./octave/dump/nccf.dat", inccf.nccf, inccf.llag);

	/* process_fft_inccf(&inccf, audio2); */
	/* write_plot_data("./octave/dump/fft_nccf.dat", inccf.nccf, inccf.llag); */

	free(sig1);
	free(sig2);

	int buf_size;
	int* data = read_int_bin_data("./octave/dump/sig_bin.dump", &buf_size);
	printf("%d\n", buf_size);

	free(data);

	return 0;
}
