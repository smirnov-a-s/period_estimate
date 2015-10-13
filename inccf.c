#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "inccf.h"

/* check for pow2 */
int is_power_of_two(unsigned int x)
{
	/* while x is even and > 1 */
	while (((x % 2) == 0) && x > 1)
		x /= 2;
	return (x == 1);
}

int fast_is_power_of_two(unsigned int x)
{
	return x && !(x & (x - 1));
}

/* searches nearest power of two */
/* int nearest_pow_two(int x) */
/* { */
/*         if (!is_power_of_two(x)) { */
/*                 /\* even *\/ */
/*                 if ((x % 2) == 0) { */
/*                         while (!is_power_of_two(x)) */
/*                                 x += 2; */
/*                 } */

/*                 while (!is_power_of_two(x)) */
/*                         x += 1; */
/*         } */

/*         return x; */
/* } */

/* search nearest power of 2 (for x >=0) */
int nearest_pow_two(int x)
{
	/* odd */
	if ((x % 2) != 0)
		x++;

	while (!is_power_of_two(x))
		x += 2;

	return x;
}


int init_inccf(InccfData* inccf, int samplerate, int fmin, int fmax, float w, float tlag)
{
	int res = 0;  /* all fine by default */

	if (fmin < 0 || fmax < 0 || samplerate < 0) {
		printf("ERROR in INCCF_Init() ! Invalid input frequencies !\n");
		res = 1;
	}
	if (fmin > fmax) {
		printf("ERROR in INCCF_Init() ! Minimum f0 is larger then maximum f0 !\n");
		res = 2;
	}
	if (fmax > samplerate || fmin > samplerate) {
		printf("ERROR in INCCF_Init() ! F0 boarders are bigger then samplerate ! \n");
		res = 3;
	}

	inccf->samplerate = samplerate;
	/* min F0 */
	inccf->fmin = fmin;
	/* max F0 */
	inccf->fmax = fmax;
	/* correlation win size in sec */
	inccf->w = w;
	/* correlation win size in samples */
	inccf->n = inccf->w * inccf->samplerate;
	/* correlation max lag in samples */
	inccf->llag = tlag * inccf->samplerate;

	/* init INCCF buf */
	/* NOTE: check for NULL */
	inccf->nccf = (float*) calloc(inccf->llag, sizeof(float));
	if (inccf->nccf == NULL) {
		printf("ERROR in INCCF_Init() ! F0 boarders are bigger then samplerate ! \n");
		return -1;
	}

	inccf->fmax_lag = samplerate / fmax;
	inccf->fmin_lag = samplerate / fmin;
	if (inccf->fmin_lag > inccf->llag) {
		printf("ERROR in INCCF_Init() ! Specified time lag is smaller then needed for specified min f0 !\n");
		res = 4;
	}

	/* fft nccf stuff */
	inccf->fft_size = nearest_pow_two(inccf->n + inccf->llag);
	/* printf("fft_size = %d\n", inccf->fft_size); */

	/* NOTE: check for NULL? */
	inccf->buf1 = (fftwf_complex *) fftwf_malloc(inccf->fft_size * sizeof(fftwf_complex));
	inccf->buf1_out = (fftwf_complex *) fftwf_malloc(inccf->fft_size * sizeof(fftwf_complex));

	inccf->buf2 = (fftwf_complex *) fftwf_malloc(inccf->fft_size * sizeof(fftwf_complex));
	inccf->buf2_out = (fftwf_complex *) fftwf_malloc(inccf->fft_size * sizeof(fftwf_complex));

	inccf->p_buf1_fwd = fftwf_plan_dft_1d(inccf->fft_size, inccf->buf1, inccf->buf1_out, FFTW_FORWARD, FFTW_ESTIMATE);
	inccf->p_buf2_fwd = fftwf_plan_dft_1d(inccf->fft_size, inccf->buf2, inccf->buf2_out, FFTW_FORWARD, FFTW_ESTIMATE);
	inccf->p_ccf_inv = fftwf_plan_dft_1d(inccf->fft_size, inccf->buf1, inccf->buf2, FFTW_BACKWARD, FFTW_ESTIMATE);

	return res;
}

