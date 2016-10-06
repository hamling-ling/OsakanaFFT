#include "stdafx.h"

#define _USE_MATH_DEFINES
#include <cmath>

#include "CppUnitTest.h"
#include "OsakanaPitchDetection.h"
#include "OsakanaPitchDetectionFp.h"

#include <algorithm>
using namespace std;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

float g_data[N] = { 0.0f };

static int readFpData(Fp_t* data, uint8_t stride, const int dataNum, Fp_t* rawdata_min, Fp_t* rawdata_max)
{
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

static void CreateSineData(float* x, float freq)
{
	// time of N samples
	float tn = (T1024_1024 / 1024.0f) * (N / 1024.0);
	// wave number in N
	float wn = freq * tn;
	for (int i = 0; i < N; i++) {
		x[i] = (float)sin(wn * i/N * 2.0 * M_PI);
		x[i] += 1.0f;
		x[i] *= 512.0f;
	}
}

namespace OsakanaPitchDetectionTest
{	
	TEST_CLASS(UnitTest1)
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
			CreateSineData(g_data, 261.6);

			PitchDetectorFp detector;
			detector.Initialize(readFpData);

			PitchInfo_t pitchInfo;
			int result = detector.DetectPitch(&pitchInfo);

			//Assert::AreEqual(result, 0);
			//Assert::AreEqual(pitchInfo.midiNote, (uint8_t)60);
		}

		TEST_METHOD(TestFft)
		{
			CreateSineData(g_data, 261.6);
			PitchDetector detector;
			detector.Initialize(readData);

			PitchInfo_t pitchInfo;
			int result = detector.DetectPitch(&pitchInfo);

			Assert::AreEqual(result, 0);
			Assert::AreEqual(pitchInfo.midiNote, (uint8_t)60);
		}
	};
}
