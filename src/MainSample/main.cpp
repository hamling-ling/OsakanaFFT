#include "stdafx.h"
#include "math.h"
#include "OsakanaFft.h"
#include "OsakanaFpFft.h"
#include <iostream>
#include "StopWatch.h"

#if 0
#define N		1024	// num of samples
#define log2N	10	// log2(N)
#else
#define N		256	// num of samples, 8, 16, 32, 64, 128, 256, 512...
#define log2N	8	// log2(N)         3,  4,  5,  6,   7,   8,   9
#endif

using namespace std;

void testFft()
{
	complex_t f[N] = { { 0.0f, 0.0f } };
	complex_t F[N] = { { 0.0f, 0.0f } };

	for (int i = 0; i < N; i++) {
		f[i].re = (float)sin(3.5 * i * 2.0 * M_PI / N);
		f[i].im = 0.0f;
	}

	OsakanaFftContext_t* ctx = NULL;
	InitOsakanaFft(&ctx, N, log2N);

	OsakanaFft(ctx, &f[0], &F[0]);

	CleanOsakanaFft(ctx);
}

void testIfft()
{
	complex_t f[N]  = { { 0.0f, 0.0f } };
	complex_t F[N]  = { { 0.0f, 0.0f } };
	complex_t f2[N] = { { 0.0f, 0.0f } };

	for (int i = 0; i < N; i++) {
		f[i].re = (float)sin(0.3 * i * 2.0 * M_PI / N);
		f[i].im = 0.0f;
	}

	OsakanaFftContext_t* ctx= NULL;
	InitOsakanaFft(&ctx, N, log2N);

	OsakanaFft(ctx, &f[0], &F[0]);
	OsakanaIfft(ctx, &F[0], &f2[0]);

	char buf[64] = { 0 };// debug
	cout << "--" << endl;
	for (int i = 0; i < N && i < 10; i++) {
		cout << complex_str(&f[i], buf, sizeof(buf)) << endl;
	}
	cout << "--" << endl;
	for (int i = 0; i < N && i < 10; i++) {
		cout << complex_str(&F[i], buf, sizeof(buf)) << endl;
	}
	cout << "--" << endl;
	for (int i = 0; i < N && i < 10; i++) {
		cout << complex_str(&f2[i], buf, sizeof(buf)) << endl;
	}

	CleanOsakanaFft(ctx);
}

void testFpFft()
{
	fp_complex_t f[N] = { { FLOAT2FP(0.0f), FLOAT2FP(0.0f) } };
	fp_complex_t F[N] = { { FLOAT2FP(0.0f), FLOAT2FP(0.0f) } };

	char buf[64] = { 0 };// debug

	for (int i = 0; i < N; i++) {
		float re = (float)sin(0.3 * i * 2.0 * M_PI / N);
		float im = 0.0f;
		f[i].re = Float2Fp(re);
		f[i].im = Float2Fp(im);
	}

	OsakanaFpFftContext_t* ctx = NULL;
	InitOsakanaFpFft(&ctx, N, log2N);

	OsakanaFpFft(ctx, &f[0], &F[0]);

	CleanOsakanaFpFft(ctx);
}

void testFpIfft()
{
	fp_complex_t f[N] = { { FLOAT2FP(0.0f), FLOAT2FP(0.0f) } };
	fp_complex_t F[N] = { { FLOAT2FP(0.0f), FLOAT2FP(0.0f) } };
	fp_complex_t f2[N] = { { FLOAT2FP(0.0f), FLOAT2FP(0.0f) } };

	char buf[64] = { 0 };// debug

	for (int i = 0; i < N; i++) {
		float re = (float)sin(0.3 * i * 2.0 * M_PI / N);
		float im = 0.0f;
		f[i].re = Float2Fp(re);
		f[i].im = Float2Fp(im);
	}

	OsakanaFpFftContext_t* ctx = NULL;
	InitOsakanaFpFft(&ctx, N, log2N);

	OsakanaFpFft(ctx, &f[0], &F[0]);
	OsakanaFpIfft(ctx, &F[0], &f2[0]);

	cout << "--" << endl;
	for (int i = 0; i < N && i < 10; i++) {
		cout << fp_complex_str(&f[i], buf, sizeof(buf)) << endl;
	}
	cout << "--" << endl;
	for (int i = 0; i < N && i < 10; i++) {
		cout << fp_complex_str(&F[i], buf, sizeof(buf)) << endl;
	}
	cout << "--" << endl;
	for (int i = 0; i < N && i < 10; i++) {
		cout << fp_complex_str(&f2[i], buf, sizeof(buf)) << endl;
	}

	CleanOsakanaFpFft(ctx);
}

void benchFft()
{
	StopWatch<std::chrono::milliseconds> sw;

	fp_complex_t f[N] = { { FLOAT2FP(0.0f), FLOAT2FP(0.0f) } };
	fp_complex_t F[N] = { { FLOAT2FP(0.0f), FLOAT2FP(0.0f) } };
	fp_complex_t f2[N] = { { FLOAT2FP(0.0f), FLOAT2FP(0.0f) } };

	for (int i = 0; i < N; i++) {
		float re = (float)sin(0.3 * i * 2.0 * M_PI / N);
		float im = 0.0f;
		f[i].re = Float2Fp(re);
		f[i].im = Float2Fp(im);
	}

	OsakanaFpFftContext_t* ctx = NULL;
	InitOsakanaFpFft(&ctx, N, log2N);

	for (int i = 0; i < 10000; i++) {
		OsakanaFpFft(ctx, &f[0], &F[0]);
		OsakanaFpIfft(ctx, &F[0], &f2[0]);
	}

	CleanOsakanaFpFft(ctx);

}
void benchFpFft()
{
	StopWatch<std::chrono::milliseconds> sw;

	fp_complex_t f[N] = { { FLOAT2FP(0.0f), FLOAT2FP(0.0f) } };
	fp_complex_t F[N] = { { FLOAT2FP(0.0f), FLOAT2FP(0.0f) } };
	fp_complex_t f2[N] = { { FLOAT2FP(0.0f), FLOAT2FP(0.0f) } };

	for (int i = 0; i < N; i++) {
		float re = (float)sin(0.3 * i * 2.0 * M_PI / N);
		float im = 0.0f;
		f[i].re = Float2Fp(re);
		f[i].im = Float2Fp(im);
	}

	OsakanaFpFftContext_t* ctx = NULL;
	InitOsakanaFpFft(&ctx, N, log2N);

	for (int i = 0; i < 10000; i++) {
		OsakanaFpFft(ctx, &f[0], &F[0]);
		OsakanaFpIfft(ctx, &F[0], &f2[0]);
	}

	CleanOsakanaFpFft(ctx);
}

int main()
{
	//char buf[64] = { 0 };
	//cout << Fp2CStr(FLOAT2FP(0.0), buf, sizeof(buf));
	//testFft();
	//testIfft();
	//testFpFft();
	testFpIfft();
	//benchFft();
	//benchFpFft();

	return 0;
}


