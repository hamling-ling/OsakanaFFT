#if defined(_WIN32)
#include "stdafx.h"
#endif
#include "math.h"
#include "OsakanaFft.h"
#include "OsakanaFpFft.h"
#include <iostream>
#include "StopWatch.h"

#if 1
#define N		1024	// num of samples
#define log2N	10	// log2(N)
#else
#define N		8	// num of samples, 8, 16, 32, 64, 128, 256, 512...
#define log2N	3	// log2(N)         3,  4,  5,  6,   7,   8,   9
#endif

using namespace std;

void testFft()
{
	osk_complex_t x[N]  = { { 0.0f, 0.0f } };

	for (int i = 0; i < N; i++) {
		x[i].re = (float)sin(3.5 * i * 2.0 * M_PI / N);
		x[i].im = 0.0f;
	}

	OsakanaFftContext_t* ctx= NULL;
	InitOsakanaFft(&ctx, N, log2N);

	char buf[64] = { 0 };// debug
	cout << "--" << endl;
	for (int i = 0; i < N && i < 10; i++) {
		cout << complex_str(&x[i], buf, sizeof(buf)) << endl;
	}

	for(int i = 0; i < 10000; i++) {
		OsakanaFft(ctx, &x[0]);
	}


	CleanOsakanaFft(ctx);
}

void testFpFft()
{
	osk_fp_complex_t x[N] = { { FLOAT2FP(0.0f), FLOAT2FP(0.0f) } };

	char buf[128] = { 0 };// debug

	for (int i = 0; i < N; i++) {
		float re = (float)sin(3.5 * i * 2.0 * M_PI / N);
		float im = 0.0f;
		x[i].re = Float2Fp(re);
		x[i].im = Float2Fp(im);
	}

	cout << "--" << endl;
	for (int i = 0; i < N && i < 10; i++) {
		cout << fp_complex_str(&x[i], buf, sizeof(buf)) << endl;
	}

	OsakanaFpFftContext_t* ctx = NULL;
	InitOsakanaFpFft(&ctx, N, log2N);

	OsakanaFpFft(ctx, &x[0], 1);
	cout << "--" << endl;
	for (int i = 0; i < N && i < 10; i++) {
		cout << fp_complex_str(&x[i], buf, sizeof(buf)) << endl;
	}

	OsakanaFpIfft(ctx, &x[0], 1);
	cout << "--" << endl;
	for (int i = 0; i < N && i < 10; i++) {
		cout << fp_complex_str(&x[i], buf, sizeof(buf)) << endl;
	}

	CleanOsakanaFpFft(ctx);
}

int main()
{
	testFft();
	//testFpFft();

	return 0;
}


