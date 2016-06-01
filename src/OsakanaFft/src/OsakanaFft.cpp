//#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "OsakanaFft.h"
#include "OsakanaFftUtil.h"
#if defined(USE_HARDCORD_TABLE)
#include "bitreversetable.h"
#endif

// mbed has memset() in mbed.h
#ifdef __MBED__
#include <mbed.h>
// mbed doesn't have M_PI
#ifndef M_PI
#define M_PI           3.14159265358979323846f
#endif
#else
#include <stdlib.h>
#include <string.h>
#endif

struct _OsakanaFftContext_t {
	int N;
	int log2N;
	// twiddle factor table
	osk_complex_t* twiddles;
#if defined(USE_HARDCORD_TABLE)
	// bit reverse index table
	const osk_bitreverse_idx_pair_t* bitReverseIndexTable;
	uint16_t bitReverseIndexTableLen;
#else
	uint16_t* bitReverseIndexTable;
#endif
};

// W^n_N = exp(-i2pin/N)
// = cos(2 pi n/N) - isin(2 pi n/N)
static inline osk_complex_t twiddle(int n, int Nin)
{
	float theta = (float)(2.0f*M_PI*n / Nin);
	return MakeComplex((float)cos(theta), (float)-sin(theta));
}

int InitOsakanaFft(OsakanaFftContext_t** pctx, int N, int log2N)
{
	int ret = 0;
	OsakanaFftContext_t* ctx = (OsakanaFftContext_t*)malloc(sizeof(OsakanaFftContext_t));
	if (ctx == NULL) {
		return -1;
	}

	memset(ctx, 0, sizeof(OsakanaFftContext_t));
	ctx->N = N;
	ctx->log2N = log2N;
	ctx->twiddles = (osk_complex_t*)malloc(sizeof(osk_complex_t) * N/2);
	
	if (ctx->twiddles == NULL) {
		ret = -2;
		goto exit_error;
	}
	for (int j = 0; j < N / 2; j++) {
		ctx->twiddles[j] = twiddle(j, N);
	}

#if defined(USE_HARDCORD_TABLE)
	ctx->bitReverseIndexTable = s_bitReverseTable[log2N - 1];
	ctx->bitReverseIndexTableLen = s_bitReversePairNums[log2N - 1];
#else
	// T.B.D
#endif
	*pctx = ctx;
	
	return 0;

exit_error:
	CleanOsakanaFft(ctx);
	ctx = NULL;
	*pctx = NULL;

	return ret;
}

void CleanOsakanaFft(OsakanaFftContext_t* ctx)
{
	free(ctx->twiddles);
	ctx->twiddles = NULL;

#if !defined(USE_HARDCORD_TABLE)
	free(ctx->bitReverseIndexTable);
#endif
	ctx->bitReverseIndexTable = NULL;

	free(ctx);
}

static inline void butterfly(osk_complex_t* r, const osk_complex_t* tf, int idx_a, int idx_b)
{
	osk_complex_t up = r[idx_a];
	osk_complex_t dn = r[idx_b];

	//r[idx_a] = up + tf * dn;
	//r[idx_b] = up - tf * dn;
	osk_complex_t dntf = complex_mult(&dn, tf);
	r[idx_a] = complex_add(&up, &dntf);
	r[idx_b] = complex_sub(&up, &dntf);
}

void OsakanaFft(const OsakanaFftContext_t* ctx, osk_complex_t* x)
{
	for (int i = 0; i < ctx->bitReverseIndexTableLen; i++) {
		const osk_bitreverse_idx_pair_t* pair = &ctx->bitReverseIndexTable[i];
		complex_swap(&x[pair->first], &x[pair->second]);
	}

	int dj = 2;
	int bnum = 1; // number of butterfly in 2nd loop
	int tw_idx_shift = ctx->log2N - 1;

	for (int i = 0; i < ctx->log2N; i++) {
		// j = 0,2,4,6
		// j = 0,4
		// j = 0
		for (int j = 0; j < ctx->N; j += dj) {
			int idx_a = j;
			int idx_b = j + bnum;
			for (int k = 0; k < bnum; k++) {

				//osk_complex_t tf = twiddle(k, dj);
				//butterfly(&f[0], &tf, idx_a++, idx_b++);

				int tw_idx = k << tw_idx_shift;
				osk_complex_t tf = ctx->twiddles[tw_idx];
				butterfly(&x[0], &tf, idx_a++, idx_b++);
			}
		}
		dj = dj << 1;
		bnum = bnum << 1;
		tw_idx_shift--;
	}
}

void OsakanaIfft(const OsakanaFftContext_t* ctx, osk_complex_t* x)
{
	for (int i = 0; i < ctx->bitReverseIndexTableLen; i++) {
		const osk_bitreverse_idx_pair_t* pair = &ctx->bitReverseIndexTable[i];
		complex_swap(&x[pair->first], &x[pair->second]);
	}

	int dj = 2;
	int bnum = 1;
	int tw_idx_shift = ctx->log2N - 1;

	for (int i = 0; i < ctx->log2N; i++) {
		for (int j = 0; j < ctx->N; j += dj) {
			int idx_a = j;
			int idx_b = j + bnum;
			for (int k = 0; k < bnum; k++) {
				
				/*osk_complex_t tf = twiddle(k, dj);
				tf.im = -tf.im;
				butterfly(&f[0], &tf, idx_a++, idx_b++);*/

				int tw_idx = k << tw_idx_shift;
				osk_complex_t tf = ctx->twiddles[tw_idx];
				tf.im = -tf.im;
				butterfly(&x[0], &tf, idx_a, idx_b);

				x[idx_a].re /= 2.0;
				x[idx_a].im /= 2.0;
				x[idx_b].re /= 2.0;
				x[idx_b].im /= 2.0;

				idx_a++;
				idx_b++;
			}
		}
		dj = dj << 1;
		bnum = bnum << 1;
		tw_idx_shift--;
	}
}
