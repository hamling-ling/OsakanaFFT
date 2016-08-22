#include "stdafx.h"

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include "OsakanaFft.h"
#include "OsakanaFpFft.h"
#include "PeakDetectMachine.h"
#include "PeakDetectMachineFp.h"

#define LOG_NEWLINE "\n"
#define LOG_PRINTF	printf
#include "OsakanaFpFftDebug.h"

#if 0	//GR-CITRUS
#define N					512		// fft sampling num(last half is 0 pad)
#define LOG2N				9		// log2(N)
#define T1024_1024			(45.336000000000006f)	// adc speedd(time to take 1024x1024 samples in sec)
#define T_PER_SAMPLE		(T1024_1024/1024.0f/1024.0f)	// factor to compute index to freq
#define FREQ_PER_SAMPLE		((float)(1.0f/T_PER_SAMPLE))
#define FREQ_PER_1024SAMPLE	(FREQ_PER_SAMPLE*1024)
#endif
#if 1	// HRM1017
#define N					256		// fft sampling num(last half is 0 pad)
#define LOG2N				8		// log2(N)
#define T1024_1024			(75.8f)	// adc speedd(time to take 1024x1024 samples in sec)
#define T_PER_SAMPLE		FLOAT2FP(7.228851318359375e-5f)	// factor to compute index to freq
#define FREQ_PER_SAMPLE		(13833)			// 13833.45646437995 casted to int
#define FREQ_PER_1024SAMPLE	14165459
#endif

#define N2					(N/2)	// sampling num of analog input
#define N_ADC				N2

// debug
#define DEBUG_OUTPUT_NUM    10


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

osk_complex_t xf[N] = { { 0, 0 } };
float xf2[N2] = { 0 };
float _mf[N2] = { 0 };

int readDataFp(const string& filename, Fp_t* data, uint8_t stride, const int dataNum)
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

int readData(const string& filename, float* data, uint8_t stride, const int dataNum)
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
		*data = x;
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

int DetectPitchFp(OsakanaFpFftContext_t* ctx, MachineContextFp_t* mctx, const string& filename)
{
	// sampling from analog pin
	DLOG("sampling...");
	readDataFp(filename, &x[0].re, 2, N_ADC);
	DLOG("sampled");

	DLOG("raw data --");
	DRAWDATA(x, 512);

	DLOG("normalizing...");
	for (int i = 0; i < N2; i++) {
		int data = x[i].re & 0x00003FFF;
		data -= 512; // center to 0 and make it signed
		x[i].re = (Fp_t)(data << (FPSHFT - 9));// div 512 then shift
		x[i].im = 0;
		x[N2+i].re = 0;
		x[N2+i].im = 0;
		x2[i] = FpMul(x[i].re, x[i].re);
		x2[i] = x2[i] >> 10;
	}
	DLOG("normalized");

	DLOG("-- normalized input signal");
	DCOMPLEXFp(x, DEBUG_OUTPUT_NUM);

	DLOG("-- fft/N");
	OsakanaFpFft(ctx, x, 1);
	DCOMPLEXFp(x, DEBUG_OUTPUT_NUM);

	DLOG("-- power spectrum");
	for (int i = 0; i < N; i++) {
		FpW_t re = FpMul(x[i].re, x[i].re) + FpMul(x[i].im, x[i].im);
		x[i].re = (Fp_t)(re >> 1);
		x[i].im = 0;
	}
	DCOMPLEXFp(x, DEBUG_OUTPUT_NUM);

	DLOG("-- IFFT");
	OsakanaFpIfft(ctx, x, 1);
	DCOMPLEXFp(x, DEBUG_OUTPUT_NUM);

	_m[0] = x[0].re << 2;
	for (int t = 1; t < N2; t++) {
		_m[t] = _m[t - 1] - x2[t - 1] + x2[t];
	}

	DLOG("-- ms smart");
	DFPSFp(_m, DEBUG_OUTPUT_NUM);

	// nsdf
	Fp_t* _nsdf = _m; // reuse buffer
	for (int t = 0; t < N2; t++) {
		Fp_t mt = _m[t] + FLOAT2FP(0.01f); // add small number to avoid 0 div
		_nsdf[t] = FpDiv(x[t].re, mt);
		_nsdf[t] = _nsdf[t] << 2;
	}
	DLOG("-- _nsdf");
	DFPSFp(_nsdf, DEBUG_OUTPUT_NUM);

	DLOG("-- pitch detection");
	for (int i = 0; i < N2; i++) {
		InputFp(mctx, _nsdf[i]);
	}

	PeakInfoFp_t keyMaximums[4] = { 0 };
	int keyMaxLen = 0;
	GetKeyMaximumsFp(mctx, FLOAT2FP(0.8f), keyMaximums, sizeof(keyMaximums)/sizeof(PeakInfoFp_t), &keyMaxLen);
	if (0 < keyMaxLen) {
		Fp_t delta = 0;
		if (ParabolicInterpFp(mctx, keyMaximums[0].index, _nsdf, N2, &delta)) {
			Fp2CStr(delta, debug_output_buf_, sizeof(debug_output_buf_));
			printf("delta %s\n", debug_output_buf_);
		}
		// want freq = FREQ_PER_SAMPLE / (index+delta)
		// idx1024=1024*index
		int32_t idx1024 = keyMaximums[0].index << 10;
		// int expression of 1024*delta
		int32_t delta1024 = (delta >> (FPSHFT - 10));
		idx1024 += delta1024;
		// freq = freq_per_sample / idx
		int32_t freq = FREQ_PER_1024SAMPLE / idx1024;
		
		int32_t idx = (idx1024 + 512) >> 10;
		uint8_t note = kNoteTable[idx] % 12;

		DLOG("freq=%u Hz, note=%s\n", freq, kNoteStrings[note]);
	}

	DLOG("finished");
	return 0;
}

