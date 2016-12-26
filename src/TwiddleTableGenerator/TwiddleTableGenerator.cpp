#include "stdafx.h"

#include <string>
#include <iostream>
#include <cmath>
#include <iomanip>

using namespace std;

typedef struct {
	double re;
	double im;
} osk_complex_t;

static inline osk_complex_t MakeComplex(double re, double im)
{
	osk_complex_t complex;
	complex.re = re;
	complex.im = im;
	return complex;
}

static inline osk_complex_t twiddle(int n, int Nin)
{
	double theta = 2.0f*M_PI*n / Nin;
	return MakeComplex(cos(theta), -sin(theta));
}

void PrintTable1()
{
	int N_MAX = 1024;

	cout << "#ifndef _TWIDLLETABLE_H" << endl;
	cout << "#define _TWIDLLETABLE_H" << endl;
	cout << endl;
	cout << "#include \"OsakanaFp.h\"" << endl;
	cout << "#include \"OsakanaFpComplex.h\"" << endl << endl;

	std::cout.precision(16);
	std::cout.setf(std::ios::fixed, std::ios::floatfield);
	for (int N = 2; N <= N_MAX; N = N << 1) {
		cout << "static const osk_fp_complex_t W";
		cout << setw(4) << std::setfill('0') << N;
		cout << "[] = {" << endl;
		for (int i = 0; i < N / 2; i++) {

			osk_complex_t tf = twiddle(i, N);
			cout << "\t{ ";
			cout << "FLOAT2FP( " << tf.re << "f),\tFLOAT2FP( " << tf.im << "f) }";
			if (i + 1 < N / 2) {
				cout << ",";
			}
			cout << endl;
		}
		cout << "};" << endl << endl;
	}

	cout << "static const osk_fp_complex_t* s_twiddlesFp[] = {" << endl;
	for (int N = 2; N <= N_MAX; N = N << 1) {
		cout << "\tW";
		cout << setw(4) << std::setfill('0') << N;
		if (N << 1 <= N_MAX) {
			cout << ",";
		}
		cout << endl;
	}
	cout << "};" << endl << endl;

	cout << "#endif" << endl;
}

void PrintTable2()
{
	int N = 1024;

	cout << "#ifndef _TWIDLLETABLE_H" << endl;
	cout << "#define _TWIDLLETABLE_H" << endl;
	cout << endl;
	cout << "#include \"OsakanaFp.h\"" << endl;
	cout << "#include \"OsakanaFpComplex.h\"" << endl << endl;

	std::cout.precision(16);
	std::cout.setf(std::ios::fixed, std::ios::floatfield);
	cout << "static const osk_fp_complex_t s_twiddlesFp[] = {" << endl;
	for (int i = 0; i < N / 2; i++) {

		osk_complex_t tf = twiddle(i, N);
		cout << "\t{ FLOAT2FP( " << tf.re << "f),\tFLOAT2FP( " << tf.im << "f) }";
		if (i + 1 < N / 2) {
			cout << ",";
		}
		cout << endl;
	}
	cout << "};" << endl << endl;

	cout << "#endif" << endl;
}

void PrintTable3()
{
	int N_MAX = 1024;

	cout << "#ifndef _TWIDLLETABLE_H" << endl;
	cout << "#define _TWIDLLETABLE_H" << endl;
	cout << endl;
	cout << "#include \"OsakanaFp.h\"" << endl;
	cout << "#include \"OsakanaFpComplex.h\"" << endl << endl;

	cout << endl;
	cout << "#if defined(USE_FPW_TWIDDLE_TABLE)" << endl;
	cout << "#define CVT_TO_FP(x)\tFLOAT2FPW(x)" << endl;
	cout << "#define COMPLEX_TYPE\tosk_fpw_complex_t" << endl;
	cout << "#else" << endl;
	cout << "#define CVT_TO_FP(x)\tFLOAT2FP(x)" << endl;
	cout << "#define COMPLEX_TYPE\tosk_fp_complex_t" << endl;
	cout << "#endif" << endl;
	cout << endl;

	cout << "//#define USE_TWIDDLE_TABLE_N2" << endl;
	cout << "//#define USE_TWIDDLE_TABLE_N4" << endl;
	cout << "//#define USE_TWIDDLE_TABLE_N8" << endl;
	cout << "//#define USE_TWIDDLE_TABLE_N16" << endl;
	cout << "//#define USE_TWIDDLE_TABLE_N32" << endl;
	cout << "//#define USE_TWIDDLE_TABLE_N64" << endl;
	cout << "//#define USE_TWIDDLE_TABLE_N128" << endl;
	cout << "//#define USE_TWIDDLE_TABLE_N256" << endl;
	cout << "//#define USE_TWIDDLE_TABLE_N512" << endl;
	cout << "//#define USE_TWIDDLE_TABLE_N1024" << endl;
	cout << endl;

	std::cout.precision(16);
	std::cout.setf(std::ios::fixed, std::ios::floatfield);
	for (int N = 2; N <= N_MAX; N = N << 1) {
		cout << "#if defined(USE_TWIDDLE_TABLE_N" << N << ")" << endl;
		
		cout << "static const COMPLEX_TYPE W";
		cout << setw(4) << std::setfill('0') << N;
		cout << "[] = {" << endl;
		for (int i = 0; i < N / 2; i++) {

			osk_complex_t tf = twiddle(i, N);
			cout << "\t{ ";
			cout << "CVT_TO_FP( " << tf.re << "f),\tCVT_TO_FP( " << tf.im << "f) }";
			if (i + 1 < N / 2) {
				cout << ",";
			}
			cout << endl;
		}
		cout << "};" << endl;
		cout << "#endif" << endl << endl;
	}

	cout << "static const COMPLEX_TYPE* s_twiddlesFp[] = {" << endl;
	for (int N = 2; N <= N_MAX; N = N << 1) {
		cout << "#if defined(USE_TWIDDLE_TABLE_N" << N << ")" << endl;
		cout << "\tW";
		cout << setw(4) << std::setfill('0') << N;
		if (N << 1 <= N_MAX) {
			cout << ",";
		}
		cout << endl;
		cout << "#else" << endl;
		cout << "\tNULL";
		if (N << 1 <= N_MAX) {
			cout << ",";
		}
		cout << endl;
		cout << "#endif" << endl;
	}
	cout << "};" << endl << endl;

	cout << "#endif" << endl;
}

int main()
{
	PrintTable3();

	return 0;
}

