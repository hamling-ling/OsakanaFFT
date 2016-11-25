#include "stdafx.h"
#include "CppUnitTest.h"
#include "../OsakanaPitchDetection/include/EdgeDetector.h"
#include "../OsakanaPitchDetection/include/MelodyCommandReceiver.h"
#include "../OsakanaPitchDetection/include/ResponsiveMelodyDetector.h"
#include "../OsakanaPitchDetection/src/ContinuityDetector.h"
#include "../OsakanaPitchDetection/src/VolumeComparator.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace OsakanaPitchDetectionTest
{
	// fuga G D Bb A G Bb A G F# A D
	static uint16_t s_fug_mel0[] = { 67, 74, 70, 69 };// G D Bb A 
	static uint16_t s_fug_mel1[] = { 69, 62 };// A D

	//  G F Bb C F D C F D Bb C G F
	static uint16_t s_pic_mel0[] = { 67, 65, 70, 72 };// G F Bb C 
	static uint16_t s_pic_mel1[] = { 67, 65 };// A D


	TEST_CLASS(MelodyDetectionTest)
	{
	public:

		TEST_METHOD(TestInstantiation)
		{
			ResponsiveMelodyDetector fd(s_fug_mel0, _countof(s_fug_mel0), s_fug_mel1, _countof(s_fug_mel1));
		}

		TEST_METHOD(TestRigntInput)
		{
			ResponsiveMelodyDetector fd(s_fug_mel0, _countof(s_fug_mel0), s_fug_mel1, _countof(s_fug_mel1));

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
			ResponsiveMelodyDetector fd(s_fug_mel0, _countof(s_fug_mel0), s_fug_mel1, _countof(s_fug_mel1));

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
			Assert::AreEqual(result, 0);

			result = fd.Input(69);
			Assert::AreEqual(result, 0);

			result = fd.Input(0);
			Assert::AreEqual(result, 0);

			result = fd.Input(62);
			Assert::AreEqual(result, 2);
		}

		TEST_METHOD(TestWrongInputResetState)
		{
			ResponsiveMelodyDetector fd(s_fug_mel0, _countof(s_fug_mel0), s_fug_mel1, _countof(s_fug_mel1));
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
						Assert::AreEqual(result, -1);
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
			ResponsiveMelodyDetector fd(s_fug_mel0, _countof(s_fug_mel0), s_fug_mel1, _countof(s_fug_mel1));
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
			ResponsiveMelodyDetector fd(s_fug_mel0, _countof(s_fug_mel0), s_fug_mel1, _countof(s_fug_mel1));
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

		TEST_METHOD(TestMelodyCommandReceiverInstantiation)
		{
			MelodyCommand_t commands[] = {
				{s_fug_mel0,		s_fug_mel1,		_countof(s_fug_mel0) ,		_countof(s_fug_mel1)		},
				{ s_pic_mel0,	s_pic_mel1, _countof(s_pic_mel0) ,	_countof(s_pic_mel1)	},
			};
			MelodyCommandReceiver mcr(commands, _countof(commands));
		}

		TEST_METHOD(TestMelodyCommandReceiverRightInput)
		{
			MelodyCommand_t commands[] = {
				{ s_fug_mel0,		s_fug_mel1,		_countof(s_fug_mel0) ,		_countof(s_fug_mel1) },
				{ s_pic_mel0,	s_pic_mel1, _countof(s_pic_mel0) ,	_countof(s_pic_mel1) },
			};

			MelodyCommandReceiver mcr(commands, _countof(commands));

			MelodyCommandResponse_t resp;
			for (int i = 0; i < _countof(s_fug_mel0); i++) {
				resp = mcr.Input(s_fug_mel0[i]);
				if (i + 1 < _countof(s_fug_mel0)) {
					Assert::IsTrue(resp.IsEmpty());
				}
				else {
					Assert::AreEqual((int)resp.commandIdx, 0);
					Assert::AreEqual((int)resp.evt, (int)kMelodyCommandEvtExcited);
				}
			}

			for (int i = 0; i < _countof(s_fug_mel1); i++) {
				resp = mcr.Input(s_fug_mel1[i]);
				if (i + 1 < _countof(s_fug_mel1)) {
					Assert::IsTrue(resp.IsEmpty());
				}
				else {
					Assert::AreEqual((int)resp.commandIdx, 0);
					Assert::AreEqual((int)resp.evt, (int)kMelodyCommandEvtFired);
				}
			}
		}

		TEST_METHOD(TestMelodyCommandReceiverRightInput2)
		{
			MelodyCommand_t commands[] = {
				{ s_fug_mel0,		s_fug_mel1,		_countof(s_fug_mel0) ,		_countof(s_fug_mel1) },
				{ s_pic_mel0,	s_pic_mel1, _countof(s_pic_mel0) ,	_countof(s_pic_mel1) },
			};

			MelodyCommandReceiver mcr(commands, _countof(commands));

			MelodyCommandResponse_t resp;
			for (int i = 0; i < _countof(s_pic_mel0); i++) {
				resp = mcr.Input(s_pic_mel0[i]);
				if (i + 1 < _countof(s_pic_mel0)) {
					Assert::IsTrue(resp.IsEmpty());
				}
				else {
					Assert::AreEqual((int)resp.commandIdx, 1);
					Assert::AreEqual((int)resp.evt, (int)kMelodyCommandEvtExcited);
				}
			}

			for (int i = 0; i < _countof(s_pic_mel1); i++) {
				resp = mcr.Input(s_pic_mel1[i]);
				if (i + 1 < _countof(s_pic_mel1)) {
					Assert::IsTrue(resp.IsEmpty());
				}
				else {
					Assert::AreEqual((int)resp.commandIdx, 1);
					Assert::AreEqual((int)resp.evt, (int)kMelodyCommandEvtFired);
				}
			}
		}

		TEST_METHOD(TestMelodyCommandReceiverRightInputWithSilence)
		{
			MelodyCommand_t commands[] = {
				{ s_fug_mel0,		s_fug_mel1,		_countof(s_fug_mel0) ,		_countof(s_fug_mel1) },
				{ s_pic_mel0,	s_pic_mel1, _countof(s_pic_mel0) ,	_countof(s_pic_mel1) },
			};

			MelodyCommandReceiver mcr(commands, _countof(commands));

			MelodyCommandResponse_t resp;
			for (int i = 0; i < _countof(s_pic_mel0); i++) {
				resp = mcr.Input(s_pic_mel0[i]);
				mcr.Input(0);
				if (i + 1 < _countof(s_pic_mel0)) {
					Assert::IsTrue(resp.IsEmpty());
				}
				else {
					Assert::AreEqual((int)resp.commandIdx, 1);
					Assert::AreEqual((int)resp.evt, (int)kMelodyCommandEvtExcited);
				}
			}

			for (int i = 0; i < _countof(s_pic_mel1); i++) {
				resp = mcr.Input(s_pic_mel1[i]);
				mcr.Input(0);
				if (i + 1 < _countof(s_pic_mel1)) {
					Assert::IsTrue(resp.IsEmpty());
				}
				else {
					Assert::AreEqual((int)resp.commandIdx, 1);
					Assert::AreEqual((int)resp.evt, (int)kMelodyCommandEvtFired);
				}
			}
		}

		TEST_METHOD(TestMelodyCommandReceiverResetWithWrongNote)
		{
			MelodyCommand_t commands[] = {
				{ s_fug_mel0,		s_fug_mel1,		_countof(s_fug_mel0) ,		_countof(s_fug_mel1) },
				{ s_pic_mel0,	s_pic_mel1, _countof(s_pic_mel0) ,	_countof(s_pic_mel1) },
			};

			MelodyCommandReceiver mcr(commands, _countof(commands));

			MelodyCommandResponse_t resp;
			for (int wrongNotePos = 0; wrongNotePos < _countof(s_pic_mel0); wrongNotePos++) {
				for (int i = 0; i < _countof(s_pic_mel0); i++) {
					resp = mcr.Input(s_pic_mel0[i]);
					mcr.Input(0);
					if (i + 1 < _countof(s_pic_mel0)) {
						Assert::IsTrue(resp.IsEmpty());
					}
					else {
						Assert::AreEqual((int)resp.commandIdx, 1);
						Assert::AreEqual((int)resp.evt, (int)kMelodyCommandEvtExcited);
					}
					
					if(i == wrongNotePos) {
						resp = mcr.Input(3);
						Assert::IsTrue(resp.IsEmpty());
						break;
					}
				}
			}

			for (int wrongNotePos = 0; wrongNotePos < _countof(s_pic_mel1); wrongNotePos++) {

				for (int i = 0; i < _countof(s_pic_mel0); i++) {
					mcr.Input(s_pic_mel0[i]);
				}

				for (int i = 0; i < _countof(s_pic_mel1); i++) {
					resp = mcr.Input(s_pic_mel1[i]);
					mcr.Input(0);
					if (i + 1 < _countof(s_pic_mel1)) {
						Assert::IsTrue(resp.IsEmpty());
					}
					else {
						Assert::AreEqual((int)resp.commandIdx, 1);
						Assert::AreEqual((int)resp.evt, (int)kMelodyCommandEvtFired);
					}

					if (i == wrongNotePos) {
						resp = mcr.Input(3);
						Assert::IsTrue(resp.IsEmpty());
						break;
					}
				}
			}
		}
	};
}
