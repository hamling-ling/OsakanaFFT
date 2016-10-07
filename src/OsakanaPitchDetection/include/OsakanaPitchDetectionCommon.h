#ifndef _OSAKANAPITCHDETECTIONCOMMON_H_
#define _OSAKANAPITCHDETECTIONCOMMON_H_

#include "OsakanaFft.h"
#include "OsakanaFp.h"
#include "OsakanaFpFft.h"

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
#if 0	// HRM1017
#define N					256		// fft sampling num(last half is 0 pad)
#define LOG2N				8		// log2(N)
#define T1024_1024			(80.73633333333335f)	// adc speed(time to take 1024x1024 samples in sec)
#define T_PER_SAMPLE		FLOAT2FP(7.699616750081382e-05)	// factor to compute index to freq
#define FREQ_PER_SAMPLE		(12988)			// 12987.659418105848 casted to int
#define FREQ_PER_1024SAMPLE	13299363
#endif
#if 1	// GR-KURUMI with min,max
#define N					256		// fft sampling num(last half is 0 pad)
#define LOG2N				8		// log2(N)
#define T1024_1024			(77.3946666667)	// adc speed(time to take 1024x1024 samples in sec)
#define T_PER_SAMPLE		FLOAT2FP(7.38093058268e-05)	// factor to compute index to freq
#define FREQ_PER_SAMPLE		(13548)			// rounded
#define FREQ_PER_1024SAMPLE	13873589		// rounded 
#endif

#define N2					(N/2)	// sampling num of analog input
#define N_ADC				N2

// To avoid overflow
// FFT result scaled to 1/N (bit shift used by 1>>LOG2N)
// 1/N FFT result power spectrum 1/(N^2) which is too small
// we scale the power spectrum result by 1 << SC_PW.
// The power spectrum result is finally scaled by 1 >> (LOG2N*2-SC_PW)
// Other values used with the scaled power spectrum also required to scaled.
// Here the value is only x2 and it should be scaled by 1 >> (LOG2N*2-SC_PW)
#define SC_PW				(LOG2N-7)
#define SC_X2				(LOG2N*2-SC_PW)

// debug
#define DEBUG_OUTPUT_NUM    128

typedef struct PitchInfo_t {
	uint16_t freq;
	uint8_t midiNote;
	const char* noteStr;
} PitchInfo_t;

class BasePitchDetector
{
public:
	BasePitchDetector() {};
	virtual ~BasePitchDetector() {};
	virtual int Initialize(void* readFunc) = 0;
	virtual void Cleanup() = 0;
	virtual int DetectPitch(PitchInfo_t* pitchInfo) = 0;
};

#endif