void process_inccf(InccfData* inccf, float* s)
{
	/* correlation */
	float corr = 0.0f;
	/* power at zero lag*/
	float pwr = 0.0f;
	/* power at k'th lag */
	float pwr_k = 0.0f;
	/* mean value of processed signal */
	float mean = 0.0f;

	int j, k;

	/* calculating mean value of data, that will be processed */
	for (j = 0; j < (inccf->n + inccf->llag); ++j)	/* processed data length = corr_wnd_len + longest_lag */
		mean += s[j];
	mean = mean / (inccf->n + inccf->llag); /* this mean value will be substracted from each sample during calculation of INCCF */

#ifdef DBG
	printf("in INCCF_Process():: mean = %lf ...\n", mean);
#endif

	pwr = 0.0f;
	/* power of the signal in correlation window at zero lag */
	for (j = 0; j < inccf->n; j++)
		pwr += (s[j] - mean) * (s[j] - mean);

	/* power of the signal in correlation window at k'th lag */
	pwr_k = pwr; /* equals to 'pwr' at zero lag */

	/* first nccf value for k = 0 lag */
	inccf->nccf[0] = 1.0;

	/* start from 1 lag */
	for (k = 1; k < inccf->llag; k++) {
		corr = 0.0;
		/* correlation for lag k */
		for (j = 0; j < inccf->n; j++)
			corr += (s[j] - mean) * (s[j + k] - mean);

		/* modify pwr_k */
		pwr_k -= ( (s[k - 1] - mean) * (s[k - 1] - mean)  );
		pwr_k += ( (s[k + inccf->n ] - mean) * (s[k + inccf->n ] - mean)  );

		/* nccf */
		inccf->nccf[k] =  (corr / sqrt(pwr * pwr_k) );
	}
}

void process_fft_inccf(InccfData* inccf, float* s)
{
	/* power at zero lag*/
	float pwr = 0.0f;
	/* power at k'th lag */
	float pwr_k = 0.0f;
	/* mean value of processed signal */
	float mean = 0.0f;

	int i;
	/* calculating mean value of data, that will be processed
		 * processed data length = n + llag
		 */
	int proc_size = inccf->n + inccf->llag;
	for (i = 0; i < proc_size; i++)
		mean += s[i];
	/* this mean value will be substracted from each sample during calculation of INCCF */
	mean = mean / (float) proc_size;

	/* substract mean and calc power of the signal in the correlation window at zero lag*/
	for (i = 0; i < proc_size; i++) {
		s[i] -= mean;
		if (i < inccf->n) {
			pwr += s[i] * s[i];
		}
	}

	/* copy signal */
	for (i = 0; i < inccf->n; i++) {
		inccf->buf1[i][0] = s[i];
		inccf->buf1[i][1] = 0.0f;
	}
	/* zero pad */
	for (i = inccf->n; i < inccf->fft_size; i++) {
		inccf->buf1[i][0] = 0.0f;
		inccf->buf1[i][1] = 0.0f;
	}
	/* copy signal */
	for (i = 0; i < proc_size; i++) {
		inccf->buf2[i][0] = s[i];
		inccf->buf2[i][1] = 0.0f;
	}
	/* zero pad (zeroes will be added if llag > n) */
	for (i = proc_size; i < inccf->fft_size; i++) {
		inccf->buf2[i][0] = 0.0f;
		inccf->buf2[i][1] = 0.0f;
	}

	/* exec fftw and get spectrum */
	fftwf_execute(inccf->p_buf1_fwd);
	fftwf_execute(inccf->p_buf2_fwd);

	/* correlation */
	for (i = 0; i < inccf->fft_size; i++) {
		inccf->buf1[i][0] = inccf->buf1_out[i][0] * inccf->buf2_out[i][0] + inccf->buf1_out[i][1] * inccf->buf2_out[i][1];
		inccf->buf1[i][1] = inccf->buf1_out[i][0] * inccf->buf2_out[i][1] - inccf->buf1_out[i][1] * inccf->buf2_out[i][0];

		inccf->buf1[i][0] /= (float) inccf->fft_size;
		inccf->buf1[i][1] /= (float) inccf->fft_size;
	}

	fftwf_execute(inccf->p_ccf_inv);

	/* power of the signal in correlation window at k'th lag */
	pwr_k = pwr; /* equals to 'pwr' at zero lag */

	/* NOTE: set 1st to 1.0f? */
	/* first nccf value for k = 0 lag */
	inccf->nccf[0] = 1.0;
	for (i = 1; i < inccf->llag; i++) {

		inccf->buf2[i][0] /= sqrt(pwr * pwr_k);

		pwr_k -= s[i] * s[i];
		pwr_k += s[i + inccf->n] * s[i + inccf->n];

		inccf->nccf[i] = inccf->buf2[i][0];
	}
}