int DetectPitch(OsakanaFftContext_t* ctx, MachineContext_t* mctx, const string& filename)
{
	// sampling from analog pin
	DLOG("sampling...");
	readData(filename, &xf[0].re, 2, N_ADC);
	DLOG("sampled");

	DLOG("raw data --");
	DRAWDATAf(xf, DEBUG_OUTPUT_NUM);

	DLOG("normalizing...");
	for (int i = 0; i < N2; i++) { 
		xf[i].re -= 512.0f;
		xf[i].re /= 1023.0f;
		xf[i].im = 0.0f;
		xf[N2 + i].re = 0.0f;
		xf[N2 + i].im = 0.0f;
		xf2[i] = xf[i].re * xf[i].re;
	}
	DLOG("normalized");

	DLOG("-- normalized input signal");
	DCOMPLEX(xf, DEBUG_OUTPUT_NUM);

	DLOG("-- fft/N");
	OsakanaFft(ctx, xf);
	DCOMPLEX(xf, DEBUG_OUTPUT_NUM);

	DLOG("-- power spectrum");
	for (int i = 0; i < N; i++) {
		xf[i].re = xf[i].re * xf[i].re + xf[i].im * xf[i].im;
		xf[i].im = 0.0f;
	}
	DCOMPLEX(xf, DEBUG_OUTPUT_NUM);

	DLOG("-- IFFT");
	OsakanaIfft(ctx, xf);
	DCOMPLEX(xf, DEBUG_OUTPUT_NUM);

	_mf[0] = xf[0].re;
	for (int t = 1; t < N2; t++) {
		_mf[t] = _mf[t - 1] - xf2[t - 1] + xf2[t];
	}

	DLOG("-- ms smart");
	DFPS(_mf, DEBUG_OUTPUT_NUM);

	// nsdf
	float* _nsdf = _mf; // reuse buffer
	for (int t = 0; t < N2; t++) {
		float mt = _mf[t] + 0.01f; // add small number to avoid 0 div
		_nsdf[t] = xf[t].re / mt;
		_nsdf[t] = _nsdf[t] * 2.0f;
	}
	DLOG("-- _nsdf");
	DFPS(_nsdf, DEBUG_OUTPUT_NUM);

	DLOG("-- pitch detection");
	for (int i = 0; i < N2; i++) {
		Input(mctx, _nsdf[i]);
	}

	PeakInfo_t keyMaximums[4] = { 0 };
	int keyMaxLen = 0;
	GetKeyMaximums(mctx, 0.8f, keyMaximums, sizeof(keyMaximums) / sizeof(PeakInfo_t), &keyMaxLen);
	if (0 < keyMaxLen) {
		float delta = 0;
		if (ParabolicInterp(mctx, keyMaximums[0].index, _nsdf, N2, &delta)) {
			DLOG("delta %f\n", delta);
		}
		
		float freq = FREQ_PER_SAMPLE / (keyMaximums[0].index + delta);
		const float k = log10f(pow(2.0f, 1.0f / 12.0f));
		uint16_t midi = (uint16_t)round(log10f(freq / 27.5f) / k) + 21;
		DLOG("freq=%f Hz, note=%s\n", freq, kNoteStrings[midi % 12]);
	}

	DLOG("finished");
	return 0;
}

int main(int argc, char* argv[])
{
	if (argc < 1) {
		DLOG("need to give file in command-line paremeter");
		return 1;
	}

#if 1
	MachineContextFp_t* mctx = NULL;
	mctx = CreatePeakDetectMachineContextFp();

	OsakanaFpFftContext_t* ctx;
	if (InitOsakanaFpFft(&ctx, N, LOG2N) != 0) {
		DLOG("InitOsakanaFpFft error");
		return 1;
	}

	while (1) {
		DetectPitchFp(ctx, mctx, argv[1]);
		ResetMachineFp(mctx);
		break;// for debug
	}
	CleanOsakanaFpFft(ctx);

	DestroyPeakDetectMachineContextFp(mctx);
#else
	MachineContext_t* mctx = NULL;
	mctx = CreatePeakDetectMachineContext();

	OsakanaFftContext_t* ctx;
	if (InitOsakanaFft(&ctx, N, LOG2N) != 0) {
		DLOG("InitOsakanaFpFft error");
		return 1;
	}

	while (1) {
		DetectPitch(ctx, mctx, argv[1]);
		ResetMachine(mctx);
		break;// for debug
	}

	DestroyPeakDetectMachineContext(mctx);
#endif

	return 0;
}

