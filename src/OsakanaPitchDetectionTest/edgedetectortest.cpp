#include "stdafx.h"
#include "CppUnitTest.h"
#include "../OsakanaPitchDetection/include/EdgeDetector.h"
#include "../OsakanaPitchDetection/src/ContinuityDetector.h"
#include "../OsakanaPitchDetection/src/VolumeComparator.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace OsakanaPitchDetectionTest
{
	TEST_CLASS(EdgeDetectionTest)
	{
	public:

		TEST_METHOD(TestContinueToTrue)
		{
			ContinuityDetector cd;
			bool result = false;

			int counter = 0;
			for(int i = 0; i < HISTROY_LEN - 1; i++)
			{
				result = cd.Input(1);
				Assert::IsFalse(result);
			}

			result = cd.Input(1);
			Assert::IsTrue(result);

			result = cd.Input(1);
			Assert::IsTrue(result);
		}

		TEST_METHOD(TestTrueToFalse)
		{
			ContinuityDetector cd;
			bool result = false;
			for (int i = 0; i < HISTROY_LEN - 1; i++) {
				result = cd.Input(1);
				Assert::IsFalse(result);
			}

			result = cd.Input(1);
			Assert::IsTrue(result);

			for (int i = 0; i < HISTROY_LEN - 1; i++) {
				result = cd.Input(2);
				Assert::IsFalse(result);
			}

			result = cd.Input(2);
			Assert::IsTrue(result);
		}

		TEST_METHOD(TestComparatorOffToOn)
		{
			VolumeComparator vc(64, 32);
			bool result = false;
			result = vc.Input(5);
			Assert::IsFalse(result);

			result = vc.Input(32);
			Assert::IsFalse(result);

			result = vc.Input(33);
			Assert::IsFalse(result);

			result = vc.Input(64);
			Assert::IsTrue(result);

			result = vc.Input(65);
			Assert::IsTrue(result);
		}

		TEST_METHOD(TestComparatorOnToOff)
		{
			VolumeComparator vc(64, 32);
			bool result = false;
			result = vc.Input(64);
			Assert::IsTrue(result);

			result = vc.Input(32);
			Assert::IsTrue(result);

			result = vc.Input(31);
			Assert::IsFalse(result);
		}

		TEST_METHOD(TestEdgeUp)
		{
			EdgeDetector ed;
			bool result = false;
			uint16_t note = 0;

			for (int i = 0; i < HISTROY_LEN - 1; i++)
			{
				result = ed.Input(1, 1024);
				Assert::IsFalse(result);

				note = ed.CurrentNote();
				AreEqualNote(0, note);
			}

			result = ed.Input(1, 1024);
			Assert::IsTrue(result);

			note = ed.CurrentNote();
			AreEqualNote(1, note);

			result = ed.Input(1, 1024);
			Assert::IsFalse(result);

			note = ed.CurrentNote();
			AreEqualNote(1, note);
		}

		TEST_METHOD(TestEdgeDown)
		{
			EdgeDetector ed;
			bool result = false;
			uint16_t note = 0;

			for (int i = 0; i < HISTROY_LEN; i++) {
				result = ed.Input(1, 1023);
			}

			Assert::IsTrue(result);

			note = ed.CurrentNote();
			AreEqualNote(1, note);

			for (int i = 0; i < HISTROY_LEN - 1; i++) {
				result = ed.Input(2, 1023);
				Assert::IsFalse(result);

				note = ed.CurrentNote();
				AreEqualNote(1, note);
			}

			result = ed.Input(2, 1023);
			Assert::IsTrue(result);

			note = ed.CurrentNote();
			AreEqualNote(2, note);
		}

		TEST_METHOD(TestEdgeSmallVolumeWontRaiseEdge)
		{
			EdgeDetector ed;
			bool result = false;
			uint16_t note = 0;

			for (int i = 0; i <= HISTROY_LEN; i++)
			{
				result = ed.Input(1, 10);
				Assert::IsFalse(result);

				note = ed.CurrentNote();
				AreEqualNote(0, note);
			}
		}

		TEST_METHOD(TestEdgeSmallToLargeVolume)
		{
			EdgeDetector ed;
			bool result = false;
			uint16_t note = 0;

			for (int i = 0; i < HISTROY_LEN; i++) {
				result = ed.Input(1, 10);
				Assert::IsFalse(result);

				note = ed.CurrentNote();
				AreEqualNote(0, note);
			}

			for (int i = 0; i < HISTROY_LEN - 1; i++) {
				result = ed.Input(1, 1024);
				Assert::IsFalse(result);

				note = ed.CurrentNote();
				AreEqualNote(0, note);
			}

			result = ed.Input(1, 1024);
			Assert::IsTrue(result);

			note = ed.CurrentNote();
			AreEqualNote(1, note);

			for (int i = 0; i < 3; i++) {
				result = ed.Input(1, 1024);
				Assert::IsFalse(result);

				note = ed.CurrentNote();
				AreEqualNote(1, note);
			}
		}

		TEST_METHOD(TestVolumeDecay)
		{
			EdgeDetector ed;
			bool result = false;
			uint16_t note = 0;

			result = ed.Input(0, 0);
			Assert::IsFalse(result);
			note = ed.CurrentNote();
			AreEqualNote(0, note);

			for (int i = 0; i < HISTROY_LEN; i++)
			{
				result = ed.Input(0, VOLUME_THRESHOLD_OFF_TO_ON + 10);
				Assert::IsFalse(result);
				note = ed.CurrentNote();
				AreEqualNote(0, note);
			}

			for (int i = 0; i < HISTROY_LEN - 1; i++)
			{
				result = ed.Input(0, VOLUME_THRESHOLD_OFF_TO_ON + 10);
				Assert::IsFalse(result);
				note = ed.CurrentNote();
				AreEqualNote(0, note);
			}

			result = ed.Input(64, VOLUME_THRESHOLD_OFF_TO_ON + 10);
			Assert::IsTrue(result);
			note = ed.CurrentNote();
			AreEqualNote(64, note);

			for (int vol = VOLUME_THRESHOLD_ON_TO_OFF + 10; vol >= VOLUME_THRESHOLD_ON_TO_OFF; vol--)
			{
				result = ed.Input(64, vol);
				Assert::IsFalse(result);
				note = ed.CurrentNote();
				AreEqualNote(64, note);
			}

			result = ed.Input(64, VOLUME_THRESHOLD_ON_TO_OFF-1);
			Assert::IsTrue(result);
			note = ed.CurrentNote();
			AreEqualNote(0, note);

			for (int i = 0; i < 3; i++) {
				result = ed.Input(0, 0);
				Assert::IsFalse(result);
				note = ed.CurrentNote();
				AreEqualNote(0, note);
			}
		}

		TEST_METHOD(TestEdgeReset)
		{
			EdgeDetector ed;
			bool result = false;
			uint16_t note = 0;

			for (int i = 0; i < 2; i++) {
				for (int j = 0; j < HISTROY_LEN - 1; j++) {
					result = ed.Input(1, 1024);
					Assert::IsFalse(result);

					note = ed.CurrentNote();
					AreEqualNote(0, note);
				}
				result = ed.Input(1, 1024);
				Assert::IsTrue(result);

				note = ed.CurrentNote();
				AreEqualNote(1, note);

				result = ed.Input(1, 1024);
				Assert::IsFalse(result);

				note = ed.CurrentNote();
				AreEqualNote(1, note);

				ed.Reset();
			}
		}

		private:
			void AreEqualNote(int expected, uint16_t note)
			{
				Assert::AreEqual(expected, static_cast<int>(note));
			}
	};
}
