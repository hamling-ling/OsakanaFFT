#include "stdafx.h"
#include "CppUnitTest.h"
#include "../OsakanaPitchDetection/include/EdgeDetector.h"
#include "../OsakanaPitchDetection/include/FugaDetector.h"
#include "../OsakanaPitchDetection/src/ContinuityDetector.h"
#include "../OsakanaPitchDetection/src/VolumeComparator.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace OsakanaPitchDetectionTest
{
	TEST_CLASS(MelodyDetectionTest)
	{
	public:

		TEST_METHOD(TestInstantiation)
		{
			FugaDetector fd;
		}

		TEST_METHOD(TestRigntInput)
		{
			FugaDetector fd;

			bool result = false;
			result = fd.Input(67);
			Assert::IsFalse(result);

			result = fd.Input(74);
			Assert::IsFalse(result);

			result = fd.Input(70);
			Assert::IsFalse(result);

			result = fd.Input(69);
			Assert::IsFalse(result);

			result = fd.Input(69);
			Assert::IsFalse(result);

			result = fd.Input(62);
			Assert::IsTrue(result);
		}

		TEST_METHOD(TestRigntInputWithNoteOff)
		{
			FugaDetector fd;

			bool result = false;
			result = fd.Input(67);
			Assert::IsFalse(result);

			result = fd.Input(0);
			Assert::IsFalse(result);

			result = fd.Input(74);
			Assert::IsFalse(result);

			result = fd.Input(0);
			Assert::IsFalse(result);

			result = fd.Input(70);
			Assert::IsFalse(result);

			result = fd.Input(0);
			Assert::IsFalse(result);

			result = fd.Input(69);
			Assert::IsFalse(result);

			result = fd.Input(0);
			Assert::IsFalse(result);

			result = fd.Input(69);
			Assert::IsFalse(result);

			result = fd.Input(0);
			Assert::IsFalse(result);

			result = fd.Input(62);
			Assert::IsTrue(result);
		}

		TEST_METHOD(TestWrongInputResetState)
		{
			FugaDetector fd;
			uint16_t notes[] = { 67, 74, 70, 69, 69, 62 };

			bool result = false;
			int wrongPos = 0;
			for (int i = 0; i < 5; i++) {
				for (int j = 0; j < 6; j++) {
					result = false;

					result = fd.Input(notes[j]);
					Assert::IsFalse(result);

					if (i == wrongPos) {
						result = fd.Input(3);// wrong input
						Assert::IsFalse(result);
					}
				}
				wrongPos++;
			}

			for (int i = 0; i < 5; i++) {
				result = fd.Input(notes[i]);
				Assert::IsFalse(result);
			}

			result = fd.Input(notes[5]);
			Assert::IsTrue(result);
		}

		TEST_METHOD(TestResetStateAfterRigntInput)
		{
			FugaDetector fd;
			uint16_t notes[] = { 67, 74, 70, 69, 69, 62 };

			bool result = false;
			for (int i = 0; i < 5; i++) {
				result = fd.Input(notes[i]);
				Assert::IsFalse(result);
			}

			result = fd.Input(notes[5]);
			Assert::IsTrue(result);

			result = fd.Input(notes[0]);
			Assert::IsFalse(result);
		}

		TEST_METHOD(TestRepeatRightInput)
		{
			FugaDetector fd;
			uint16_t notes[] = { 67, 74, 70, 69, 69, 62 };

			for (int i = 0; i < 2; i++) {
				bool result = false;
				for (int j = 0; j < 5; j++) {
					result = fd.Input(0);
					Assert::IsFalse(result);

					result = fd.Input(notes[j]);
					Assert::IsFalse(result);

					result = fd.Input(0);
					Assert::IsFalse(result);
				}

				result = fd.Input(0);
				Assert::IsFalse(result);

				result = fd.Input(notes[5]);
				Assert::IsTrue(result);

				result = fd.Input(0);
				Assert::IsFalse(result);
			}
		}
	};
}
