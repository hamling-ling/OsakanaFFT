#include "stdafx.h"
#include "CppUnitTest.h"
#include "../OsakanaPitchDetection/include/EdgeDetector.h"
#include "../OsakanaPitchDetection/include/MelodyCommandReceiver.h"
#include "../OsakanaPitchDetection/include/ResponsiveMelodyDetector.h"
#include "../OsakanaPitchDetection/include/LooseNoteMelodyDetector.h"
#include "../OsakanaPitchDetection/src/ContinuityDetector.h"
#include "../OsakanaPitchDetection/src/VolumeComparator.h"
#include "../OsakanaPitchDetection/src/LooseNoteDetector.h"

#include <fstream>
#include <string>
#include <sstream>
#include <vector>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace OsakanaPitchDetectionTest
{
	typedef vector<pair<unsigned long, uint16_t> > input_data_t;

	// fuga G D Bb A G Bb A G F# A D
	static uint16_t s_fug_mel0[] = { 67, 74, 70, 69 };// G D Bb A 
	static uint16_t s_fug_mel1[] = { 69, 62 };// A D

	//  G F Bb C F D C F D Bb C G F
	static uint16_t s_pic_mel0[] = { 67, 65, 70, 72 };// G F Bb C 
	static uint16_t s_pic_mel1[] = { 67, 65 };// A D

	//   E Eb E Eb E B D C A
	static uint16_t s_els_mel0[] = { 76, 75, 76, 75, 76 };// E Eb E Eb E
	static uint16_t s_els_mel1[] = { 69 };// A

	unsigned long g_now;

	unsigned long mills()
	{
		return g_now;
	}


	static int readInputData(const string filename, input_data_t& data)
	{
		ifstream file(filename);
		if (!file.is_open()) {
			stringstream ss;
			ss << "can't open " << filename;
			Logger::WriteMessage(ss.str().c_str());
			return 1;
		}

		string line;
		int counter = 0;
		while (getline(file, line) && counter < 2048) {

			istringstream iss(line);
			/*if (!(iss >> x)) {
				stringstream ss;
				ss << "can't convert " << line << " to float";
				Logger::WriteMessage(ss.str().c_str());
				return 1;
			}*/

			unsigned long t;
			uint16_t val;
			iss >> t >> val;

			data.push_back({ t, val });
			counter++;
		}

		file.close();
		return counter;
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

		TEST_METHOD(TestLooseNoteMelodySiletReset)
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

			// long silent
			for (int i = 0; i < 10; i++) {
				g_now += 1000;
				result = md.Input(0);
				if (i == 0) {
					Assert::AreEqual<int>(result, 0);
				}
				else if (result == -1) {
					Assert::AreEqual<int>(result, -1);
					break;
				}
				// should break befor i reaches 9
				Assert::AreNotEqual(i, 9);
			}

			g_now += 1000;
			result = md.Input(s_fug_mel1[1]);
			Assert::AreEqual<int>(result, 0);

			g_now += 1000;
			result = md.Input(s_fug_mel1[1]);
			Assert::AreEqual<int>(result, -1);
		}

		TEST_METHOD(TestLooseNoteMelodyElise0)
		{
			input_data_t input;
			int readResult = readInputData("melodyinput_elise0.dat", input);
			Assert::IsTrue(readResult > 0);

			LooseNoteMelodyDetector md(s_els_mel1, _countof(s_els_mel1));
			md.SetDebugMillsFunc(mills);
			int result = 0;

			input_data_t::const_iterator it = input.begin();
			while (it != input.end()) {
				g_now = it->first;
				result = md.Input(it->second);
				if (result == 1) {
					break;
				}
				it++;
			}

			Assert::IsTrue(it != input.end());

			while (it != input.end()) {
				g_now = it->first;
				result = md.Input(it->second);
				Assert::AreNotEqual(result, 1);
				if (result == -1) {
					break;
				}
				it++;
			}

			Assert::IsTrue(it != input.end());
		}
	};
}
