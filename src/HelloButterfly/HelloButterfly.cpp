#include "stdafx.h"
#include <iostream>
#include <string>

using namespace std;

void butterfly(int idx_a, int idx_b) {
	cout << "butterfly(" << idx_a << ", " << idx_b << ")" << endl;
}

void twiddle(int n, int N)
{
	//cout << "twiddle(" << n << ", " << N << ")" << endl;
}

int main()
{
	/*int N = 32;
	int log2N = 3;
	int N2 = N >> 1; // N2=N/2
	int di = 2;
	int dj = 2;*/

	// i = 0,2,4 for N=8
	// i = 0,2,4,6 for N=16
/*	for (int i = 0; i < log2N; i++) {
		int bnum = dj >> 1; // number of butterfly in 2nd loop
		// j = 0,2,4,6
		// j = 0,4
		// j = 0
		for (int j = 0; j < N; j += dj) {
			cout << "i=" << i << ", j=" << j << endl;
			int idx_a = j;
			int idx_b = j + bnum;
			for (int k = 0; k < bnum; k++) {
				twiddle(k, dj);
				butterfly(idx_a++, idx_b++);
			}
		}
		dj = dj << 1;
	}*/
	
	int N = 32;
	int log2N = 5;

	int di = 2;
	int dj = 2;
	int log2djm1 = 0;	// index of N in twiddle table. log2(dj)-1

	// i = 0,2,4 for N=8
	// i = 0,2,4,6 for N=16
	for (int i = 0; i < log2N; i++) {
		int bnum = dj >> 1;
		// j = 0,2,4,6
		// j = 0,4
		// j = 0
		for (int j = 0; j < N; j += dj) {
			cout << "i=" << i << ", j=" << j << endl;
			int idx_a = j;
			int idx_b = j + bnum;
			for (int k = 0; k < bnum; k++) {
				twiddle(k, dj);
				butterfly(idx_a++, idx_b++);
			}
		}
		dj = dj << 1;
		log2djm1++;
	}

    return 0;
}