void free_inccf(InccfData* inccf)
{
	if(inccf->nccf != NULL) {
		fftwf_destroy_plan(inccf->p_buf1_fwd);
		fftwf_destroy_plan(inccf->p_buf2_fwd);
		fftwf_destroy_plan(inccf->p_ccf_inv);

		fftwf_free(inccf->buf1);
		fftwf_free(inccf->buf2);
		fftwf_free(inccf->buf1_out);
		fftwf_free(inccf->buf2_out);

		fftwf_cleanup();

		free(inccf->nccf);
	}
}

int search_candidates_inccf(InccfData* inccf, float threshold, int max_cand, float* out_corrs, int* out_lags)
{
	int result_idx = 0; /* by default */

#ifdef DBG
	printf("In INCCF_search_candidates() ...\n");
#endif

	float local_max = 0.0f;
	int local_max_idx = 0;
	float* nccf = inccf->nccf; /* just to ease syntax */
	int idx;

	/*
		 * Trace whole nccf array in range, calculated from desired f0min/f0max during INCCF initialization.
		 * What we basically do - we look for areas where nccf value exceeds THRESHOLD. When such area is found -
		 * we find local maximum of such area and name it a candidate. All candidates found in such a simple way
		 * will be put into out_corrs/out_lags arrays
		 */
	int first_thr_cross = 0;

	/* finding first crossing of threshold, first_thr_cross will contain index of first nccf value that is less then threshold */
	while (nccf[++first_thr_cross] >= threshold);

	/* start actual search as far from 0 lag as possible */
	int search_start_idx = first_thr_cross > inccf->fmax_lag ? first_thr_cross : inccf->fmax_lag;

	for (idx = search_start_idx; idx <= inccf->fmin_lag; ++idx) {
		if (nccf[idx] >= threshold) {
			/* INCCF value broke the threshold - entering area of search */
			if (result_idx > max_cand - 1) {  /* -1 cause we're indexing an array with 'result_idx' */
				/*
								 * we allready found as much as MAX_CAND candidates and still smth remains -
								 * that seems like erroneous signal which is not supposed to be processed by DP module.
								 */
				return 0;
			}
			int local_idx = idx;
			local_max = nccf[local_idx];
			local_max_idx = local_idx;
			/* search for maximum value until nccf value is bigger then THRESHOLD */
			while (nccf[++local_idx] >= threshold) {
				if (nccf[local_idx] >= local_max) {
					local_max = nccf[local_idx];
					local_max_idx = local_idx;

				}
			}
			idx = local_idx;
			out_corrs[result_idx] = local_max;
			out_lags[result_idx++] = local_max_idx;
#ifdef DBG
			printf("local_max=%f   local_max_idx=%d   nccf[idx]=%f ...(span %d:%d)\n", local_max, local_max_idx, nccf[local_max_idx], inccf->fmax_lag, inccf->fmin_lag);
#endif

		}
	}

	return result_idx;
}

float get_optimal_time_step_inccf(unsigned long samplerate)
{
	if (samplerate <= 8000)
		return 0.016f;
	else if (samplerate == 32000)
		return 0.016f;
	else if (samplerate == 44100)
		return 0.011610f;
	else
		return 0.010f;
	/* and then pray it will work - did not test it on
		 * other samplerates except the ones above
		 */
	return 0.0f;
}
