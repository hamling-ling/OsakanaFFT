#include "stdafx.h"
#include "CppUnitTest.h"
#include "OsakanaPitchDetection.h"
#include "OsakanaPitchDetectionFp.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static int readFpData(Fp_t* data, uint8_t stride, const int dataNum, Fp_t* rawdata_min, Fp_t* rawdata_max)
{

	return 0;
}

static int readData(float* data, uint8_t stride, const int dataNum)
{

	return 0;
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
			PitchDetectorFp detector;
			detector.Initialize(readFpData);
			detector.DetectPitch();
		}

		TEST_METHOD(TestFft)
		{
			PitchDetector detector;
			detector.Initialize(readData);
			detector.DetectPitch();
		}
	};
}
