#ifndef _OSAKANAFPFFT_H_
#define _OSAKANAFPFFT_H_

#include "OsakanaFpComplex.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct _OsakanaFpFftContext_t OsakanaFpFftContext_t;

int InitOsakanaFpFft(OsakanaFpFftContext_t** pctx, int N, int log2N);
void CleanOsakanaFpFft(OsakanaFpFftContext_t* ctx);
void OsakanaFpFft(const OsakanaFpFftContext_t* ctx, osk_fp_complex_t* x, int scale);
void OsakanaFpFftScales(const OsakanaFpFftContext_t* ctx, osk_fp_complex_t* x, int* scales);
void OsakanaFpIfft(const OsakanaFpFftContext_t* ctx, osk_fp_complex_t* x, int scale);
void OsakanaFpIfftScales(const OsakanaFpFftContext_t* ctx, osk_fp_complex_t* x, int* scales);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
