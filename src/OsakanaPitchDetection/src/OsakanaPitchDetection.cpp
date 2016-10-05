#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "OsakanaPitchDetection.h"

#define LOG_NEWLINE "\n"
#define LOG_PRINTF	printf
#include "OsakanaFpFftDebug.h"


using namespace std;

osk_fp_complex_t x[N] = { { 0, 0 } };
Fp_t x2[N2] = { 0 };

Fp_t rawdata_min = 512;
Fp_t rawdata_max = 0;

osk_complex_t xf[N] = { { 0, 0 } };
float xf2[N2] = { 0 };
float _mf[N2] = { 0 };

static int readData(const string& filename, float* data, uint8_t stride, const int dataNum)
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
		xf[i].re /= 512.0f;
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

	_mf[0] = xf[0].re * 2.0f;// why 2?
	for (int t = 1; t < N2; t++) {
		_mf[t] = _mf[t - 1] - xf2[t - 1];
	}

	DLOG("-- ms smart");
	DFPS(_mf, DEBUG_OUTPUT_NUM);

	// nsdf
	float* _nsdf = _mf; // reuse buffer
	for (int t = 0; t < N2; t++) {
		float mt = _mf[t]; // add small number to avoid 0 div
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
	GetKeyMaximums(mctx, 0.5f, keyMaximums, sizeof(keyMaximums) / sizeof(PeakInfo_t), &keyMaxLen);
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

