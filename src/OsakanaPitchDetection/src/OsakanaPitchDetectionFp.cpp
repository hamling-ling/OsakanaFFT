#include "OsakanaFpFft.h"
#include "../include/OsakanaPitchDetectionFp.h"
#include "PeakDetectMachineFp.h"
#include "NoteTables.h"

#if defined(ARDUINO_PLATFORM) || defined(RLDUINO78_VERSION) || defined(ARDUINO)      // arduino
#include <Arduino.h>
#define _countof(x) (sizeof(x) / sizeof (x[0]))
#else                               // anything else
#include <algorithm>
using namespace std;
#endif
#include <inttypes.h>

//#define DEBUG_RAGRANGE
#define DEBUG_PITCH

#if defined(DEBUG_RAGRANGE)
#define PRINTRAGRANGE(...)	PrintRagrange(__VA_ARGS__)
#else
#define PRINTRAGRANGE(...)
#endif
#if defined(DEBUG_PITCH)
#define PRINTRESULT(...)	PrintResult(__VA_ARGS__)
#else
#define PRINTRESULT(...)
#endif

osk_fp_complex_t x[N] = { { 0, 0 } };
Fp_t x2[N2] = { 0 };

Fp_t rawdata_min = 512;
Fp_t rawdata_max = 0;

/**
*  amplitude=abs([-1.0,1.0])=[0,1.0]
*/
int GetSourceSignalShiftScale(Fp_t amplitude)
{
	if (amplitude > FLOAT2FP(0.5)) {
		return 0;
	}
	else if (amplitude > FLOAT2FP(0.25)) {
		return 1;
	}
#if defined(SCALE_CUTOFF_0250)
	else if (amplitude > FLOAT2FP(0.125)) {
		return 2;
	}
#if defined(SCALE_CUTOFF_0125)
	else if (amplitude > FLOAT2FP(0.0625)) {
		return 3;
	}
#endif
#endif
	return 4;
}

/**
*  [0,1024] to [-1.0,1.0]
*/
static inline Fp_t ScaleRawData(Fp_t rawData, int extraShft = 0) {
	rawData = rawData & 0x00003FF;
	rawData -= 512;// center to 0 and make it signed
	return (Fp_t)(rawData << (FPSHFT - 9 + extraShft));// div 512 then shift
}

static void PrintResult(uint16_t freq, const char* str, int8_t pitch, uint16_t vol)
{
#if defined(BROKEN_SPRINTF)
	LOG_PRINTF("freq=");
	LOG_PRINTF(freq, DEC);
	LOG_PRINTF(", note=");
	LOG_PRINTF(str);
	LOG_PRINTF(", pitch=");
	LOG_PRINTF(pitch, DEC);
	LOG_PRINTF(", vol=");
	LOG_PRINTF(vol, DEC);
	LOG_PRINTF(LOG_NEWLINE);
#else
	ILOG("freq=%u Hz, note=%s, pitch=%d\n", freq, str, (int)pitch);
#endif
}

static void PrintRagrange(uint16_t index, Fp_t prev, Fp_t cent, Fp_t last, Fp_t delta)
{
	printf("index=%u, ", index);

	char printbuf[64] = { '\0' };
	Fp2CStr(prev, printbuf, sizeof(printbuf));
	printf("[-1]=%s, ", printbuf);

	Fp2CStr(cent, printbuf, sizeof(printbuf));
	printf("[ 0]=%s, ", printbuf);

	Fp2CStr(last, printbuf, sizeof(printbuf));
	printf("[+1]=%s, ", printbuf);

	Fp2CStr(delta, printbuf, sizeof(printbuf));
	printf("delta %s\n", printbuf);
}

PitchDetectorFp::PitchDetectorFp()
	: _fft(NULL), _det(NULL), _func(NULL)
{
}

PitchDetectorFp::~PitchDetectorFp()
{
	Cleanup();
}

int PitchDetectorFp::Initialize(void* readFunc)
{
	_det = CreatePeakDetectMachineContextFp(N_NSDF);

	if (InitOsakanaFpFft(&_fft, N, LOG2N) != 0) {
		DLOG("InitOsakanaFpFft error");
		return 1;
	}
	_func = (ReadFpDataFunc_t)readFunc;

	return 0;
}

void PitchDetectorFp::Cleanup()
{
	CleanOsakanaFpFft(_fft);
	DestroyPeakDetectMachineContextFp(_det);
	_fft = NULL;
	_det = NULL;
}

