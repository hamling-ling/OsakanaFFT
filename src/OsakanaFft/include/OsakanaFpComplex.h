#ifndef _OSAKANACOMPLEXFP_H_
#define _OSAKANACOMPLEXFP_H_

#include "OsakanaFp.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct {
	Fp_t re;
	Fp_t im;
} osk_fp_complex_t;

typedef struct {
	FpW_t re;
	FpW_t im;
} osk_fpw_complex_t;

static inline osk_fp_complex_t FpMakeComplex(Fp_t re, Fp_t im)
{
	osk_fp_complex_t complex;
	complex.re = re;
	complex.im = im;
	return complex;
}

static inline osk_fp_complex_t FpMakeComplexFromFpW(const osk_fpw_complex_t* x)
{
	return FpMakeComplex((Fp_t)x->re, (Fp_t)x->im);
}

static inline osk_fpw_complex_t FpWMakeComplexFromFp(const osk_fp_complex_t* x)
{
	osk_fpw_complex_t complex;
	complex.re = (FpW_t)(x->re);
	complex.im = (FpW_t)(x->im);
	return complex;;
}

static inline osk_fp_complex_t fp_complex_add(const osk_fp_complex_t* a, const osk_fp_complex_t* b)
{
	osk_fp_complex_t x;
	x.re = a->re + b->re;
	x.im = a->im + b->im;
	return x;
}

static inline osk_fpw_complex_t fpw_complex_add(const osk_fpw_complex_t* a, const osk_fpw_complex_t* b)
{
	osk_fpw_complex_t x;
	x.re = a->re + b->re;
	x.im = a->im + b->im;
	return x;

}
static inline osk_fp_complex_t fp_complex_sub(const osk_fp_complex_t* a, const osk_fp_complex_t* b)
{
	osk_fp_complex_t x;
	x.re = a->re - b->re;
	x.im = a->im - b->im;
	return x;
}

static inline osk_fpw_complex_t fpw_complex_sub(const osk_fpw_complex_t* a, const osk_fpw_complex_t* b)
{
	osk_fpw_complex_t x;
	x.re = a->re - b->re;
	x.im = a->im - b->im;
	return x;

}
static inline osk_fp_complex_t fp_complex_mult(const osk_fp_complex_t* a, const osk_fp_complex_t* b)
{
	osk_fp_complex_t x;
	FpW_t re_w = (FpW_t)a->re * (FpW_t)b->re;
	re_w -= (FpW_t)a->im * (FpW_t)b->im;
	x.re = (Fp_t)(re_w >> FPSHFT);

	FpW_t im_w = (FpW_t)a->re  * (FpW_t)b->im;
	im_w += (FpW_t)a->im * (FpW_t)b->re;
	x.im = (Fp_t)(im_w >> FPSHFT);

	return x;
}

static inline osk_fp_complex_t fp_complex_mult_left_shift(const osk_fp_complex_t* a, const osk_fp_complex_t* b, int shift)
{
	osk_fp_complex_t x;
	FpW_t re_w = (FpW_t)a->re * (FpW_t)b->re;
	re_w -= (FpW_t)a->im * (FpW_t)b->im;
	x.re = (Fp_t)(re_w >> (FPSHFT-shift));

	FpW_t im_w = (FpW_t)a->re  * (FpW_t)b->im;
	im_w += (FpW_t)a->im * (FpW_t)b->re;
	x.im = (Fp_t)(im_w >> (FPSHFT-shift));

	return x;
}

static inline osk_fpw_complex_t fpw_complex_mult_left_shift(const osk_fpw_complex_t* a, const osk_fpw_complex_t* b, int shift)
{
	osk_fpw_complex_t x;
	FpW_t re_w = a->re * b->re;
	re_w -= a->im * b->im;
	x.re = (re_w >> (FPSHFT - shift));

	FpW_t im_w = a->re  * b->im;
	im_w += a->im * b->re;
	x.im = (im_w >> (FPSHFT - shift));

	return x;
}

static inline osk_fp_complex_t fp_complex_mult_fp(const osk_fp_complex_t* a, Fp_t b)
{
	osk_fp_complex_t x;
	x.re = FpMul(a->re, b) - FpMul(a->im, b);
	return x;
}

static inline osk_fp_complex_t fp_complex_div_fp(const osk_fp_complex_t* a, Fp_t b)
{
	osk_fp_complex_t x;
	x.re = FpDiv(a->re, b) - FpDiv(a->im, b);
	return x;
}

static inline osk_fp_complex_t fp_complex_r_shift(const osk_fp_complex_t* a, int n)
{
	osk_fp_complex_t x;
	x.re = FpRShift(a->re, n);
	x.im = FpRShift(a->im, n);
	return x;
}

static inline osk_fp_complex_t fp_complex_l_shift(const osk_fp_complex_t* a, int n)
{
	osk_fp_complex_t x;
	x.re = FpLShift(a->re, n);
	x.im = FpLShift(a->im, n);
	return x;
}

static inline osk_fpw_complex_t fpw_complex_l_shift(const osk_fpw_complex_t* a, int n)
{
	osk_fpw_complex_t x;
	x.re = FpWLShift(a->re, n);
	x.im = FpWLShift(a->im, n);
	return x;
}


static inline void fp_complex_swap(osk_fp_complex_t* a, osk_fp_complex_t* b)
{
	FpSwap(&a->re, &b->re);
	FpSwap(&a->im, &b->im);
}

static inline char* fp_complex_str(const osk_fp_complex_t* a, char* buf, size_t buf_size)
{
	char buf_sub[32] = { '\0' };
	memset(buf, 0, buf_size);

	Fp2CStr(a->re, buf_sub, sizeof(buf_sub));
	StrNCat_S(buf, buf_size, buf_sub, strlen(buf_sub));
	const char* sep = ", ";
	StrNCat_S(buf, buf_size, sep, strlen(sep));

	Fp2CStr(a->im, buf_sub, sizeof(buf_sub));
	StrNCat_S(buf, buf_size, buf_sub, strlen(buf_sub));

	return buf;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
