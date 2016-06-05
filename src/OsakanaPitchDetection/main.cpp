// OsakanaPitchDetection.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include "OsakanaFpFft.h"
#include "PeakDetectMachine.h"

using namespace std;

int readData(const string& filename, uint16_t* data, const int dataNum)
{
	ifstream file(filename);
	if (!file.is_open()) {
		cout << "can't open " << filename << endl;
		return 1;
	}

	string line;
	int index = 0;
	while (getline(file, line) && index < dataNum) {
		istringstream iss(line);
		float x;
		if (!(iss >> x)) {
			cout << "can't convert " << line << " to float" << endl;
			return 1;
		}
		data[index++] = static_cast<int16_t>(x);
	}

	file.close();
	return 0;
}

int saveData(const string& filename, Fp_t* data, const int dataNum)
{
	ofstream file(filename);
	if (!file.is_open()) {
		cout << "can't open " << filename << endl;
		return 1;
	}

	for (int i = 0; i < dataNum; i++) {
		//stringstream ss;
		char buf[128] = { 0 };
		Fp2CStr(data[i], buf, sizeof(buf));
		file << buf << endl;
		//file << ss.str();
	}

	file.close();
	return 0;
}

int main(int argc, char* argv[])
{
	if (argc < 1) {
		wcout << "need to give file in command-line paremeter" << endl;
		return 1;
	}

	const int N = 1024;
	uint16_t adc_in[N] = { 0 };
	// supporse get 10bit analog input [0,1023]
	readData(argv[1], &adc_in[0], 512);

	// convert to Q1.14 fixedpoint
	osk_fp_complex_t x[N] = { { 0, 0} };
	Fp_t x2[N] = { 0 };
	char buf[128] = { 0 };// for output
	for (int i = 0; i < 512; i++) {

		//adc_in[i] = 512;// debug
		FpBigFp_t w = adc_in[i] - 512; // center to 0 and make it signed
		//x[i].re = (Fp_t)w << 5; // to Q1.14
		x[i].re = (Fp_t)w << 7; // to Q15.16

		// cheating
		//float wf = (adc_in[i] - 512.0) / 512.0;
		//x[i].re = Float2Fp(wf);

		//Fp2CStr(x.re[i], buf, sizeof(buf));
		//printf("%s\n", buf);

		x2[i] = FpMul(x[i].re, x[i].re);
		x2[i] = x2[i] >> 10;
	}

	cout << "-- normalized input signal" << endl;
	for (int i = 500; i < 520; i++) {
		fp_complex_str(&x[i], buf, sizeof(buf));
		printf("%s\n", buf);
	}

	OsakanaFpFftContext_t* ctx;
	if (InitOsakanaFpFft(&ctx, N, 10) != 0) {
		cout << "error" << endl;
		return 1;
	}

	cout << "-- fft/N" << endl;
	OsakanaFpFft(ctx, x, 1);
	for (int i = 0; i < 10; i++) {
		fp_complex_str(&x[i], buf, sizeof(buf));
		printf("%s\n", buf);
	}

	cout << "-- power spectrum" << endl;
	for (int i = 0; i < N; i++) {
		//x[i] = fp_complex_mult(&x[i], &x[i]);

		FpBigFp_t re = FpMul(x[i].re, x[i].re)
						+ FpMul(x[i].im, x[i].im);
		re = re >> 1;
		x[i].re = (Fp_t)re;
		x[i].im = 0;
		//x[N - i - 1] = x[i];
	}

	for (int i = 30; i < 40; i++) {
		fp_complex_str(&x[i], buf, sizeof(buf));
		printf("%s\n", buf);
	}

	cout << "-- IFFT" << endl;
	OsakanaFpIfft(ctx, x, 1);
	for (int i = 0; i < 10; i++) {
		fp_complex_str(&x[i], buf, sizeof(buf));
		printf("%s\n", buf);
	}

#if STRAIGHT_FORARD_M
	Fp_t _m[N] = { 0 };
	for (int t = 0; t < N; t++) {
		for (int j = 0; j < N - t; j++) {
			_m[t] = _m[t] + x2[j] + x2[j + t];
		}
	}
	cout << "-- ms" << endl;
	for (int i = 0; i < 10; i++) {
		Fp2CStr(_m[i], buf, sizeof(buf));
		printf("%s\n", buf);
	}
#else
	Fp_t _m[N] = { 0 }; // smarter way
	_m[0] = x[0].re << 2;
	for (int t = 1; t < N; t++) {
		_m[t] = _m[t - 1] - x2[t - 1] + x2[t];
}
	cout << "-- ms smart" << endl;
	for (int i = 0; i < 10; i++) {
		Fp2CStr(_m[i], buf, sizeof(buf));
		printf("%s\n", buf);
	}
#endif

	// nsdf
	Fp_t _nsdf[N/2] = { 0 };
	for (int t = 0; t < N/2; t++) {
		//_nsdf[t] = (x[t].re << (1 + 1 + 4));
		_nsdf[t] = FpDiv(x[t].re, _m[t]);// possible zero div!
		_nsdf[t] = _nsdf[t] * 2 * 2;
	}
	cout << "-- _nsdf" << endl;
	for (int i = 0; i < 10; i++) {
		Fp2CStr(_nsdf[i], buf, sizeof(buf));
		printf("%s\n", buf);
	}

	saveData("nsdf.dat", _nsdf, 512);

	MachineContext_t* mctx = NULL;
	mctx = CreatePeakDetectMachineContext();
	for (int i = 0; i < N/2; i++) {
		Fp2CStr(_nsdf[i], buf, sizeof(buf));
		printf("Input %s\n", buf);
		Input(mctx, _nsdf[i]);
	}
	DestroyPeakDetectMachineContext(mctx);

	CleanOsakanaFpFft(ctx);
	ctx = NULL;

    return 0;
}