int PitchDetectorFp::DetectPitch(PitchInfo_t* pitchInfo)
{
	int ret = 1;
	ResetMachineFp(_det);

	// sampling from analog pin
	DLOG("sampling...");
	_func(&x[0].re, 2, N_ADC, &rawdata_min, &rawdata_max);
	DLOG("sampled");

	DLOG("raw data --");
	DRAWDATA(x, DEBUG_OUTPUT_NUM);

	DLOG("normalizing...");
	{
		if (500 < rawdata_min || rawdata_max < 524) {
			return 1;
		}
		Fp_t amplitude = max(abs(ScaleRawData(rawdata_max)), abs(ScaleRawData(rawdata_min)));
		int extraShift = GetSourceSignalShiftScale(amplitude);
		if (4 <= extraShift) {
			return 1;
		}
		pitchInfo->volume = rawdata_max - rawdata_min;

		for (int i = 0; i < N2; i++) {
			x[i].re = ScaleRawData(x[i].re, extraShift);
			x[i].im = 0;
			x[N2 + i].re = 0;
			x[N2 + i].im = 0;
# if 0
			x2[i] = FpMul(x[i].re, x[i].re);
			x2[i] = x2[i] >> SC_X2;
#else
			// SC_X2 right shift is too small to ignore.
			x2[i] = 0;
#endif
		}
	}
	DLOG("normalized");

	DLOG("-- normalized input signal");
	DCOMPLEXFp(x, DEBUG_OUTPUT_NUM);

	DLOG("-- fft/N");
	// 1 means 1/N scaling. resulting x = (nonscaled fft's x) >> LOG2N
	OsakanaFpFft(_fft, x, 1);
	DCOMPLEXFp(x, DEBUG_OUTPUT_NUM);

	DLOG("-- power spectrum");
	for (int i = 0; i < N; i++) {
		FpW_t re = (FpW_t)x[i].re * (FpW_t)x[i].re + (FpW_t)x[i].im * (FpW_t)x[i].im;
		x[i].re = (Fp_t)(re >> (FPSHFT - SC_PW));
		//x[i].re = (Fp_t)(re >> (FPSHFT - 5));
		x[i].im = 0;
	}
	DCOMPLEXFp(x, DEBUG_OUTPUT_NUM);

	DLOG("-- IFFT");
	// 0 means * N scaling, 1 means no scalling.
	OsakanaFpIfft(_fft, x, 1);
	DCOMPLEXFp(x, DEBUG_OUTPUT_NUM);

	// compute m[] incrementally
	// _m[t] = _m[t - 1] - x2[t - 1] + x2[N2+t]);
	// where _m[0] = 2 * x[0].re.
	// the 2 comes from x[i]^2+x[i+t]^2=x[0]^2 for i=0
	Fp_t m_old = x[0].re << 1;
	Fp_t x2_old = x2[0];
	Fp_t* _nsdf = x2;// reuse memory

	Fp_t m = m_old;
	if (m == 0) {
		return 1;
	}

	DLOG("-- m");
	DFPFp(m);
	_nsdf[0] = FpDivLeftShift(x[0].re, m, 1);

	// curve analysis
	InputFp(_det, _nsdf[0]);

	for (int t = 1; t < N_NSDF; t++) {
		//cmputing _m[t] = _m[t - 1] - x2[t - 1] + x2[N2+t]
		m = m_old - x2_old;

		// prepare for next loop
		x2_old = x2[t];
		m_old = m;

		// nsdf
		if (m == 0) {
			return 1;
		}
		DFPFp(m);
		_nsdf[t] = FpDivLeftShift(x[t].re, m, 1);

		// curve analysis
		InputFp(_det, _nsdf[t]);
	}

	DLOG("-- _nsdf");
	DFPSFp(_nsdf, DEBUG_OUTPUT_NUM);

	PeakInfoFp_t keyMaximums[4] = { 0 };
	int keyMaxLen = 0;
	GetKeyMaximumsFp(_det, FLOAT2FP(NSDF_THRESHOLD), keyMaximums, sizeof(keyMaximums) / sizeof(PeakInfoFp_t), &keyMaxLen);
	if (0 < keyMaxLen) {
		Fp_t delta = 0;
		uint16_t index = keyMaximums[0].index;
		ParabolicInterpFp(_det, index, _nsdf, N2, &delta);
		PRINTRAGRANGE( index, keyMaximums[0].index, _nsdf[index + 0], _nsdf[index + 1], delta);

		// want freq = FREQ_PER_SAMPLE / (index+delta)
		// idx1024=1024*index
		int32_t idx1024 = (int32_t)(index) << 10;
		// int expression of 1024*delta
		int32_t delta1024 = (delta >> (FPSHFT - 10));
		idx1024 += delta1024;
		// freq = freq_per_sample / idx
		int32_t freq = (FREQ_PER_1024SAMPLE + (idx1024 >> 1)) / idx1024;

		//int32_t idx = (idx1024 + 512) >> 10;
		//uint8_t note = kNoteTable[idx] % 12;
		int32_t idx8 = (idx1024 + 64) >> 7; // 64 is for rounding
		uint8_t note = kNoteTable8[idx8] % 12;

		if (2048 < freq) {
			// should be wrong input
			return 1;
		}
		pitchInfo->freq = (uint16_t)freq;
		pitchInfo->midiNote = kNoteTable8[idx8];
		pitchInfo->noteStr = kNoteStrings[note];
		pitchInfo->pitch = GetAccuracy(pitchInfo->midiNote, idx8);

		PRINTRESULT(freq, kNoteStrings[note], pitchInfo->pitch, pitchInfo->volume);

		ret = 0;
	}

	DLOG("finished");
	return ret;
}

int8_t PitchDetectorFp::GetAccuracy(uint16_t note, uint16_t idx8)
{
	if (_countof(kNoteTable8IndexRange) <= note) {
		return INT8_MIN;
	}

	const NoteTableIndexRange_t* range = &kNoteTable8IndexRange[note];
	uint16_t width = (range->max_idx - range->min_idx);
	if (width < (uint16_t)4) {
		return INT8_MIN;
	}

	uint16_t err = width / 6;
	uint16_t mid = (range->max_idx + range->min_idx) >> 1;
	if (idx8 < mid - err) {
		// smaller index -> higher pitch
		return (int8_t)(1);
	}

	if (mid + err < idx8) {
		// larger index -> lower pitch
		return (int8_t)(-1);
	}
	return (int8_t)(0);
}
