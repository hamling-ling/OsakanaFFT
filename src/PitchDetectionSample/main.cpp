#define _USE_MATH_DEFINES
#include <cmath>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "OsakanaPitchDetection.h"
#include "OsakanaPitchDetectionFp.h"

//#define USE_DATFILE

using namespace std;

string g_filename;

float g_data[N] = { 0.0f };

static int readGeneratedFpData(Fp_t* data, uint8_t stride, const int dataNum, Fp_t* rawdata_min, Fp_t* rawdata_max)
{
	int counter = 0;
	while (counter < dataNum) {
		float x = g_data[counter];

		*data = static_cast<int16_t>(x);
		*rawdata_min = std::min(*data, *rawdata_min);
		*rawdata_max = std::max(*data, *rawdata_max);
		data += stride;
		counter++;
	}
	return 0;
}

static int readGeneratedData(float* data, uint8_t stride, const int dataNum)
{
	int counter = 0;
	while (counter < dataNum) {
		float x = g_data[counter];

		*data = x;
		data += stride;
		counter++;
	}
	return 0;
}

static int readFpData(Fp_t* data, uint8_t stride, const int dataNum, Fp_t* rawdata_min, Fp_t* rawdata_max)
{
	ifstream file(g_filename);
	if (!file.is_open()) {
		cout << "can't open " << g_filename << endl;
		return 1;
	}

	string line;
	int counter = 0;
	while (getline(file, line) && counter < dataNum) {
		istringstream iss(line);
		float x;
		if (!(iss >> x)) {
			cout << "can't convert " << line << " to float" << endl;
			return 1;
		}
		*data = static_cast<int16_t>(x);
		*rawdata_min = std::min(*data, *rawdata_min);
		*rawdata_max = std::max(*data, *rawdata_max);
		data += stride;
		counter++;
	}

	file.close();
	return 0;
}

static int readData(float* data, uint8_t stride, const int dataNum)
{
	ifstream file(g_filename);
	if (!file.is_open()) {
		cout << "can't open " << g_filename << endl;
		return 1;
	}

	string line;
	int counter = 0;
	while (getline(file, line) && counter < dataNum) {
		istringstream iss(line);
		float x;
		if (!(iss >> x)) {
			cout << "can't convert " << line << " to float" << endl;
			return 1;
		}
		*data = x;
		data += stride;
		counter++;
	}

	file.close();
	return 0;
}

static void CreateSineData(float* x, float freq)
{
	// time of N samples
	float tn = (T1024_1024 / 1024.0f) * (N / 1024.0);
	// wave number in N
	float wn = freq * tn;
	for (int i = 0; i < N; i++) {
		x[i] = (float)sin(wn * i / N * 2.0 * M_PI);
		x[i] += 1.0f;
		x[i] *= 512.0f;
	}
}

int main(int argc, char* argv[])
{
	if (argc < 1) {
		DLOG("need to give file in command-line paremeter");
		return 1;
	}
#if defined(USE_DATFILE)
#if 1
	PitchDetectorFp detector;
	detector.Initialize(readFpData);
#else
	PitchDetector detector;
	detector.Initialize(readData);
#endif
	g_filename = argv[1];

	PitchInfo_t pitchInfo;
	while (1) {
		detector.DetectPitch(&pitchInfo);
		break;// for debug
	}
#else

	//CreateSineData(g_data, 65.4);// note = 36 for 65.4Hz
	CreateSineData(g_data, 1568.0f);// note = 91 for 1568.0Hz
	PitchDetectorFp detector;
	detector.Initialize(readGeneratedFpData);

	PitchInfo_t pitchInfo;
	pitchInfo = MakePitchInfo();
	int result = detector.DetectPitch(&pitchInfo);
#endif

	detector.Cleanup();
	return 0;
}

