#ifndef _OSAKANACOMPLEXFP_H_
#define _OSAKANACOMPLEXFP_H_

#include "OsakanaFp.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct {
	Fp_t re;
	Fp_t im;
} fp_complex_t;

static inline fp_complex_t FpMakeComplex(Fp_t re, Fp_t im)
{
	fp_complex_t complex;
	complex.re = re;
	complex.im = im;
	return complex;
}

static inline fp_complex_t fp_complex_add(const fp_complex_t* a, const fp_complex_t* b)
{
	fp_complex_t x;
	x.re = a->re + b->re;
	x.im = a->im + b->im;
	return x;
}

static inline fp_complex_t fp_complex_sub(const fp_complex_t* a, const fp_complex_t* b)
{
	fp_complex_t x;
	x.re = a->re - b->re;
	x.im = a->im - b->im;
	return x;
}

static inline fp_complex_t fp_complex_mult(const fp_complex_t* a, const fp_complex_t* b)
{
	fp_complex_t x;
	//x.re = a->re * b->re - a->im * b->im;
	x.re = FpMul(a->re, b->re) - FpMul(a->im, b->im);
	//x.im = a->re * b->im + a->im * b->re;
	x.im = FpMul(a->re, b->im) + FpMul(a->im, b->re);
	return x;
}

static inline fp_complex_t fp_complex_mult_fp(const fp_complex_t* a, Fp_t b)
{
	fp_complex_t x;
	x.re = FpMul(a->re, b) - FpMul(a->im, b);
	return x;
}

static inline fp_complex_t fp_complex_div_fp(const fp_complex_t* a, Fp_t b)
{
	fp_complex_t x;
	x.re = FpDiv(a->re, b) - FpDiv(a->im, b);
	return x;
}

static inline fp_complex_t fp_complex_r_shift(const fp_complex_t* a, int n)
{
	fp_complex_t x;
	x.re = FpRShift(a->re, n);
	x.im = FpRShift(a->im, n);
	return x;
}

static inline fp_complex_t fp_complex_l_shift(const fp_complex_t* a, int n)
{
	fp_complex_t x;
	x.re = FpLShift(a->re, n);
	x.im = FpLShift(a->im, n);
	return x;
}

static inline void fp_complex_swap(fp_complex_t* a, fp_complex_t* b)
{
	FpSwap(&a->re, &b->re);
	FpSwap(&a->im, &b->im);
}

static inline char* fp_complex_str(const fp_complex_t* a, char* buf, size_t buf_size)
{
	char buf_sub[32] = { '\0' };
	memset(buf, 0, buf_size);

	Fp2CStr(a->re, buf_sub, sizeof(buf_sub));
	StrNCat_S(buf, buf_size, buf_sub, strlen(buf_sub));
	char* sep = ", ";
	StrNCat_S(buf, buf_size, sep, strlen(sep));

	Fp2CStr(a->im, buf_sub, sizeof(buf_sub));
	StrNCat_S(buf, buf_size, buf_sub, strlen(buf_sub));

	return buf;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
