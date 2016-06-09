// OsakanaPitchDetection.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include "OsakanaFpFft.h"
#include "PeakDetectMachine.h"

#define N					512		// fft sampling num(last half is 0 pad)
#define LOG2N				9		// log2(N)
#define N2					(N/2)	// sampling num of analog input
#define N_ADC				N2
#define T					(0.03701f/(1024/N))		// time to take N2 signals to samples in sec.
#define T_PER_SAMPLE		(T/N2)	// factor to compute index to freq
#define FREQ_PER_001SAMPLE	((uint32_t)(100.0f *1.0f/T_PER_SAMPLE))
#define NOTE_CONST			0.025085832972
#define NOTE_CONST_INV10	3986	// 10/NOTE_CONST
#define DEBUG_OUTPUT_NUM    512

using namespace std;

#if defined (_USE_Q1_14_FIXEDPOINT)
static const string kNsdfFileName("nsdf_Q1.14.dat");
#elif defined(_USE_Q7_8_FIXEDPOINT)
static const string kNsdfFileName("nsdf_Q7.8.dat");
#else
static const string kNsdfFileName("nsdf_Q15.16.dat");
#endif

osk_fp_complex_t x[N] = { { 0, 0 } };
Fp_t x2[N2] = { 0 };
Fp_t _m[N2] = { 0 };
char buf[128] = { 0 };// for output

int readData(const string& filename, Fp_t* data, uint8_t stride, const int dataNum)
{
	ifstream file(filename);
	if (!file.is_open()) {
		cout << "can't open " << filename << endl;
		return 1;
	}

	string line;
	int counter = 0;
	while (getline(file, line) && counter < dataNum) {
		istringstream iss(line);
		float x;
		if (!(iss >> x)) {
			cout << "can't convert " << line << " to float" << endl;
			return 1;
		}
		*data = static_cast<int16_t>(x);
		data += stride;
		counter++;
	}

	file.close();
	return 0;
}

int saveData(const string& filename, Fp_t* data, const int dataNum)
{
	ofstream file(filename);
	if (!file.is_open()) {
		cout << "can't open " << filename << endl;
		return 1;
	}

	for (int i = 0; i < dataNum; i++) {
		char buf[128] = { 0 };
		Fp2CStr(data[i], buf, sizeof(buf));
		file << buf << endl;
	}

	file.close();
	return 0;
}


int DetectPitch(OsakanaFpFftContext_t* ctx, MachineContext_t* mctx, const string& filename)
{
	// till stabilized reset all
	memset(&x, 0, sizeof(x));
	memset(&x2, 0, sizeof(x2));
	memset(&_m, 0, sizeof(_m));

	// sampling from analog pin
	printf("sampling...\n");
	readData(filename, &x[0].re, 2, N_ADC);
	printf("sampled\n");

	printf("raw data --\n");
	for (int i = 0; i < 512; i++) {
		printf("%u\n", x[i].re);
	}
	printf("normalizing...\n");
	for (int i = 0; i < N2; i++) {
		int data = x[i].re & 0x00003FFF;
		data -= 512; // center to 0 and make it signed
		x[i].re = (Fp_t)data << (FPSHFT - 9);// div 512 then shift
		x[i].im = 0;
		x[N - i - 1].re = 0;
		x[N - i - 1].im = 0;
		x2[i] = FpMul(x[i].re, x[i].re);
		x2[i] = x2[i] >> 10;
	}
	printf("normalized\n");

	printf("-- normalized input signal\n");
	for (int i = 0; i < DEBUG_OUTPUT_NUM; i++) {
		fp_complex_str(&x[i], buf, sizeof(buf));
		printf("%s\n", buf);
	}

	printf("-- fft/N\n");
	OsakanaFpFft(ctx, x, 1);
	for (int i = 0; i < DEBUG_OUTPUT_NUM; i++) {
		fp_complex_str(&x[i], buf, sizeof(buf));
		printf("%s\n", buf);
	}

	printf("-- power spectrum\n");
	for (int i = 0; i < N; i++) {
		FpW_t re = FpMul(x[i].re, x[i].re) + FpMul(x[i].im, x[i].im);
		x[i].re = (Fp_t)(re >> 1);
		x[i].im = 0;
	}

	for (int i = 0; i < DEBUG_OUTPUT_NUM; i++) {
		fp_complex_str(&x[i], buf, sizeof(buf));
		printf("%s\n", buf);
	}

	printf("-- IFFT\n");
	OsakanaFpIfft(ctx, x, 1);
	for (int i = 0; i < DEBUG_OUTPUT_NUM; i++) {
		fp_complex_str(&x[i], buf, sizeof(buf));
		printf("%s\n", buf);
	}

	_m[0] = x[0].re << 2;
	for (int t = 1; t < N2; t++) {
		_m[t] = _m[t - 1] - x2[t - 1] + x2[t];
	}

	printf("-- ms smart\n");
	for (int i = 0; i < DEBUG_OUTPUT_NUM; i++) {
		Fp2CStr(_m[i], buf, sizeof(buf));
		printf("%s\n", buf);
	}

	// nsdf
	Fp_t* _nsdf = _m; // reuse buffer
	for (int t = 0; t < N2; t++) {
		Fp_t mt = _m[t] + FLOAT2FP(0.01f); // add small number to avoid 0 div
		_nsdf[t] = FpDiv(x[t].re, mt);
		_nsdf[t] = _nsdf[t] * 2 * 2;
	}
	printf("-- _nsdf\n");
	for (int i = 0; i < DEBUG_OUTPUT_NUM; i++) {
		Fp2CStr(_nsdf[i], buf, sizeof(buf));
		printf("%s\n", buf);
	}

	printf("-- pitch detection\n");
	for (int i = 0; i < N2; i++) {
		//Fp2CStr(_nsdf[i], buf, sizeof(buf));
		//printf("Input %s\n", buf);
		Input(mctx, _nsdf[i]);
	}

	PeekInfo keyMaximums[1] = { 0 };
	int keyMaxLen = 0;
	GetKeyMaximums(mctx, FLOAT2FP(0.8f), keyMaximums, 1, &keyMaxLen);
	if (0 < keyMaxLen) {
		uint32_t freq = FREQ_PER_001SAMPLE / (keyMaximums[0].index * 100);
		uint8_t note = kNoteTable[keyMaximums[0].index];
		printf("freq=%u Hz, note=%s\n", freq, kNoteStrings[note]);
		Fp_t delta = 0;
		if (ParabolicInterp(mctx, keyMaximums[0].index, _nsdf, N2, &delta)) {
			Fp2CStr(delta, buf, sizeof(buf));
			printf("delta %s\n", buf);
		}
	}

	printf("finished\n");
	return 0;
}

int main(int argc, char* argv[])
{
	if (argc < 1) {
		wcout << "need to give file in command-line paremeter" << endl;
		return 1;
	}

	OsakanaFpFftContext_t* ctx;
	if (InitOsakanaFpFft(&ctx, N, LOG2N) != 0) {
		printf("InitOsakanaFpFft error\n");
		return 1;
	}

	MachineContext_t* mctx = NULL;
	mctx = CreatePeakDetectMachineContext();

	while (1) {
		DetectPitch(ctx, mctx, argv[1]);
		ResetMachine(mctx);
		break;// for debug
	}

	DestroyPeakDetectMachineContext(mctx);
	CleanOsakanaFpFft(ctx);

	return 0;
}

