#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "OsakanaPitchDetection.h"
#include "OsakanaPitchDetectionFp.h"

using namespace std;

string g_filename;

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

int main(int argc, char* argv[])
{
	if (argc < 1) {
		DLOG("need to give file in command-line paremeter");
		return 1;
	}

#if 1
	PitchDetectorFp detector;
	detector.Initialize(readFpData);
#else
	PitchDetector detector;
	detector.Initialize(readData);
#endif
	g_filename = argv[1];

	while (1) {
		detector.DetectPitch();
		break;// for debug
	}
	detector.Cleanup();

	return 0;
}

