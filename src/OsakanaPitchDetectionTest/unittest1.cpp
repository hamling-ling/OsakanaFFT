#include "stdafx.h"
#include "CppUnitTest.h"
#include "PeakDetectMachineFp.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace OsakanaPitchDetectionTest
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		
		TEST_METHOD(TestMethod1)
		{
			MachineContextFp_t* mctx = NULL;
			mctx = CreatePeakDetectMachineContextFp();
			Assert::IsNotNull(mctx);
			DestroyPeakDetectMachineContextFp(mctx);
		}

	};
}
