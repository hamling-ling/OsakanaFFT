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

			int result = 0;
			result = fd.Input(67);
			Assert::AreEqual(result, 0);

			result = fd.Input(74);
			Assert::AreEqual(result, 0);

			result = fd.Input(70);
			Assert::AreEqual(result, 0);

			result = fd.Input(69);
			Assert::AreEqual(result, 1);

			result = fd.Input(69);
			Assert::AreEqual(result, 0);

			result = fd.Input(62);
			Assert::AreEqual(result, 2);
		}

		TEST_METHOD(TestRigntInputWithNoteOff)
		{
			FugaDetector fd;

			int result = 0;
			result = fd.Input(67);
			Assert::AreEqual(result, 0);

			result = fd.Input(0);
			Assert::AreEqual(result, 0);

			result = fd.Input(74);
			Assert::AreEqual(result, 0);

			result = fd.Input(0);
			Assert::AreEqual(result, 0);

			result = fd.Input(70);
			Assert::AreEqual(result, 0);

			result = fd.Input(0);
			Assert::AreEqual(result, 0);

			result = fd.Input(69);
			Assert::AreEqual(result, 1);

			result = fd.Input(0);
			Assert::IsFalse(result);

			result = fd.Input(69);
			Assert::AreEqual(result, 0);

			result = fd.Input(0);
			Assert::IsFalse(result);

			result = fd.Input(62);
			Assert::AreEqual(result, 2);
		}

		TEST_METHOD(TestWrongInputResetState)
		{
			FugaDetector fd;
			uint16_t notes[] = { 67, 74, 70, 69, 69, 62 };

			int result = 0;
			for (int i = 0; i < 5; i++) {
				for (int j = 0; j < 6; j++) {
					result = false;

					result = fd.Input(notes[j]);
					if (j == 3)
					{
						Assert::AreEqual(result, 1);
					}
					else if (j == 5) {
						Assert::AreEqual(result, 2);
					}
					else
					{
						Assert::AreEqual(result, 0);
					}

					if (j == i) {
						result = fd.Input(3);// wrong input
						Assert::AreEqual(result, 0);
						break;
					}
				}
			}

			for (int i = 0; i < 3; i++) {
				result = fd.Input(notes[i]);
				Assert::AreEqual(result, 0);
			}

			result = fd.Input(notes[3]);
			Assert::AreEqual(result, 1);

			result = fd.Input(notes[4]);
			Assert::AreEqual(result, 0);

			result = fd.Input(notes[5]);
			Assert::AreEqual(result, 2);
		}

		TEST_METHOD(TestResetStateAfterRigntInput)
		{
			FugaDetector fd;
			uint16_t notes[] = { 67, 74, 70, 69, 69, 62 };

			int result = 0;
			for (int i = 0; i < 6; i++) {
				result = fd.Input(notes[i]);
				if (i == 3)
				{
					Assert::AreEqual(result, 1);
				}
				else if (i == 5) {
					Assert::AreEqual(result, 2);
				}
				else
				{
					Assert::AreEqual(result, 0);
				}
			}

			result = fd.Input(notes[0]);
			Assert::AreEqual(result, 0);
		}

		TEST_METHOD(TestRepeatRightInput)
		{
			FugaDetector fd;
			uint16_t notes[] = { 67, 74, 70, 69, 69, 62 };

			for (int i = 0; i < 2; i++) {
				int result = 0;
				for (int i = 0; i < 6; i++) {
					result = fd.Input(notes[i]);
					if (i == 3)
					{
						Assert::AreEqual(result, 1);
					}
					else if (i == 5) {
						Assert::AreEqual(result, 2);
					}
					else
					{
						Assert::AreEqual(result, 0);
					}
					result = fd.Input(0);
					Assert::AreEqual(result, 0);
				}

				result = fd.Input(0);
				Assert::AreEqual(result, 0);
			}
		}
	};
}
