// BitReverseTableGenerator.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include <iostream>
#include <string>
#include <iomanip>

using namespace std;

static inline uint32_t bitSwap(uint32_t x, int a, int b)
{
	uint32_t bita = (1 << a);
	uint32_t bitb = (1 << b);
	uint32_t out = x & ~(bita | bitb);// remove bit a and b

									  // swap position
	bita = (bita&x) << (b - a);
	bitb = (bitb&x) >> (b - a);
	// assign bit
	out |= (bita | bitb);
	return out;
}

static inline uint32_t bitReverse(int log2N, uint32_t x)
{
	uint32_t out = x;
	for (int i = 0; i < log2N / 2; i++) {
		out = bitSwap(out, i, log2N - 1 - i);
	}
	return out;
}

void PrintTable1()
{
	int N_MAX = 1024;

	cout << "#ifndef _BITREVERSETABLE_H" << endl;
	cout << "#define _BITREVERSETABLE_H" << endl;
	cout << endl;
	cout << "#include <inttypes.h>" << endl;

	cout << "//#define USE_BIT_REVERSE_N2" << endl;
	cout << "//#define USE_BIT_REVERSE_N4" << endl;
	cout << "//#define USE_BIT_REVERSE_N8" << endl;
	cout << "//#define USE_BIT_REVERSE_N16" << endl;
	cout << "//#define USE_BIT_REVERSE_N32" << endl;
	cout << "//#define USE_BIT_REVERSE_N64" << endl;
	cout << "//#define USE_BIT_REVERSE_N128" << endl;
	cout << "//#define USE_BIT_REVERSE_N256" << endl;
	cout << "//#define USE_BIT_REVERSE_N512" << endl;
	cout << "//#define USE_BIT_REVERSE_N1024" << endl;
	cout << endl;

	int log2N = 1;
	for (int N = 2; N <= N_MAX; N = N << 1) {
		cout << "#if defined(USE_BIT_REVERSE_N" << N << ")"<< endl;

		cout << "static const uint16_t s_bitReverse";
		cout << setw(4) << std::setfill('0') << N;
		cout << "[] = {" << endl;
		for (int i = 0; i < N; i++) {
			uint32_t r_idx = bitReverse(log2N, i);
			cout << "\t" << setw(4) << std::setfill(' ') << r_idx;
			if (i + 1 < N ) {
				cout << ",";
			}
			if (i % 8 == 7) {
				cout << endl;
			}
		}
		cout << "};" << endl;
		cout << "#endif" << endl << endl;
		log2N++;
	}

	cout << "static const uint16_t* s_bitReverseTable[] = {" << endl;
	for (int N = 2; N <= N_MAX; N = N << 1) {
		cout << "#if defined(USE_BIT_REVERSE_N" << N << ")" << endl;
		cout << "\ts_bitReverse";
		cout << setw(4) << std::setfill('0') << N;
		if ((N << 1) <= N_MAX) {
			cout << ",";
		}
		cout << endl;
		cout << "#else" << endl;
		cout << "\tNULL";
		if ((N << 1) <= N_MAX) {
			cout << ",";
		}
		cout << endl;
		cout << "#endif" << endl;

		cout << endl;
	}
	cout << "};" << endl << endl;

	cout << "#endif" << endl;
}

int main()
{
	PrintTable1();
    return 0;
}

