#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "OsakanaPitchDetectionFp.h"

osk_fp_complex_t x[N] = { { 0, 0 } };
Fp_t x2[N2] = { 0 };

Fp_t rawdata_min = 512;
Fp_t rawdata_max = 0;

using namespace std;

static int readDataFp(const string& filename, Fp_t* data, uint8_t stride, const int dataNum)
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
		rawdata_min = std::min(*data, rawdata_min);
		rawdata_max = std::max(*data, rawdata_max);
		data += stride;
		counter++;
	}

	file.close();
	return 0;
}

int GetSourceSignalShiftScale(Fp_t amplitude)
{
	//Fp2CStr(amplitude, debug_output_buf_, sizeof(debug_output_buf_));
	//printf("amp %s\n", debug_output_buf_);

	if (amplitude > FLOAT2FP(0.5)) {
		return 0;
	} else 	if (amplitude > FLOAT2FP(0.25)) {
		return 1;
	} else if (amplitude > FLOAT2FP(0.125)) {
		return 2;
	} else if (amplitude > FLOAT2FP(0.0625)) {
		return 3;
	}
	return 4;
}

static inline Fp_t ScaleRawData(Fp_t rawData, int extraShft = 0) {
	rawData = rawData & 0x00003FF;
	rawData -= 512;// center to 0 and make it signed
	return (Fp_t)(rawData << (FPSHFT - 9 + extraShft));// div 512 then shift
}

int DetectPitchFp(OsakanaFpFftContext_t* ctx, MachineContextFp_t* mctx, const string& filename)
{
	// sampling from analog pin
	DLOG("sampling...");
	readDataFp(filename, &x[0].re, 2, N_ADC);
	DLOG("sampled");

	DLOG("raw data --");
	DRAWDATA(x, 0);

	DLOG("normalizing...");
	{
		Fp_t amplitude = std::max(abs(ScaleRawData(rawdata_max)), abs(ScaleRawData(rawdata_min)));
		if (amplitude <= FLOAT2FP(0.0625)) {
			return 1;
		}
		int extraShift = GetSourceSignalShiftScale(amplitude);
		for (int i = 0; i < N2; i++) {
			x[i].re = ScaleRawData(x[i].re, extraShift);
			x[i].im = 0;
			x[N2 + i].re = 0;
			x[N2 + i].im = 0;
			x2[i] = FpMul(x[i].re, x[i].re);
			x2[i] = x2[i] >> SC_X2;
		}
	}
	DLOG("normalized");

	DLOG("-- normalized input signal");
	DCOMPLEXFp(x, DEBUG_OUTPUT_NUM);

	DLOG("-- fft/N");
	OsakanaFpFft(ctx, x, 1); // 1 means scaling. (this x) = (nromal x) >> LOG2N
	DCOMPLEXFp(x, DEBUG_OUTPUT_NUM);

	DLOG("-- power spectrum");
	for (int i = 0; i < N; i++) {
		//Fp_t re = FpMul(x[i].re, x[i].re) + 
		//		  FpMul(x[i].im, x[i].im); // (this x) = (normal x) >> LOG2N*2
		//x[i].re = (Fp_t)(re << (SC_PW)); // x = x >> (LOG2N*2-SC_PW)
		FpW_t re = x[i].re * x[i].re + x[i].im * x[i].im;
		x[i].re = (Fp_t)(re >> (FPSHFT - SC_PW));
		x[i].im = 0;
	}
	DCOMPLEXFp(x, DEBUG_OUTPUT_NUM);

	DLOG("-- IFFT");
	OsakanaFpIfft(ctx, x, 1);// 1 means not *N scaling
	DCOMPLEXFp(x, DEBUG_OUTPUT_NUM);

	// following loop compute :
	// _m[t] = _m[t - 1] + 2 * (- x2[t - 1] + x2[t]);// why 2?
	// where [0] = x[0].re * 2
	// nsdf[t] = 2 * x[t].re / m[t]
	Fp_t m_old = (x[0].re << 1);// why 2?
	Fp_t x2_old = x2[0];
	Fp_t* _nsdf = x2;// reuse memory
	
	Fp_t mt = m_old;
	_nsdf[0] = FpDiv(x[0].re, mt);
	_nsdf[0] = _nsdf[0] << 1;
	// curve analysis
	InputFp(mctx, _nsdf[0]);

	for (int t = 1; t < N2; t++) {
		//_m[t] = _m[t - 1] - x2[t - 1]
		Fp_t m = m_old - x2_old;

		// prepare for next loop
		x2_old = x2[t];
		m_old = m;

		// nsdf
		Fp_t mt = m_old;
		_nsdf[t] = FpDiv(x[t].re, mt);
		_nsdf[t] = _nsdf[t] << 1;

		// curve analysis
		InputFp(mctx, _nsdf[t]);
	}

	DLOG("-- _nsdf");
	DFPSFp(_nsdf, DEBUG_OUTPUT_NUM);

	PeakInfoFp_t keyMaximums[4] = { 0 };
	int keyMaxLen = 0;
	GetKeyMaximumsFp(mctx, FLOAT2FP(0.5f), keyMaximums, sizeof(keyMaximums)/sizeof(PeakInfoFp_t), &keyMaxLen);
	if (0 < keyMaxLen) {
		Fp_t delta = 0;
		if (ParabolicInterpFp(mctx, keyMaximums[0].index, _nsdf, N2, &delta)) {
			char printbuf[64] = { '\0' };
			Fp2CStr(delta, printbuf, sizeof(printbuf));
			printf("delta %s\n", printbuf);
			//Fp2CStr(keyMaximums[0].value, debug_output_buf_, sizeof(debug_output_buf_));
			//printf("nsdf=%s\n", debug_output_buf_);
		}

		// want freq = FREQ_PER_SAMPLE / (index+delta)
		// idx1024=1024*index
		int32_t idx1024 = keyMaximums[0].index << 10;
		// int expression of 1024*delta
		int32_t delta1024 = (delta >> (FPSHFT - 10));
		idx1024 += delta1024;
		// freq = freq_per_sample / idx
		int32_t freq = (FREQ_PER_1024SAMPLE + (idx1024 >> 1)) / idx1024;
		
		//int32_t idx = (idx1024 + 512) >> 10;
		//uint8_t note = kNoteTable[idx] % 12;
		int32_t idx8 = (idx1024 + 4) >> 7;
		uint8_t note = kNoteTable8[idx8] % 12;

		DLOG("freq=%u Hz, note=%s\n", freq, kNoteStrings[note]);
	}

	DLOG("finished");
	return 0;
}

