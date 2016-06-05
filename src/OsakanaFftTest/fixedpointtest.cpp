#include "stdafx.h"
#include "CppUnitTest.h"
#include "OsakanaFp.h"
#include <iostream>
#include <string>
#include <sstream>
#include <CppUnitTestLogger.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

string Fp2Str(Fp_t a)
{
	char buf[128] = { 0 };
	string out = Fp2CStr(a, buf, sizeof(buf));
	return out;
}

string Fp2HexStr(Fp_t a)
{
	char buf[128] = { 0 };
	string out = Fp2HexCStr(a, buf, sizeof(buf));
	return out;
}

namespace OsakanaFftTest
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		
		TEST_METHOD(TestMethod1)
		{
			testFpArithmetic(0.5, 0.25);
			testFpArithmetic(2.5, 1.25);
		}

	private:
		void outputLog(stringstream& ss)
		{
			Logger::WriteMessage(ss.str().c_str());
			ss.str("");
			ss.clear(stringstream::goodbit);
		}

		bool testFpArithmetic(float x, float y)
		{
			stringstream ss;
			Fp_t a = 0;
			Fp_t b = 0;
			Fp_t c = 0;
			float f = 0;

			// +float to fp
			f = x;
			c = Float2Fp(f);
			ss << "float " << f << " = Fp " << Fp2Str(c) << "(" << Fp2HexStr(c) << ")" << endl;
			outputLog(ss);

			// +float to fp
			f = y;
			c = Float2Fp(f);
			ss << "float " << f << " = Fp " << Fp2Str(c) << "(" << Fp2HexStr(c) << ")" << endl;
			outputLog(ss);
			
			// -float to fp
			f = -x;
			c = Float2Fp(f);
			ss << "float " << f << " = Fp " << Fp2Str(c) << "(" << Fp2HexStr(c) << ")" << endl;
			outputLog(ss);

			// -float to fp
			f = -y;
			c = Float2Fp(f);
			ss << "float " << f << " = Fp " << Fp2Str(c) << "(" << Fp2HexStr(c) << ")" << endl;
			outputLog(ss);

			// fp to float
			c = Float2Fp(x);
			ss << "Fp " << Fp2Str(c) << " = float " << Fp2Float(c) << endl;
			outputLog(ss);

			// fp to float
			c = Float2Fp(-x);
			ss << "Fp " << Fp2Str(c) << " = float " << Fp2Float(c) << endl;
			outputLog(ss);

			// fp to float
			c = Float2Fp(y);
			ss << "Fp " << Fp2Str(c) << " = float " << Fp2Float(c) << endl;
			outputLog(ss);

			// fp to float
			c = Float2Fp(-y);
			ss << "Fp " << Fp2Str(c) << " = float " << Fp2Float(c) << endl;
			outputLog(ss);

			// +/+ multiplication
			a = Float2Fp(x);
			b = Float2Fp(y);
			c = FpMul(a, b);
			ss << Fp2Str(a) << " * " << Fp2Str(b) << " = " << Fp2Str(c) << "(" << Fp2HexStr(c) << ")" << endl;
			outputLog(ss);

			// -/+ multiplication
			a = Float2Fp(-x);
			b = Float2Fp(y);
			c = FpMul(a, b);
			ss << Fp2Str(a) << " * " << Fp2Str(b) << " = " << Fp2Str(c) << "(" << Fp2HexStr(c) << ")" << endl;
			outputLog(ss);

			// +/- multiplication
			a = Float2Fp(x);
			b = Float2Fp(-y);
			c = FpMul(a, b);
			ss << Fp2Str(a) << " * " << Fp2Str(b) << " = " << Fp2Str(c) << "(" << Fp2HexStr(c) << ")" << endl;
			outputLog(ss);

			// -/- multiplication
			a = Float2Fp(-x);
			b = Float2Fp(-y);
			c = FpMul(a, b);
			ss << Fp2Str(a) << " * " << Fp2Str(b) << " = " << Fp2Str(c) << "(" << Fp2HexStr(c) << ")" << endl;
			outputLog(ss);

			// +/+ division
			a = Float2Fp(x);
			b = Float2Fp(y);
			c = FpDiv(a, b);
			ss << Fp2Str(a) << " / " << Fp2Str(b) << " = " << Fp2Str(c) << "(" << Fp2HexStr(c) << ")" << endl;
			outputLog(ss);

			// -/+ division
			a = Float2Fp(-x);
			b = Float2Fp(y);
			c = FpDiv(a, b);
			ss << Fp2Str(a) << " / " << Fp2Str(b) << " = " << Fp2Str(c) << "(" << Fp2HexStr(c) << ")" << endl;
			outputLog(ss);

			// +/- division
			a = Float2Fp(0.5f);
			b = Float2Fp(-0.25f);
			c = FpDiv(a, b);
			ss << Fp2Str(a) << " / " << Fp2Str(b) << " = " << Fp2Str(c) << "(" << Fp2HexStr(c) << ")" << endl;
			outputLog(ss);

			// -/- division
			a = Float2Fp(-x);
			b = Float2Fp(-y);
			c = FpDiv(a, b);
			ss << Fp2Str(a) << " / " << Fp2Str(b) << " = " << Fp2Str(c) << "(" << Fp2HexStr(c) << ")" << endl;
			outputLog(ss);

			// inv sign x
			f = x;
			a = Float2Fp(f);
			c = FpInvSgn(a);
			ss << "inv sign " << Fp2Str(a) << " = " << Fp2Str(c) << "(" << Fp2HexStr(c) << ")" << endl;
			outputLog(ss);

			// inv back sign x
			b = FpInvSgn(c);
			ss << "inv sign " << Fp2Str(c) << " = " << Fp2Str(b) << "(" << Fp2HexStr(b) << ")" << endl;
			outputLog(ss);

			// inv sign y
			f = y;
			a = Float2Fp(f);
			c = FpInvSgn(a);
			ss << "inv sign " << Fp2Str(a) << " = " << Fp2Str(c) << "(" << Fp2HexStr(c) << ")" << endl;
			outputLog(ss);

			// inv back sign y
			b = FpInvSgn(c);
			ss << "inv sign " << Fp2Str(c) << " = " << Fp2Str(b) << "(" << Fp2HexStr(b) << ")" << endl;
			outputLog(ss);

			// right shift x
			f = x;
			a = Float2Fp(f);
			c = FpRShift(a, 1);
			ss << Fp2Str(a) << " >> " << 2 << " = " << Fp2Str(c) << "(" << Fp2HexStr(c) << ")" << endl;
			outputLog(ss);

			// right shift y
			f = y;
			a = Float2Fp(f);
			c = FpRShift(a, 1);
			ss << Fp2Str(a) << " >> " << 2 << " = " << Fp2Str(c) << "(" << Fp2HexStr(c) << ")" << endl;
			outputLog(ss);

			// right shift -x
			f = -x;
			a = Float2Fp(f);
			c = FpRShift(a, 1);
			ss << Fp2Str(a) << " >> " << 2 << " = " << Fp2Str(c) << "(" << Fp2HexStr(c) << ")" << endl;
			outputLog(ss);

			// right shift -y
			f = -y;
			a = Float2Fp(f);
			c = FpRShift(a, 1);
			ss << Fp2Str(a) << " >> " << 2 << " = " << Fp2Str(c) << "(" << Fp2HexStr(c) << ")" << endl;
			outputLog(ss);

			// left shift x
			f = x;
			a = Float2Fp(f);
			c = FpLShift(a, 1);
			ss << Fp2Str(a) << " << " << 2 << " = " << Fp2Str(c) << "(" << Fp2HexStr(c) << ")" << endl;
			outputLog(ss);

			// left shift y
			f = y;
			a = Float2Fp(f);
			c = FpLShift(a, 1);
			cout << Fp2Str(a) << " << " << 2 << " = " << Fp2Str(c) << "(" << Fp2HexStr(c) << ")" << endl;
			outputLog(ss);

			// left shift -x
			f = -x;
			a = Float2Fp(f);
			c = FpLShift(a, 1);
			ss << Fp2Str(a) << " << " << 2 << " = " << Fp2Str(c) << "(" << Fp2HexStr(c) << ")" << endl;
			outputLog(ss);

			// left shift -y
			f = -y;
			a = Float2Fp(f);
			c = FpLShift(a, 1);
			ss << Fp2Str(a) << " << " << 2 << " = " << Fp2Str(c) << "(" << Fp2HexStr(c) << ")" << endl;
			outputLog(ss);
			
			// sqrt x
			f = x;
			a = Float2Fp(f);
			c = FpSqrt(a);
			ss << "sqrt(" << Fp2Str(a) << ") = " << Fp2Str(c) << "(" << Fp2HexStr(c) << ")" << endl;
			outputLog(ss);

			// sqrt y
			f = y;
			a = Float2Fp(f);
			c = FpSqrt(a);
			ss << "sqrt(" << Fp2Str(a) << ") = " << Fp2Str(c) << "(" << Fp2HexStr(c) << ")" << endl;
			outputLog(ss);

			return true;
		}
	};
}