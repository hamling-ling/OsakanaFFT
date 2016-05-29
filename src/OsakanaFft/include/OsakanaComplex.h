#ifndef _OSAKANACOMPLEX_H_
#define _OSAKANACOMPLEX_H_

#ifdef __MBED__
#include <mbed.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct {
	float re;
	float im;
} complex_t;

static inline complex_t MakeComplex(float re, float im)
{
	complex_t complex;
	complex.re = re;
	complex.im = im;
	return complex;
}

static inline complex_t complex_add(const complex_t* a, const complex_t* b)
{
	complex_t x;
	x.re = a->re + b->re;
	x.im = a->im + b->im;
	return x;
}

static inline complex_t complex_sub(const complex_t* a, const complex_t* b)
{
	complex_t x;
	x.re = a->re - b->re;
	x.im = a->im - b->im;
	return x;
}

static inline complex_t complex_mult(const complex_t* a, const complex_t* b)
{
	complex_t x;
	x.re = a->re * b->re - a->im * b->im;
	x.im = a->re * b->im + a->im * b->re;
	return x;
}

static inline char* complex_str(const complex_t* a, char* buf, size_t buf_size)
{
	snprintf(buf, buf_size, "%f, %f", a->re, a->im);

	return buf;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
