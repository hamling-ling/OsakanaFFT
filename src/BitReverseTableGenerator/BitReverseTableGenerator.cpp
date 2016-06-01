// BitReverseTableGenerator.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include <iostream>
#include <string>
#include <iomanip>
#include <map>
#include <algorithm>
#include <vector>

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

void BitReverseTableForN(int N, int log2N, map<uint16_t, uint16_t>& table)
{
	table.clear();
	for (int i = 0; i < N; i++) {
		uint32_t r_idx = bitReverse(log2N, i);
		if (r_idx == i) {
			continue;
		}
		uint16_t idx_min = std::min(
			static_cast<uint16_t>(i),
			static_cast<uint16_t>(r_idx));
		uint16_t idx_max = std::max(
			static_cast<uint16_t>(i),
			static_cast<uint16_t>(r_idx));
		table[idx_min] = idx_max;
	}
}

void PrintTable2()
{
	int N_MAX = 1024;

	cout << "#ifndef _BITREVERSETABLE_H" << endl;
	cout << "#define _BITREVERSETABLE_H" << endl;
	cout << endl;
	cout << "#include <inttypes.h>" << endl;

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

	cout << "typedef struct {" << endl;
	cout << "\tuint16_t first;" << endl;
	cout << "\tuint16_t second;" << endl;
	cout << "} osk_bitreverse_idx_pair_t;" << endl;
	cout << endl;

	int log2N = 2;
	vector<uint16_t> lens;
	lens.push_back(0);
	for (int N =4; N <= N_MAX; N = N << 1) {
		map<uint16_t, uint16_t> table;
		BitReverseTableForN(N, log2N, table);

		cout << "#if defined(USE_BIT_REVERSE_N" << N << ")" << endl;

		cout << "static const osk_bitreverse_idx_pair_t s_bitReverse";
		cout << setw(4) << std::setfill('0') << N;
		cout << "[] = {" << endl;
		int i = 0;
		for (auto item : table) {
			cout << "\t{ ";
			cout << setw(4) << std::setfill(' ') << item.first << ", ";
			cout << setw(4) << std::setfill(' ') << item.second << "}";
			if (i + 1 < table.size()) {
				cout << ",";
			}
			if (i % 4 == 3) {
				cout << endl;
			}
			i++;
		}
		if (table.size() < 8) {
			cout << endl;
		}
		cout << "};" << endl;
		cout << "#endif" << endl << endl << endl;
		log2N++;

		lens.push_back(table.size());
	}

	cout << "static const osk_bitreverse_idx_pair_t* s_bitReverseTable[] = {" << endl;
	for (int N = 2; N <= N_MAX; N = N << 1) {
		//if (N == 2) {
		//	cout << "\tNULL," << endl;
		//	continue;
		//}
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

	cout << "static const uint16_t s_bitReversePairNums[] = {" << endl;
	cout << "\t";
	int i = 0;
	for (int N = 2; N <= N_MAX; N = N << 1) {
		cout << setw(4) << std::setfill(' ') << lens[i];
		bool hasNext = ((N << 1) <= N_MAX);
		if (hasNext) {
			cout << ", ";
		}
		if (i % 4 == 3) {
			if (hasNext)
				cout << endl << "\t";
			else
				cout << endl;
		}
		i++;
	}
	if (lens.size() % 4 != 0) {
		cout << endl;
	}
	cout << "};" << endl << endl;

	cout << "#endif" << endl;

}
int main()
{
	PrintTable2();
    return 0;
}

