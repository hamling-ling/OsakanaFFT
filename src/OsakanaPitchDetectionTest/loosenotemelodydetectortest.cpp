#include "stdafx.h"
#include "CppUnitTest.h"
#include "../OsakanaPitchDetection/include/EdgeDetector.h"
#include "../OsakanaPitchDetection/include/MelodyCommandReceiver.h"
#include "../OsakanaPitchDetection/include/ResponsiveMelodyDetector.h"
#include "../OsakanaPitchDetection/include/LooseNoteMelodyDetector.h"
#include "../OsakanaPitchDetection/src/ContinuityDetector.h"
#include "../OsakanaPitchDetection/src/VolumeComparator.h"
#include "../OsakanaPitchDetection/src/LooseNoteDetector.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace OsakanaPitchDetectionTest
{
	// fuga G D Bb A G Bb A G F# A D
	static uint16_t s_fug_mel0[] = { 67, 74, 70, 69 };// G D Bb A 
	static uint16_t s_fug_mel1[] = { 69, 62 };// A D

	//  G F Bb C F D C F D Bb C G F
	static uint16_t s_pic_mel0[] = { 67, 65, 70, 72 };// G F Bb C 
	static uint16_t s_pic_mel1[] = { 67, 65 };// A D

	unsigned long g_now;

	unsigned long mills()
	{
		return g_now;
	}


	TEST_CLASS(LooseNoteDetectionTest)
	{
	public:

		TEST_METHOD(TestLooseNoteRightInput)
		{
			LooseNoteDetectionResult_t result;
			LooseNoteDetector nd(10, 999, 499, mills);
			g_now = 0;
			result = nd.Input(10);
			Assert::AreEqual<int>(result, kLooseNoteDetectionResultNone);

			g_now = 1000;
			result = nd.Input(0);
			Assert::AreEqual<int>(result, kLooseNoteDetectionResultNone);

			g_now = 1500;
			result = nd.Input(0);
			Assert::AreEqual<int>(result, kLooseNoteDetectionResultDetected);
		}

		TEST_METHOD(TestLooseNoteInvalidInput)
		{
			LooseNoteDetectionResult_t result;
			LooseNoteDetector nd(10, 999, 499, mills);
			g_now = 0;
			result = nd.Input(9);
			Assert::AreEqual<int>(result, kLooseNoteDetectionResultNone);

			g_now = 1000;
			result = nd.Input(0);
			Assert::AreEqual<int>(result, kLooseNoteDetectionResultInvalid);
		}

		TEST_METHOD(TestLooseNoteAttackNoise)
		{
			LooseNoteDetectionResult_t result;
			LooseNoteDetector nd(10, 999, 499, mills);
			g_now = 0;
			result = nd.Input(9);
			Assert::AreEqual<int>(result, kLooseNoteDetectionResultNone);

			g_now = 100;
			result = nd.Input(10);
			Assert::AreEqual<int>(result, kLooseNoteDetectionResultNone);

			g_now = 1100;
			result = nd.Input(0);
			Assert::AreEqual<int>(result, kLooseNoteDetectionResultNone);

			g_now = 1600;
			result = nd.Input(0);
			Assert::AreEqual<int>(result, kLooseNoteDetectionResultDetected);
		}

		TEST_METHOD(TestLooseNoteEndNoise)
		{
			LooseNoteDetectionResult_t result;
			LooseNoteDetector nd(10, 999, 499, mills);
			g_now = 0;
			result = nd.Input(10);
			Assert::AreEqual<int>(result, kLooseNoteDetectionResultNone);

			g_now = 1000;
			result = nd.Input(9);
			Assert::AreEqual<int>(result, kLooseNoteDetectionResultNone);

			g_now = 1500;
			result = nd.Input(0);
			Assert::AreEqual<int>(result, kLooseNoteDetectionResultDetected);
		}

		TEST_METHOD(TestLooseNoteContinuousChange)
		{
			LooseNoteDetectionResult_t result;
			LooseNoteDetector nd(10, 999, 499, mills);
			g_now = 0;
			result = nd.Input(10);
			Assert::AreEqual<int>(result, kLooseNoteDetectionResultNone);

			g_now = 1000;
			result = nd.Input(9);
			Assert::AreEqual<int>(result, kLooseNoteDetectionResultNone);

			g_now = 2000;
			result = nd.Input(9);
			Assert::AreEqual<int>(result, kLooseNoteDetectionResultDetectedWithNextNote);
		}

		TEST_METHOD(TestLooseNoteResetAfterDetection)
		{
			LooseNoteDetectionResult_t result;
			LooseNoteDetector nd(10, 999, 499, mills);
			g_now = 0;
			result = nd.Input(10);
			Assert::AreEqual<int>(result, kLooseNoteDetectionResultNone);

			g_now = 1000;
			result = nd.Input(0);
			Assert::AreEqual<int>(result, kLooseNoteDetectionResultNone);

			g_now = 1500;
			result = nd.Input(0);
			Assert::AreEqual<int>(result, kLooseNoteDetectionResultDetected);

			nd.Reset(9);

			g_now = 0;
			result = nd.Input(9);
			Assert::AreEqual<int>(result, kLooseNoteDetectionResultNone);

			g_now = 1000;
			result = nd.Input(0);
			Assert::AreEqual<int>(result, kLooseNoteDetectionResultNone);

			g_now = 1500;
			result = nd.Input(0);
			Assert::AreEqual<int>(result, kLooseNoteDetectionResultDetected);
		}

		TEST_METHOD(TestLooseNoteResetIntermediate)
		{
			LooseNoteDetectionResult_t result;
			LooseNoteDetector nd(10, 999, 499, mills);
			g_now = 0;
			result = nd.Input(10);
			Assert::AreEqual<int>(result, kLooseNoteDetectionResultNone);

			g_now = 1000;
			result = nd.Input(0);
			Assert::AreEqual<int>(result, kLooseNoteDetectionResultNone);

			g_now = 1100;
			result = nd.Input(0);
			Assert::AreEqual<int>(result, kLooseNoteDetectionResultNone);

			nd.Reset(9);

			g_now = 0;
			result = nd.Input(9);
			Assert::AreEqual<int>(result, kLooseNoteDetectionResultNone);

			g_now = 1000;
			result = nd.Input(0);
			Assert::AreEqual<int>(result, kLooseNoteDetectionResultNone);

			g_now = 1500;
			result = nd.Input(0);
			Assert::AreEqual<int>(result, kLooseNoteDetectionResultDetected);
		}
	};

	TEST_CLASS(LooseNoteMelodyDetectionTest)
	{
	public:

		TEST_METHOD(TestLooseNoteMelodyRightInput)
		{
			LooseNoteMelodyDetector md(s_fug_mel1, _countof(s_fug_mel1));
			md.SetDebugMillsFunc(mills);
			int result = 0;

			g_now = 0;
			result = md.Input(s_fug_mel1[0]);
			Assert::AreEqual<int>(result, 0);

			g_now += 1000;
			result = md.Input(s_fug_mel1[0]);
			Assert::AreEqual<int>(result, 0);

			g_now += 1000;
			result = md.Input(s_fug_mel1[0]);
			Assert::AreEqual<int>(result, 0);

			g_now += 1000;
			result = md.Input(0);
			Assert::AreEqual<int>(result, 0);

			g_now += 1000;
			result = md.Input(s_fug_mel1[1]);
			Assert::AreEqual<int>(result, 0);

			g_now += 1000;
			result = md.Input(s_fug_mel1[1]);
			Assert::AreEqual<int>(result, 0);

			g_now += 1000;
			result = md.Input(s_fug_mel1[1]);
			Assert::AreEqual<int>(result, 0);

			g_now += 1000;
			result = md.Input(0);
			Assert::AreEqual<int>(result, 0);

			g_now += 1000;
			result = md.Input(0);
			Assert::AreEqual<int>(result, 1);
		}
	};
}
