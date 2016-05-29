#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "OsakanaFft.h"
#include "OsakanaFftUtil.h"

// mbed has memset() in mbed.h
#ifdef __MBED__
#include <mbed.h>
// mbed doesn't have M_PI
#ifndef M_PI
#define M_PI           3.14159265358979323846
#endif
#else
#include <memory.h>
#endif

struct _OsakanaFftContext_t {
	int N;
	int log2N;
	// twiddle factor table
	complex_t** twiddles;
	// bit reverse index table
	int* bitReverseIndexTable;
};

// W^n_N = exp(-i2pin/N)
// = cos(2 pi n/N) - isin(2 pi n/N)
static inline complex_t twiddle(int n, int Nin)
{
	float theta = 2.0f*M_PI*n / Nin;
	return MakeComplex(cos(theta), -sin(theta));
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
	ctx->twiddles = (complex_t**)malloc(sizeof(complex_t*) * N);
	if (ctx->twiddles == NULL) {
		return -2;
	}

	int numAllocated = 0;
	int itemNum = 1; // 1,2,4,...
	for (int i = 0; i <= log2N; i++) {
		ctx->twiddles[i] = (complex_t*)malloc(sizeof(complex_t) * itemNum);
		if (ctx->twiddles[i] == NULL) {
			ret = -3;
			goto exit_error;
		}
		for (int j = 0; j < itemNum; j++) {
			ctx->twiddles[i][j] = twiddle(j, 2 << i);
		}

		itemNum = itemNum << 1;
	}

	ctx->bitReverseIndexTable = (int*)malloc(sizeof(complex_t) * N);
	if (ctx->bitReverseIndexTable == NULL) {
		ret = -4;
	}
	for (int i = 0; i < N; i++) {
		ctx->bitReverseIndexTable[i] = bitReverse(log2N, i);
	}
	*pctx = ctx;

	return 0;

exit_error:
#if !defined(USE_HARDCORD_TABLE)
	for (int i = 0; i < numAllocated; i++) {
		free(ctx->twiddles[i]);
		ctx->twiddles[i] = NULL;
	}
	free(ctx->twiddles);
#endif
	ctx->twiddles = NULL;

	free(ctx->bitReverseIndexTable);
	return ret;
}

void CleanOsakanaFft(OsakanaFftContext_t* ctx)
{
	for (int i = 0; i < ctx->log2N + 1; i++) {
		free(ctx->twiddles[i]);
		ctx->twiddles[i] = NULL;
	}
	free(ctx->twiddles);
	ctx->twiddles = NULL;

	free(ctx->bitReverseIndexTable);
	ctx->bitReverseIndexTable = NULL;
}

static inline void butterfly(complex_t* r, const complex_t* tf, int idx_a, int idx_b)
{
	complex_t up = r[idx_a];
	complex_t dn = r[idx_b];

	//r[idx_a] = up + tf * dn;
	//r[idx_b] = up - tf * dn;
	complex_t dntf = complex_mult(&dn, tf);
	r[idx_a] = complex_add(&up, &dntf);
	r[idx_b] = complex_sub(&up, &dntf);
}

void OsakanaFft(const OsakanaFftContext_t* ctx, complex_t* f, complex_t* F)
{
	for (int i = 0; i < ctx->N; i++) {
		int ridx = ctx->bitReverseIndexTable[i];
		F[i] = f[ridx];
	}

	int dj = 2;
	int bnum = 1; // number of butterfly in 2nd loop

	for (int i = 0; i < ctx->log2N; i++) {
		// j = 0,2,4,6
		// j = 0,4
		// j = 0
		for (int j = 0; j < ctx->N; j += dj) {
			int idx_a = j;
			int idx_b = j + bnum;
			for (int k = 0; k < bnum; k++) {

				//complex_t tf = twiddle(k, dj);
				//butterfly(&f[0], &tf, idx_a++, idx_b++);

				complex_t tf = ctx->twiddles[i][k];
				butterfly(&F[0], &tf, idx_a++, idx_b++);
			}
		}
		dj = dj << 1;
		bnum = bnum << 1;
	}
}

void OsakanaIfft(const OsakanaFftContext_t* ctx, complex_t* F, complex_t* f)
{
	for (int i = 0; i < ctx->N; i++) {
		int ridx = ctx->bitReverseIndexTable[i];
		f[i] = F[ridx];
	}

	int dj = 2;
	int bnum = 1;

	for (int i = 0; i < ctx->log2N; i++) {
		for (int j = 0; j < ctx->N; j += dj) {
			int idx_a = j;
			int idx_b = j + bnum;
			for (int k = 0; k < bnum; k++) {
				
				/*complex_t tf = twiddle(k, dj);
				tf.im = -tf.im;
				butterfly(&f[0], &tf, idx_a++, idx_b++);*/

				complex_t tf = ctx->twiddles[i][k];
				tf.im = -tf.im;
				butterfly(&f[0], &tf, idx_a++, idx_b++);
			}
		}
		dj = dj << 1;
		bnum = bnum << 1;
	}

	for (int i = 0; i < ctx->N; i++) {
		f[i].re /= ctx->N;
		f[i].im /= ctx->N;
	}
}
