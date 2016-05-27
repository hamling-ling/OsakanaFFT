#include <assert.h>
#include "OsakanaFpFft.h"
#include "OsakanaFftUtil.h"
#include "twiddletable.h"


struct _OsakanaFpFftContext_t {
	int N;		// num of samples
	int log2N;	// log2(N)
	// twiddle factor table
	const fp_complex_t** twiddles;
	// bit reverse index table
	int* bitReverseIndexTable;
};

int InitOsakanaFpFft(OsakanaFpFftContext_t** pctx, int N, int log2N)
{
	OsakanaFpFftContext_t* ctx = (OsakanaFpFftContext_t*)malloc(sizeof(OsakanaFpFftContext_t));

	memset(ctx, 0, sizeof(OsakanaFpFftContext_t));
	ctx->N = N;
	ctx->log2N = log2N;
	ctx->twiddles = s_twiddlesFp;

	ctx->bitReverseIndexTable = (int*)malloc(sizeof(fp_complex_t) * N);
	for (int i = 0; i < N; i++) {
		ctx->bitReverseIndexTable[i] = bitReverse(log2N, i);
	}
	*pctx = ctx;

	return 0;
}

void CleanOsakanaFpFft(OsakanaFpFftContext_t* ctx)
{
	free(ctx->bitReverseIndexTable);
	ctx->bitReverseIndexTable = NULL;
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

	for (int i = 0; i < ctx->log2N; i++) {
		for (int j = 0; j < ctx->N; j += dj) {
			int idx_a = j;
			int idx_b = j + bnum;
			for (int k = 0; k < bnum; k++) {
				fp_complex_t tf = ctx->twiddles[i][k];
				fp_butterfly(&F[0], &tf, idx_a++, idx_b++);
			}
		}
		dj = dj << 1;
		bnum = bnum << 1;
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

	for (int i = 0; i < ctx->log2N; i++) {

		for (int j = 0; j < ctx->N; j += dj) {
			int idx_a = j;
			int idx_b = j + bnum;
			for (int k = 0; k < bnum; k++) {

				// div f[idx_a] by 2 instead of div by N end of func
				f[idx_a] = fp_complex_r_shift(&f[idx_a], 1);
				f[idx_b] = fp_complex_r_shift(&f[idx_b], 1);

				fp_complex_t tf = ctx->twiddles[i][k];
				tf.im = -tf.im;

				fp_butterfly(&f[0], &tf, idx_a, idx_b);

				idx_a++;
				idx_b++;
			}
		}
		dj = dj << 1;
		bnum = bnum << 1;
	}
}
