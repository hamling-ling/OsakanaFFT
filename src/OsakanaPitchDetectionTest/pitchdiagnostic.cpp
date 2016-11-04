#include "stdafx.h"
#include "CppUnitTest.h"
#include "../OsakanaPitchDetection/include/PitchDiagnostic.h"

#include <tuple>
#include <vector>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace OsakanaPitchDetectionTest
{
	TEST_CLASS(PitchDiagnosticTest)
	{
	public:

		typedef tuple<int8_t, bool, DiagnoseResult_t> param_t;

		TEST_METHOD(TestPItchDiagnosticInstantiation)
		{
			PitchDiagnostic pd(10);
		}

		TEST_METHOD(TestPitchDiagnosticRightGood)
		{
			vector<param_t> params = {
				{ (int8_t)0, true,  DiagnoseResultNone },
				{ (int8_t)0, false, DiagnoseResultNone },
				{ (int8_t)0, false, DiagnoseResultGood },
			};

			RunDiagnoseWithParam(3, params);
		}

		TEST_METHOD(TestPitchDiagnosticRightHigh)
		{
			vector<param_t> params = {
				{ (int8_t)1, true,  DiagnoseResultNone },
				{ (int8_t)1, false, DiagnoseResultNone },
				{ (int8_t)1, false, DiagnoseResultHigh },
			};

			RunDiagnoseWithParam(3, params);
		}

		TEST_METHOD(TestPitchDiagnosticRightLow)
		{
			vector<param_t> params = {
				{ (int8_t)-1, true,  DiagnoseResultNone },
				{ (int8_t)-1, false, DiagnoseResultNone },
				{ (int8_t)-1, false, DiagnoseResultLow },
			};

			RunDiagnoseWithParam(3, params);
		}

		TEST_METHOD(TestPitchDiagnosticKeepGood)
		{
			vector<param_t> params = {
				{ (int8_t)0, true,  DiagnoseResultNone},
				{ (int8_t)0, false, DiagnoseResultNone },
				{ (int8_t)0, false, DiagnoseResultGood },
				{ (int8_t)0, false, DiagnoseResultNone },
				{ (int8_t)0, false, DiagnoseResultNone },
				{ (int8_t)0, false, DiagnoseResultGood },
			};

			RunDiagnoseWithParam(3, params);
		}

		TEST_METHOD(TestPitchDiagnosticKeepHigh)
		{
			vector<param_t> params = {
				{ (int8_t)1, true,  DiagnoseResultNone },
				{ (int8_t)1, false, DiagnoseResultNone },
				{ (int8_t)1, false, DiagnoseResultHigh },
				{ (int8_t)1, false, DiagnoseResultNone },
				{ (int8_t)1, false, DiagnoseResultNone },
				{ (int8_t)1, false, DiagnoseResultHigh },
			};

			RunDiagnoseWithParam(3, params);
		}

		TEST_METHOD(TestPitchDiagnosticKeepLow)
		{
			vector<param_t> params = {
				{ (int8_t)-1, true,  DiagnoseResultNone },
				{ (int8_t)-1, false, DiagnoseResultNone },
				{ (int8_t)-1, false, DiagnoseResultLow },
				{ (int8_t)-1, false, DiagnoseResultNone },
				{ (int8_t)-1, false, DiagnoseResultNone },
				{ (int8_t)-1, false, DiagnoseResultLow },
			};

			RunDiagnoseWithParam(3, params);
		}

		TEST_METHOD(TestPitchDiagnosticEdgeResetGood)
		{
			vector<param_t> params = {
				{ (int8_t)0, true,  DiagnoseResultNone },
				{ (int8_t)0, false, DiagnoseResultNone },
				{ (int8_t)0, false, DiagnoseResultGood },
				{ (int8_t)0, false, DiagnoseResultNone },
				{ (int8_t)0, false, DiagnoseResultNone },
				{ (int8_t)0, true,  DiagnoseResultNone },
				{ (int8_t)0, false, DiagnoseResultNone },
				{ (int8_t)0, false, DiagnoseResultNone },
				{ (int8_t)0, false, DiagnoseResultNone },
				{ (int8_t)0, true,  DiagnoseResultNone },
				{ (int8_t)0, false, DiagnoseResultNone },
				{ (int8_t)0, false, DiagnoseResultGood },
			};

			RunDiagnoseWithParam(3, params);
		}

		TEST_METHOD(TestPitchDiagnosticEdgeResetHigh)
		{
			vector<param_t> params = {
				{ (int8_t)1, true,  DiagnoseResultNone },
				{ (int8_t)1, false, DiagnoseResultNone },
				{ (int8_t)1, false, DiagnoseResultHigh },
				{ (int8_t)1, false, DiagnoseResultNone },
				{ (int8_t)1, false, DiagnoseResultNone },
				{ (int8_t)0, true,  DiagnoseResultNone },
				{ (int8_t)0, false, DiagnoseResultNone },
				{ (int8_t)0, false, DiagnoseResultNone },
				{ (int8_t)0, false, DiagnoseResultNone },
				{ (int8_t)1, true,  DiagnoseResultNone },
				{ (int8_t)1, false, DiagnoseResultNone },
				{ (int8_t)1, false, DiagnoseResultHigh },
				{ (int8_t)1, false, DiagnoseResultNone },
			};

			RunDiagnoseWithParam(3, params);
		}

		TEST_METHOD(TestPitchDiagnosticEdgeResetLow)
		{
			vector<param_t> params = {
				{ (int8_t)-1, true,  DiagnoseResultNone },
				{ (int8_t)-1, false, DiagnoseResultNone },
				{ (int8_t)-1, false, DiagnoseResultLow },
				{ (int8_t)-1, false, DiagnoseResultNone },
				{ (int8_t)-1, false, DiagnoseResultNone },
				{ (int8_t)0, true,  DiagnoseResultNone },
				{ (int8_t)0, false, DiagnoseResultNone },
				{ (int8_t)0, false, DiagnoseResultNone },
				{ (int8_t)0, false, DiagnoseResultNone },
				{ (int8_t)-1, true,  DiagnoseResultNone },
				{ (int8_t)-1, false, DiagnoseResultNone },
				{ (int8_t)-1, false, DiagnoseResultLow },
			};

			RunDiagnoseWithParam(3, params);
		}

	private:

		void RunDiagnoseWithParam(int interval, vector<param_t>& params)
		{
			PitchDiagnostic pd(interval);
			DiagnoseResult_t result = DiagnoseResultNone;

			for (auto item : params) {
				result = pd.Diagnose(std::get<0>(item), std::get<1>(item));
				Assert::AreEqual((int8_t)result, (int8_t)std::get<2>(item));
			}
		}
	};
}
