#ifndef _OSAKANAFFT_H_
#define _OSAKANAFFT_H_

#include "OsakanaComplex.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct _OsakanaFftContext_t OsakanaFftContext_t;

int InitOsakanaFft(OsakanaFftContext_t** pctx, int N, int log2N);
void CleanOsakanaFft(OsakanaFftContext_t* ctx);
void OsakanaFft(const OsakanaFftContext_t* ctx, complex_t* f, complex_t* F);
void OsakanaIfft(const OsakanaFftContext_t* ctx, complex_t* F, complex_t* f);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif