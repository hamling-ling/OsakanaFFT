#include <assert.h>
#include "OsakanaFpFft.h"
#include "OsakanaFftUtil.h"
#if defined(USE_HARDCORD_TABLE)
#include "twiddletable.h"
#else
#include "OsakanaFftUtil.h"
#endif

// mbed has memset() in mbed.h
#ifdef __MBED__
#include <mbed.h>
// mbed doesn't have M_PI
#ifndef M_PI
#define M_PI           3.14159265358979323846
#endif
#endif

struct _OsakanaFpFftContext_t {
	int N;		// num of samples
	int log2N;	// log2(N)
	// twiddle factor table
	fp_complex_t* twiddles;
	// bit reverse index table
	uint16_t* bitReverseIndexTable;
};

// W^n_N = exp(-i2pin/N)
// = cos(2 pi n/N) - isin(2 pi n/N)
static inline fp_complex_t twiddle(int n, int Nin)
{
	float theta = 2.0f*M_PI*n / Nin;
	fp_complex_t ret;
	ret.re = Float2Fp(cos(theta));
	ret.im = Float2Fp(-sin(theta));

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
	ctx->twiddles = s_twiddlesFp;
#else
	ctx->twiddles = (fp_complex_t*)malloc(sizeof(fp_complex_t) * N/2);
	if (ctx->twiddles == NULL) {
		ret = -3;
		goto exit_error;
	}	printf("malloc items %d\n", sizeof(fp_complex_t) * N / 2);// debug
	for (int j = 0; j < N/2; j++) {
		ctx->twiddles[j] = twiddle(j, N);
	}
#endif

	ctx->bitReverseIndexTable = (uint16_t*)malloc(sizeof(uint16_t) * N);
	if (ctx->bitReverseIndexTable == NULL) {
		ret = -4;
	}printf("malloc reverse %d\n", sizeof(fp_complex_t) * N);// debug
	for (int i = 0; i < N; i++) {
		ctx->bitReverseIndexTable[i] = (uint16_t)bitReverse(log2N, i);
	}
	*pctx = ctx;

	return 0;

exit_error:
#if !defined(USE_HARDCORD_TABLE)
	free(ctx->twiddles);
#endif
	ctx->twiddles = NULL;

	free(ctx->bitReverseIndexTable);
	ctx->bitReverseIndexTable = NULL;
	return ret;
}

void CleanOsakanaFpFft(OsakanaFpFftContext_t* ctx)
{
	free(ctx->twiddles);
	ctx->twiddles = NULL;

	free(ctx->bitReverseIndexTable);
	ctx->bitReverseIndexTable = NULL;

	free(ctx);
}

static inline void fp_butterfly(fp_complex_t* r, const fp_complex_t* tf, int idx_a, int idx_b)
{
	fp_complex_t up = r[idx_a];
	fp_complex_t dn = r[idx_b];

	//r[idx_a] = up + tf * dn;
	//r[idx_b] = up - tf * dn;
	fp_complex_t dntf = fp_complex_mult(&dn, tf);
	r[idx_a] = fp_complex_add(&up, &dntf);
	r[idx_b] = fp_complex_sub(&up, &dntf);
}

void OsakanaFpFft(const OsakanaFpFftContext_t* ctx, fp_complex_t* f, fp_complex_t* F)
{
	for (int i = 0; i < ctx->N; i++) {
		int ridx = ctx->bitReverseIndexTable[i];
		F[i] = f[ridx];
	}

	int dj = 2;
	int bnum = 1;
	int tw_idx_shift = ctx->log2N - 1;

	for (int i = 0; i < ctx->log2N; i++) {
		for (int j = 0; j < ctx->N; j += dj) {
			int idx_a = j;
			int idx_b = j + bnum;
			for (int k = 0; k < bnum; k++) {
				int tw_idx = k << tw_idx_shift;
				fp_complex_t tf = ctx->twiddles[tw_idx];
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
