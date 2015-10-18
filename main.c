#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include "inccf.h"

int main(int argc, char *argv[])
{
    int samplerate = 200;

    int buf_size;
    float* data = read_float_bin_data("./octave/dump/main_input_norm_float_bin.dat", &buf_size);
    printf("buf size: %d\n", buf_size);

    float freq = 0.2f;
    int size = samplerate * 20;
    float *sig1 = malloc(size * sizeof(float));
    /* float *sig2 = malloc(size * sizeof(float)); */

    /* audio data */
    /* int audio_size; */
    /* const char* fn = "../testaudio/announcer8kHz.wav"; */
    /* float *audio1 = read_audio_data(fn, &audio_size); */
    /* float *audio2 = read_audio_data(fn, &audio_size); */

    /* generate test data */
    generate_sin(samplerate, freq, sig1, size);
    /* generate_sin(samplerate, freq, sig2, size); */

    /* write_plot_data("./octave/dump/sig.dat", sig1, samplerate); */

    InccfData inccf;
    float fmin = 0.1f;
    float fmax = 5.0f;
    /* correlation win size in sec */
    // float w = 512.0f / (float) samplerate;
    float w = 5.0f;
    /* correlation lag in sec */
    // float tlag = 512.0f / (float) samplerate;
    float tlag = 50.0f;

    init_inccf(&inccf, samplerate, fmin, fmax, w, tlag);

    printf("corr win    = %d samp\n", inccf.n);
    printf("longest lag = %d samp\n", inccf.llag);

    /* process_inccf(&inccf, audio1); */
    // process_inccf(&inccf, sig1);
    process_inccf(&inccf, data);
    write_plot_data("./octave/dump/nccf.dat", inccf.nccf, inccf.llag);

    /* process_fft_inccf(&inccf, audio2); */
    /* write_plot_data("./octave/dump/fft_nccf.dat", inccf.nccf, inccf.llag); */

    float threshold = 0.55f;
    int max_cand_num = 50;

    float* out_corrs = (float*) calloc(max_cand_num, sizeof(float));
    // int* out_lags = (int*) calloc(max_cand_num, sizeof(int));

    // search_candidates_inccf(&inccf, threshold, max_cand_num, out_corrs, out_lags);

    find_nccf_candidates(&inccf, threshold, out_corrs);

    for (int i = 0; i < inccf.llag - 1; i++) {
        int delta = out_corrs[i + 1] - out_corrs[i];
        float freq = (float) samplerate / (float) delta;
        printf("delta = %d, f = %f\n", delta, freq);
    }

    /* free(sig1); */
    /* free(sig2); */
    free(data);

    return 0;
}
