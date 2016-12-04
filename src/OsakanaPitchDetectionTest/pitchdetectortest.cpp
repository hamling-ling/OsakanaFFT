#include "stdafx.h"

#define _USE_MATH_DEFINES
#include <cmath>

#include "CppUnitTest.h"
#include "OsakanaPitchDetection.h"
#include "OsakanaPitchDetectionFp.h"

#include <algorithm>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <ctime>

using namespace std;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

float g_data[N] = { 0.0f };

vector<pair<uint8_t, float> > g_expValues = {
	{ 60,	261.6f },	{ 61,	277.2f },	{ 62,	293.7f },	{ 63,	311.1f },
	{ 64,	329.6f },	{ 65,	349.2f },	{ 66,	370.0f },	{ 67,	392.0f },
	{ 68,	415.3f },	{ 69,	440.0f },	{ 70,	466.2f },	{ 71,	493.9f },
	{ 72,	523.3f },	{ 73,	554.4f },	{ 74,	587.3f },	{ 75,	622.3f },
	{ 76,	659.3f },	{ 77,	698.5f },	{ 78,	740.0f },	{ 79,	784.0f },
	{ 80,	830.6f },	{ 81,	880.0f },	{ 82,	932.3f },	{ 83,	987.8f }
};

vector<pair<uint8_t, float> > g_expLowValues = {
	{ 36,	65.4 },		{ 37,	69.3 },		{ 38,	73.4 },		{ 39,	77.8 },
	{ 40,	82.4 },		{ 41,	87.3 },		{ 42,	92.5 },		{ 43,	98.0 },
	{ 44,	103.8 },	{ 45,	110.0 },	{ 46,	116.5 },	{ 47,	123.5 },
	{ 48,	130.8 },
};

vector<pair<uint8_t, float> > g_expHighValues = {
	{ 84,	1046.5 },	{ 85,	1108.7 },	{ 86,	1174.7 },	{ 87,	1244.5 },
	{ 88,	1318.5 },	/*{ 89,	1396.9 },	{ 90,	1480.0 },	{ 91,	1568.0 },
	{ 92,	1661.2 },	{ 93,	1760.0 },	{ 94,	1864.7 },	{ 95,	1975.5 },
	{ 96,	2093.0 },*/
};

static int readFpData(Fp_t* data, uint8_t stride, const int dataNum, Fp_t* rawdata_min, Fp_t* rawdata_max)
{
	*rawdata_min = 512;
	*rawdata_max = 0;

	int counter = 0;
	while (counter < dataNum) {
		float x = g_data[counter];

		*data = static_cast<int16_t>(x);
		*rawdata_min = std::min(*data, *rawdata_min);
		*rawdata_max = std::max(*data, *rawdata_max);
		data += stride;
		counter++;
	}
	return 0;
}

static int readData(float* data, uint8_t stride, const int dataNum)
{
	int counter = 0;
	while (counter < dataNum) {
		float x = g_data[counter];

		*data = x;
		data += stride;
		counter++;
	}
	return 0;
}

static void CreateSineData(float* x, float freq, float ampScale)
{
	// time of N samples
	float tn = (T1024_1024 / 1024.0f) * (N / 1024.0f);
	// wave number in N
	float wn = freq * tn;
	for (int i = 0; i < N; i++) {
		x[i] = (float)sin(wn * i/N * 2.0 * M_PI) * ampScale;
		x[i] += 1.0f;
		x[i] *= 512.0f;
	}
}

/**
 *	val=[0,1]
 */
static void CreateSteadyData(float* x, float val)
{
	for (int i = 0; i < N; i++) {
		x[i] = val;
		x[i] += 1.0f;
		x[i] *= 512.0f;
	}
}

