#ifndef _OSAKANAFP_H_
#define _OSAKANAFP_H_

#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#if _USE_Q7_8_FIXEDPOINT
#define FPSHFT		8
typedef int16_t		Fp_t;
typedef uint8_t		FpFract_t;
typedef int32_t		FpBigFp_t;
#else // otherwise Q15.16
#define FPSHFT		16
typedef int32_t		Fp_t;
typedef uint16_t	FpFract_t;
typedef int64_t		FpBigFp_t;
#endif

#define FPONE	(1 << FPSHFT)
#define FRACT_MASK	((1 << FPSHFT) - 1)
#define INT2FP(x) ((x) << FPSHFT)
#define FLOAT2FP(x) ((int)((x) * (1 << FPSHFT)))
#define FP2INT(x) ((x) >> FPSHFT)

#if !defined(_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES)

void strncpy_s(char *strDest,
	size_t numberOfElements,
	const char *strSource,
	size_t count) {
	strncpy(strDest, count, numberOfElements);
}

void strncat_s(
	char *strDest,
	size_t numberOfElements,
	const char *strSource,
	size_t count
	) {
	strncat(strDest, strSource, count);
}

void _itoa_s(
	int value,
	char *buffer,
	size_t sizeInCharacters,
	int radix
	)
{
	itoa(value, buffer, radix);
}

#endif

static const Fp_t kMsb = (Fp_t)(1 << ((sizeof(Fp_t) * 8) -1));

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

static inline int Fp2Int(Fp_t a)
{
	return (a >> FPSHFT);
}

static inline Fp_t Int2Fp(int a)
{
	return (a << FPSHFT);
}

static inline Fp_t Float2Fp(float a)
{
	return ((Fp_t)(a * FPONE));
}

static inline float Fp2Float(Fp_t a)
{
	float fp = (float)a;
	fp = fp / (float)(1 << FPSHFT); // multiplication by a constant
	return fp;
}

static inline Fp_t FpMul(Fp_t a, Fp_t b)
{
	Fp_t fp = ((FpBigFp_t)a * (FpBigFp_t)b) >> FPSHFT;
	return fp;
}

static inline Fp_t FpDiv(Fp_t a, Fp_t b)
{
	Fp_t fp = ((FpBigFp_t)a / (FpBigFp_t)b) << FPSHFT;
	return fp;
}

static inline Fp_t FpInvSgn(Fp_t a)
{
	return -a;
}

static inline Fp_t FpRShift(Fp_t a, int n)
{
	return (a >> n);
}

static inline Fp_t FpLShift(Fp_t a, int n)
{
	return (a << n);
}

static inline char* Fp2CStr(Fp_t a, char* buf, const size_t buf_size)
{
	float af = Fp2Float(a);
	Fp_t fractParts = a & FRACT_MASK;

	buf[0] = '\0';
	if (a&kMsb) {
		char* cat = "-";
		strncpy_s(buf, buf_size, cat, strlen(cat));

		// Fp ‚Å‚Í¬”•”•ª‚Í2‚Ì•â”•\Œ»‚È‚Ì‚ÅŒ³‚É–ß‚·
		fractParts = (-a) & FRACT_MASK;
	}
	int mainPart = (int)fabs(af);

	char itoaBuf[33] = { 0 };
	_itoa_s(mainPart, itoaBuf, sizeof(itoaBuf), 10);
	strncat_s(buf, buf_size, itoaBuf, strlen(itoaBuf));

	char* cat = ".";
	strncat_s(buf, buf_size, cat, strlen(cat));

	while (fractParts > 0) {
		fractParts *= 10;
		int num = (fractParts >> FPSHFT);
		
		_itoa_s(num, itoaBuf, sizeof(itoaBuf), 10);
		strncat_s(buf, buf_size, itoaBuf, strlen(itoaBuf));

		fractParts &= FRACT_MASK;
	}
	return buf;
}

static inline char* Fp2HexCStr(Fp_t a, char* buf, const size_t buf_size)
{
	if (sizeof(Fp_t) <= 16) {
		snprintf(buf, buf_size, "0x%0*hu", 2 * sizeof(Fp_t) / 8, a);
	}
	else {
		snprintf(buf, buf_size, "0x%0*x", 2 * sizeof(Fp_t) / 8, a);
	}
	
return		buf;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
