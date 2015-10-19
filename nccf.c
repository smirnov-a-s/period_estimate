#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "nccf.h"

void init_nccf(NccfData* nccf, int samplerate, float w, float tlag)
{
	if (w <= 0.0f) {
		printf("wrong corr window\n");
		exit(EXIT_FAILURE);
	}
	if ((tlag <= 0.0f) || (tlag <= w)) {
		printf("wrong longest lag\n");
		exit(EXIT_FAILURE);
	}
	if (samplerate <= 0) {
		printf("wrong samplerate\n");
		exit(EXIT_FAILURE);
	}
	nccf->samplerate = samplerate;
	/* correlation win size in samples */
	nccf->n = w * nccf->samplerate;
	/* correlation max lag in samples */
	nccf->llag = tlag * nccf->samplerate;
	/* init buf */
	nccf->nccf = (float*) calloc(nccf->llag, sizeof(float));
}

void process_nccf(NccfData* nccf, float* s)
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
	for (j = 0; j < (nccf->n + nccf->llag); ++j)	/* processed data length = corr_wnd_len + longest_lag */
		mean += s[j];
	mean = mean / (nccf->n + nccf->llag); /* this mean value will be substracted from each sample during calculation of INCCF */

	pwr = 0.0f;
	/* power of the signal in correlation window at zero lag */
	for (j = 0; j < nccf->n; j++)
		pwr += (s[j] - mean) * (s[j] - mean);

	/* power of the signal in correlation window at k'th lag */
	pwr_k = pwr; /* equals to 'pwr' at zero lag */

	/* first nccf value for k = 0 lag */
	nccf->nccf[0] = 1.0;

	/* start from 1 lag */
	for (k = 1; k < nccf->llag; k++) {
		corr = 0.0;
		/* correlation for lag k */
		for (j = 0; j < nccf->n; j++)
			corr += (s[j] - mean) * (s[j + k] - mean);

		/* modify pwr_k */
		pwr_k -= ( (s[k - 1] - mean) * (s[k - 1] - mean)  );
		pwr_k += ( (s[k + nccf->n ] - mean) * (s[k + nccf->n ] - mean)  );

		/* nccf */
		nccf->nccf[k] =  (corr / sqrt(pwr * pwr_k) );
	}
}

void free_nccf(NccfData* nccf)
{
	if(nccf->nccf != NULL) {
		free(nccf->nccf);
	}
}

static void find_nccf_candidates(NccfData* inccf, float threshold, float *out_corrs, int* out_corrs_size)
{
	// move far from 0 lag!!!
	int out_corr_cnt = 0;
	float* nccf = inccf->nccf;
	for (int i = 0; i < inccf->llag; i++) {
		if (nccf[i] >= threshold) {
			int j = i;
			float local_max = 0.0f;
			int lag = 0;
			while (nccf[j] >= threshold) {
				if (nccf[j] > local_max) {
					local_max = nccf[j];
					lag = j;
				}
				j++;
			}
			printf("max = %f, lag = %d\n", local_max, lag);
			out_corrs[out_corr_cnt++] = lag;
			i = j;
		}
	}
	*out_corrs_size = out_corr_cnt;
}

void estimate_freq(NccfData* nccf, float threshold)
{
	float* out_corrs = (float*) calloc(nccf->llag, sizeof(float));
	int out_corrs_size = 0;

	find_nccf_candidates(nccf, threshold, out_corrs, &out_corrs_size);

	// printf("out_corrs_size: %d\n", out_corrs_size);
	float mean_freq_val = 0.0f;
	for (int i = 0; i < out_corrs_size - 1; i++) {
		int delta = out_corrs[i + 1] - out_corrs[i];
		float freq = (float) nccf->samplerate / (float) delta;
		mean_freq_val += freq;
		printf("delta = %d, f = %f\n", delta, freq);
	}
	mean_freq_val /= ((float) (out_corrs_size - 1));
	printf("\nmean freq val = %f\n", mean_freq_val);
}