static void CreateRandomData(float* x, float scale)
{
	srand(time(NULL));

	for (int i = 0; i < N; i++) {
		x[i] = rand()%1024;
	}

}
namespace OsakanaPitchDetectionTest
{
	void testFpFftWithFreq(float ampScale, float freq, uint8_t note)
	{
		CreateSineData(g_data, freq, ampScale);

		PitchDetectorFp detector;
		detector.Initialize(readFpData);

		PitchInfo_t pitchInfo;
		int result = detector.DetectPitch(&pitchInfo);

		Assert::AreEqual(result, 0);
		Assert::AreEqual(pitchInfo.midiNote, (uint8_t)note);

		bool isFreqInRange = false;
		if (freq - 1.0f <= (float)pitchInfo.freq && (float)pitchInfo.freq <= freq + 1.0f) {
			isFreqInRange = true;
		}
		Assert::IsTrue(freq);

		stringstream ss;
		ss << "freq=" << pitchInfo.freq << ", midiNote=" << (int)pitchInfo.midiNote << endl;
		Logger::WriteMessage(ss.str().c_str());
	}

	TEST_CLASS(PitchDetectionTest)
	{
	public:
		
		TEST_METHOD(TestDestructWithoutInit)
		{
			{
				PitchDetectorFp detector;
			}
			{
				PitchDetector detector;
			}
		}

		TEST_METHOD(TestCleanupWithoutInit)
		{
			{
				PitchDetectorFp detector;
				detector.Cleanup();
			}
			{
				PitchDetector detector;
				detector.Cleanup();
			}
		}

		TEST_METHOD(TestFpFft)
		{
			testFpFftWithFreq(1.0f, 261.6f, 60);
		}

		TEST_METHOD(TestFpFftRangeScale1)
		{
			for (auto x : g_expValues) {
				testFpFftWithFreq(1.0, x.second, x.first);
			}
		}

		TEST_METHOD(TestFpFftRangeScale05)
		{
			for (auto x : g_expValues) {
				testFpFftWithFreq(0.5, x.second, x.first);
			}
		}
#if defined(SCALE_0250)
		TEST_METHOD(TestFpFftRangeScale025)
		{
			for (auto x : g_expValues) {
				testFpFftWithFreq(0.25, x.second, x.first);
			}
		}
#endif
#if defined(SCALE_1025)
		TEST_METHOD(TestFpFftRangeScale0125)
		{
			for (auto x : g_expValues) {
				testFpFftWithFreq(0.125, x.second, x.first);
			}
		}
#endif
		TEST_METHOD(TestFpFftLowRangeScale1)
		{
			for (auto x : g_expLowValues) {
				testFpFftWithFreq(1.0, x.second, x.first);
			}
		}

		TEST_METHOD(TestFpFftLowRangeScale05)
		{
			for (auto x : g_expLowValues) {
				testFpFftWithFreq(0.5, x.second, x.first);
			}
		}
#if defined(SCALE_0250)
		TEST_METHOD(TestFpFftLowRangeScale025)
		{
			for (auto x : g_expLowValues) {
				testFpFftWithFreq(0.25, x.second, x.first);
			}
		}
#endif
#if defined(SCALE_1025)
		TEST_METHOD(TestFpFftLowRangeScale0125)
		{
			for (auto x : g_expLowValues) {
				testFpFftWithFreq(0.125, x.second, x.first);
			}
		}
#endif
		TEST_METHOD(TestFpFftHightRangeScale1)
		{
			for (auto x : g_expHighValues) {
				testFpFftWithFreq(1.0, x.second, x.first);
			}
		}

		TEST_METHOD(TestFpFftHightRangeScale05)
		{
			for (auto x : g_expHighValues) {
				testFpFftWithFreq(0.5, x.second, x.first);
			}
		}
#if defined(SCALE_0250)
		TEST_METHOD(TestFpFftHightRangeScale025)
		{
			for (auto x : g_expHighValues) {
				testFpFftWithFreq(0.25, x.second, x.first);
			}
		}
#endif
#if defined(SCALE_1025)
		TEST_METHOD(TestFpFftHightRangeScale0125)
		{
			for (auto x : g_expHighValues) {
				testFpFftWithFreq(0.125, x.second, x.first);
			}
		}
#endif
		TEST_METHOD(TestFpFftRangeScaleTooSmall)
		{
			CreateSineData(g_data, 261.6f, 0.03125f);

			PitchDetectorFp detector;
			detector.Initialize(readFpData);

			PitchInfo_t pitchInfo = MakePitchInfo();

			int result = detector.DetectPitch(&pitchInfo);

			Assert::AreEqual(result, 1);
			Assert::AreEqual(pitchInfo.midiNote, (uint8_t)0);
			Assert::AreEqual((int)pitchInfo.freq, 0);
			Assert::AreEqual((int)pitchInfo.volume, 0);
		}

		TEST_METHOD(TestFpFftContinuous)
		{
			PitchDetectorFp detector;
			detector.Initialize(readFpData);

			for (auto x : g_expValues) {
				uint8_t expNote = x.first;
				float freq = x.second;

				CreateSineData(g_data, freq, 0.5);

				PitchInfo_t pitchInfo;
				int result = detector.DetectPitch(&pitchInfo);

				Assert::AreEqual(result, 0);
				Assert::AreEqual(pitchInfo.midiNote, expNote);
			}
		}

		TEST_METHOD(TestFpFftLow)
		{
			testFpFftWithFreq(1.0f, 82.4, 40);
		}

		TEST_METHOD(TestFft)
		{
			CreateSineData(g_data, 261.6f, 1.0f);
			PitchDetector detector;
			detector.Initialize(readData);

			PitchInfo_t pitchInfo;
			int result = detector.DetectPitch(&pitchInfo);

			Assert::AreEqual(result, 0);
			Assert::AreEqual(pitchInfo.midiNote, (uint8_t)60);
		}

		TEST_METHOD(TestVolume512)
		{
			CreateSineData(g_data, 261.6f, 0.5f);
			PitchDetectorFp detector;
			detector.Initialize(readFpData);

			PitchInfo_t pitchInfo;
			int result = detector.DetectPitch(&pitchInfo);

			Assert::AreEqual(result, 0);
			Assert::AreEqual(pitchInfo.midiNote, (uint8_t)60);

			bool isAround512 = (500 < pitchInfo.volume && pitchInfo.volume < 520);
			Assert::IsTrue(isAround512);
		}

		TEST_METHOD(TestVolume1024)
		{
			CreateSineData(g_data, 261.6f, 1.0f);
			PitchDetectorFp detector;
			detector.Initialize(readFpData);

			PitchInfo_t pitchInfo;
			int result = detector.DetectPitch(&pitchInfo);

			Assert::AreEqual(result, 0);
			Assert::AreEqual(pitchInfo.midiNote, (uint8_t)60);

			bool isAround512 = (1020 < pitchInfo.volume && pitchInfo.volume < 1030);
			Assert::IsTrue(isAround512);
		}

		TEST_METHOD(TestFpPitchDiagnostic)
		{
			CreateSineData(g_data, 261.6f, 1.0f);
			PitchDetectorFp detector;
			detector.Initialize(readFpData);

			PitchInfo_t pitchInfo = MakePitchInfo();

			int result = detector.DetectPitch(&pitchInfo);

			Assert::AreEqual(result, 0);
			Assert::AreEqual(pitchInfo.midiNote, (uint8_t)60);
			Assert::AreEqual(pitchInfo.pitch, (int8_t)0);
		}

		TEST_METHOD(TestFpPitchDiagnoseLow)
		{
			float freq = sqrt(261.6f*246.0f) + 2.0f;
			CreateSineData(g_data, freq, 1.0f);
			PitchDetectorFp detector;
			detector.Initialize(readFpData);

			PitchInfo_t pitchInfo = MakePitchInfo();

			int result = detector.DetectPitch(&pitchInfo);

			Assert::AreEqual(result, 0);
			Assert::AreEqual(pitchInfo.midiNote, (uint8_t)60);
			Assert::AreEqual(pitchInfo.pitch, (int8_t)-1);
		}

		TEST_METHOD(TestFpPitchDiagnoseHigh)
		{
			float freq = sqrt(261.6f*277.2f) - 2.0f;
			CreateSineData(g_data, freq, 1.0);
			PitchDetectorFp detector;
			detector.Initialize(readFpData);

			PitchInfo_t pitchInfo = MakePitchInfo();

			int result = detector.DetectPitch(&pitchInfo);

			Assert::AreEqual(result, 0);
			Assert::AreEqual(pitchInfo.midiNote, (uint8_t)60);
			Assert::AreEqual(pitchInfo.pitch, (int8_t)1);
		}

		TEST_METHOD(TestFpNoneInput)
		{
			PitchDetectorFp detector;
			detector.Initialize(readFpData);

			PitchInfo_t pitchInfo = MakePitchInfo();

			CreateSteadyData(g_data, 0.0f);

			int result = detector.DetectPitch(&pitchInfo);

			Assert::AreNotEqual(result, 0);
			Assert::AreEqual(pitchInfo.midiNote, (uint8_t)0);
			Assert::AreEqual(pitchInfo.pitch, (int8_t)0);
		}

		TEST_METHOD(TestFpPositiveConstantInput)
		{
			PitchDetectorFp detector;
			detector.Initialize(readFpData);

			PitchInfo_t pitchInfo = MakePitchInfo();

			CreateSteadyData(g_data, 1.0f);

			int result = detector.DetectPitch(&pitchInfo);

			Assert::AreNotEqual(result, 0);
			Assert::AreEqual(pitchInfo.midiNote, (uint8_t)0);
			Assert::AreEqual(pitchInfo.pitch, (int8_t)0);
		}

		TEST_METHOD(TestFpNegativeConstantInput)
		{
			PitchDetectorFp detector;
			detector.Initialize(readFpData);

			PitchInfo_t pitchInfo = MakePitchInfo();

			CreateSteadyData(g_data, -1.0f);

			int result = detector.DetectPitch(&pitchInfo);

			Assert::AreNotEqual(result, 0);
			Assert::AreEqual(pitchInfo.midiNote, (uint8_t)0);
			Assert::AreEqual(pitchInfo.pitch, (int8_t)0);
		}

		TEST_METHOD(TestFpRandomInput)
		{
			PitchDetectorFp detector;
			detector.Initialize(readFpData);

			PitchInfo_t pitchInfo = MakePitchInfo();

			int result = 1;
			for (int i = 0; i < 64; i++) {
				CreateRandomData(g_data, 1.0);

				int result = detector.DetectPitch(&pitchInfo);
				if (result == 0) {
					Assert::AreNotEqual(result, 0);
					Assert::AreNotEqual(pitchInfo.midiNote, (uint8_t)0);
					bool isValidPitch = false;
					if (pitchInfo.pitch < -1 || 1 < pitchInfo.pitch) {
						isValidPitch = true;
					}
					Assert::IsTrue(isValidPitch);
				}
			}
		}

		TEST_METHOD(TestFpRandomSmallInput)
		{
			PitchDetectorFp detector;
			detector.Initialize(readFpData);

			PitchInfo_t pitchInfo = MakePitchInfo();

			int result = 1;
			for (int i = 0; i < 64; i++) {
				CreateRandomData(g_data, 0.3);

				int result = detector.DetectPitch(&pitchInfo);
				if (result == 0) {
					Assert::AreNotEqual(result, 0);
					Assert::AreNotEqual(pitchInfo.midiNote, (uint8_t)0);
					bool isValidPitch = false;
					if (pitchInfo.pitch < -1 || 1 < pitchInfo.pitch) {
						isValidPitch = true;
					}
					Assert::IsTrue(isValidPitch);
				}
			}
		}

		TEST_METHOD(TestFpPositivePulseInput)
		{
			PitchDetectorFp detector;
			detector.Initialize(readFpData);

			PitchInfo_t pitchInfo = MakePitchInfo();

			CreateSteadyData(g_data, 0.0f);
			for (int i = 0; i < 5; i++) {
				g_data[N_ADC / 4 + i] = 1023;
			}

			int result = detector.DetectPitch(&pitchInfo);
			Assert::AreNotEqual(result, 0);
			Assert::AreEqual(pitchInfo.midiNote, (uint8_t)0);
			Assert::AreEqual(pitchInfo.pitch, (int8_t)0);
		}

		TEST_METHOD(TestFpNegativePulseInput)
		{
			PitchDetectorFp detector;
			detector.Initialize(readFpData);

			PitchInfo_t pitchInfo = MakePitchInfo();

			CreateSteadyData(g_data, 1.0f);
			for (int i = 0; i < 5; i++) {
				g_data[N_ADC / 4 + i] = 0;
			}

			int result = detector.DetectPitch(&pitchInfo);
			Assert::AreNotEqual(result, 0);
			Assert::AreEqual(pitchInfo.midiNote, (uint8_t)0);
			Assert::AreEqual(pitchInfo.pitch, (int8_t)0);
		}
	};
}
