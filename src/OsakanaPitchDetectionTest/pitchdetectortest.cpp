#include "stdafx.h"

#define _USE_MATH_DEFINES
#include <cmath>

#include "CppUnitTest.h"
#include "OsakanaPitchDetection.h"
#include "OsakanaPitchDetectionFp.h"

#include <algorithm>
#include <vector>
#include <sstream>

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

		TEST_METHOD(TestFpFftRangeScale025)
		{
			for (auto x : g_expValues) {
				testFpFftWithFreq(0.25, x.second, x.first);
			}
		}

		TEST_METHOD(TestFpFftRangeScale0125)
		{
			for (auto x : g_expValues) {
				testFpFftWithFreq(0.125, x.second, x.first);
			}
		}

		TEST_METHOD(TestFpFftRangeScaleTooSmall)
		{
			CreateSineData(g_data, 261.6f, 0.03125f);

			PitchDetectorFp detector;
			detector.Initialize(readFpData);

			PitchInfo_t pitchInfo;
			memset(&pitchInfo, 0, sizeof(pitchInfo));

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

			PitchInfo_t pitchInfo;
			memset(&pitchInfo, 0, sizeof(pitchInfo));

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

			PitchInfo_t pitchInfo;
			memset(&pitchInfo, 0, sizeof(pitchInfo));

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

			PitchInfo_t pitchInfo;
			memset(&pitchInfo, 0, sizeof(pitchInfo));

			int result = detector.DetectPitch(&pitchInfo);

			Assert::AreEqual(result, 0);
			Assert::AreEqual(pitchInfo.midiNote, (uint8_t)60);
			Assert::AreEqual(pitchInfo.pitch, (int8_t)1);
		}
	};
}
