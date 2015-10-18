#ifndef INCCF_H
#define INCCF_H

/* #define DBG */

#include <fftw3.h>

typedef struct inccf_data {
        /* FREQUENCIES */
        int samplerate; /* guess!! */
        float fmax; /* minimal frequency to search */
        float fmin; /* minimal frequency to search */

        /* TIMES */
        float w;  /* correlation win size in sec */

        /* SAMPLES */
        int llag; /* longets lag */
        int n; /* correlation win size in samples */
        int fmax_lag; /* fmax_lag is always smaller then fmin_lag ! */
        int fmin_lag;

        /* NOTE: fft xcorr stuff starts here */
        /* xcorr size (or fft size for xcorr calculation) */
        int fft_size;
        /* buffer to store zero padded input signal */
        fftwf_complex *buf1;
        /* complex spectrum */
        fftwf_complex *buf1_out;
        /* buffer to store zero padded input signal */
        fftwf_complex *buf2;
        /* complex spectrum */
        fftwf_complex *buf2_out;
        /* fftw plans */
        fftwf_plan p_buf1_fwd;
        fftwf_plan p_buf2_fwd;
        fftwf_plan p_ccf_inv;
        /* NOTE: fft xcorr stuff ends here */

        /* BUFFER FOR RESULT */
        float* nccf;
} InccfData;

/* fmin - min f0
 * fmax - max f0
 * w - corr window in sec
 * tlag - max lag in sec
 */
int init_inccf(InccfData* inccf, int samplerate, float fmin, float fmax, float w, float tlag);
void process_inccf(InccfData* inccf, float* s);
/* fft nccf */
void process_fft_inccf(InccfData* inccf, float* s);
void free_inccf(InccfData* inccf);

void find_nccf_candidates(InccfData* inccf, float threshold, float* out_corrs);

/*
  Searches for candidates in calculated nccf sequence. Return number of found candidates.
  Returns 0 if no candidates are found or if their number is bigger then MAX_CAND param.
  Found candidates lags and their corresponding nccf values are put into arrays, handled as input params.
*/
int search_candidates_inccf(InccfData* inccf, float threshold, int max_cand, float* out_corrs, int* out_lags);
float get_optimal_time_step_inccf(unsigned long samplerate);

#endif
