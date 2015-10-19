#ifndef _NCCF_H_
#define _NCCF_H_

typedef struct {
	int samplerate;
	int llag; /* longets lag in samples */
	int n; /* correlation win size in samples */
	float* nccf; /* result buffer */
} NccfData;

/* w - corr window in sec
 * tlag - max lag in sec
 */
void init_nccf(NccfData* nccf, int samplerate, float w, float tlag);
/* s - input signal */
void process_nccf(NccfData* nccf, float* s);
void free_nccf(NccfData* nccf);
void estimate_freq(NccfData* nccf, float threshold);

#endif
