#include <assert.h>
#include "OsakanaFpFft.h"
#include "OsakanaFftUtil.h"
#if defined(USE_HARDCORD_TABLE)
#include "twiddletable.h"
#include "bitreversetable.h"
#else
#include "OsakanaFftUtil.h"
#endif

// mbed has memset() in mbed.h
#ifdef __MBED__
#include <mbed.h>
// mbed doesn't have M_PI
#ifndef M_PI
#define M_PI           3.14159265358979323846f
#endif
#endif

struct _OsakanaFpFftContext_t {
	int N;		// num of samples
	int log2N;	// log2(N)

#if defined(USE_HARDCORD_TABLE)
	// twiddle factor table
	const fp_complex_t* twiddles;
	// bit reverse index table
	const uint16_t* bitReverseIndexTable;
#else
	fp_complex_t* twiddles;
	uint16_t* bitReverseIndexTable;
#endif

};

// W^n_N = exp(-i2pin/N)
// = cos(2 pi n/N) - isin(2 pi n/N)
static inline fp_complex_t twiddle(int n, int Nin)
{
	float theta = (float)(2.0f*M_PI*n / Nin);
	fp_complex_t ret;
	ret.re = Float2Fp((float)cos(theta));
	ret.im = Float2Fp((float)-sin(theta));

	return ret;
}

int InitOsakanaFpFft(OsakanaFpFftContext_t** pctx, int N, int log2N)
{
	int ret = 0;
	OsakanaFpFftContext_t* ctx = (OsakanaFpFftContext_t*)malloc(sizeof(OsakanaFpFftContext_t));
	if (ctx == NULL) {
		return -1;
	}

	memset(ctx, 0, sizeof(OsakanaFpFftContext_t));
	ctx->N = N;
	ctx->log2N = log2N;

#if defined(USE_HARDCORD_TABLE)
	ctx->twiddles = s_twiddlesFp[log2N-1];
	ctx->bitReverseIndexTable = s_bitReverseTable[log2N-1];
#else
	ctx->twiddles = (fp_complex_t*)malloc(sizeof(fp_complex_t) * N/2);
	if (ctx->twiddles == NULL) {
		ret = -3;
		goto exit_error;
	}	printf("malloc items %d\n", sizeof(fp_complex_t) * N / 2);// debug
	for (int j = 0; j < N/2; j++) {
		ctx->twiddles[j] = twiddle(j, N);
	}

	ctx->bitReverseIndexTable = (uint16_t*)malloc(sizeof(uint16_t) * N);
	if (ctx->bitReverseIndexTable == NULL) {
		ret = -4;
		goto exit_error;
	}printf("malloc reverse %d\n", sizeof(fp_complex_t) * N);// debug
	for (int i = 0; i < N; i++) {
		ctx->bitReverseIndexTable[i] = (uint16_t)bitReverse(log2N, i);
	}
#endif
	*pctx = ctx;

	return 0;

exit_error:
	CleanOsakanaFpFft(ctx);
	ctx = NULL;
	*pctx = NULL;

	return ret;
}

void CleanOsakanaFpFft(OsakanaFpFftContext_t* ctx)
{
#if !defined(USE_HARDCORD_TABLE)
	free(ctx->twiddles);
	free(ctx->bitReverseIndexTable);
#endif
	ctx->twiddles = NULL;
	ctx->bitReverseIndexTable = NULL;

	free(ctx);
}

static inline void fp_butterfly(fp_complex_t* r, const fp_complex_t* tf, int idx_a, int idx_b)
{
	fp_complex_t up = r[idx_a];
	fp_complex_t dn = r[idx_b];

	//char buf[128] = { 0 };

	//r[idx_a] = up + tf * dn;
	//r[idx_b] = up - tf * dn;
	
	//fp_complex_str(&up, buf, sizeof(buf));
	//printf("up=%s\n", buf);

	//fp_complex_str(&dn, buf, sizeof(buf));
	//printf("dn=%s\n", buf);

	fp_complex_t dntf = fp_complex_mult(&dn, tf);
	//fp_complex_str(&dntf, buf, sizeof(buf));
	//printf("dntf=%s\n", buf);

	r[idx_a] = fp_complex_add(&up, &dntf);
	//fp_complex_str(&r[idx_a], buf, sizeof(buf));
	//printf("r[idx_a]=%s\n", buf);

	r[idx_b] = fp_complex_sub(&up, &dntf);
	//fp_complex_str(&r[idx_b], buf, sizeof(buf));
	//printf("r[idx_b]=%s\n", buf);
}

void OsakanaFpFft(const OsakanaFpFftContext_t* ctx, fp_complex_t* f, fp_complex_t* F)
{
	for (int i = 0; i < ctx->N; i++) {
		int ridx = ctx->bitReverseIndexTable[i];
		F[i] = f[ridx];
	}

	int dj = 2;
	int bnum = 1;
	//int tw_idx_shift = 9;// log2(N_MAX)-1
	int tw_idx_shift = ctx->log2N - 1;

	for (int i = 0; i < ctx->log2N; i++) {
		for (int j = 0; j < ctx->N; j += dj) {
			int idx_a = j;
			int idx_b = j + bnum;
			for (int k = 0; k < bnum; k++) {

				// shift to avoid overflow
				//F[idx_a] = fp_complex_r_shift(&F[idx_a], 1);
				//F[idx_b] = fp_complex_r_shift(&F[idx_b], 1);

				int tw_idx = k << tw_idx_shift;
				fp_complex_t tf = ctx->twiddles[tw_idx];

				//char buf[128] = { 0 };
				//fp_complex_str(&tf, buf, sizeof(buf));
				//printf("tf=%s, tw_idx=%d\n", buf, tw_idx);

				fp_butterfly(&F[0], &tf, idx_a++, idx_b++);
			}
		}
		dj = dj << 1;
		bnum = bnum << 1;
		tw_idx_shift--;
	}
}

void OsakanaFpIfft(const OsakanaFpFftContext_t* ctx, fp_complex_t* F, fp_complex_t* f)
{
	for (int i = 0; i < ctx->N; i++) {
		int ridx = ctx->bitReverseIndexTable[i];
		f[i] = F[ridx];
	}

	int dj = 2;
	int bnum = 1;
	int tw_idx_shift = ctx->log2N - 1;

	for (int i = 0; i < ctx->log2N; i++) {

		for (int j = 0; j < ctx->N; j += dj) {
			int idx_a = j;
			int idx_b = j + bnum;
			for (int k = 0; k < bnum; k++) {

				// div f[idx_a] by 2 instead of div by N end of func
				f[idx_a] = fp_complex_r_shift(&f[idx_a], 1);
				f[idx_b] = fp_complex_r_shift(&f[idx_b], 1);

				int tw_idx = k << tw_idx_shift;
				fp_complex_t tf = ctx->twiddles[tw_idx];
				tf.im = -tf.im;

				fp_butterfly(&f[0], &tf, idx_a, idx_b);

				idx_a++;
				idx_b++;
			}
		}
		dj = dj << 1;
		bnum = bnum << 1;
		tw_idx_shift--;
	}
}
