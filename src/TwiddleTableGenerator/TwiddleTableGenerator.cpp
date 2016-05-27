#include "stdafx.h"

#include <string>
#include <iostream>
#include <cmath>
#include <iomanip>

using namespace std;

typedef struct {
	double re;
	double im;
} complex_t;

static inline complex_t MakeComplex(double re, double im)
{
	complex_t complex;
	complex.re = re;
	complex.im = im;
	return complex;
}

static inline complex_t twiddle(int n, int Nin)
{
	double theta = 2.0f*M_PI*n / Nin;
	return MakeComplex(cos(theta), -sin(theta));
}

int main()
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
		cout << "static fp_complex_t W";
		cout << setw(4) << std::setfill('0') << N;
		cout << "[] = {" << endl;
		for (int i = 0; i < N/2; i++) {
			
			complex_t tf = twiddle(i, N);
			cout << "\t{ ";
			cout << "FLOAT2FP( " << tf.re << "f),\tFLOAT2FP( " << tf.im << "f) }";
			if (i + 1 < N / 2) {
				cout << ",";
			}
			cout << endl;
		}
		cout << "};" << endl << endl;
	}

	cout << "static const fp_complex_t* s_twiddlesFp[] = {" << endl;
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

	return 0;
}

