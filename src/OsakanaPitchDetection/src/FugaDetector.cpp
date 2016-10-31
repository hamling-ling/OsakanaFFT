#include "FugaDetector.h"
#include "MelodyDetector.h"

#if defined(ARDUINO_PLATFORM) || defined(RLDUINO78_VERSION) || defined(ARDUINO)      // arduino
#include <Arduino.h>
#else
#include <cstdlib>
#include <algorithm>
#endif

using namespace std;

FugaDetector::FugaDetector()
	:
	_pos(0)
{
	// fuga G D Bb A G Bb A G F# A D
	uint16_t mel0[] = { 67, 74, 70, 69 };// G D Bb A 
	uint16_t mel1[] = { 69, 62 };// A D
	_mds[0] = new MelodyDetector(mel0, sizeof(mel0) / sizeof(mel0[0]));
	_mds[1] = new MelodyDetector(mel1, sizeof(mel1) / sizeof(mel1[0]));
}

FugaDetector::~FugaDetector()
{
	delete _mds[0];
	_mds[0] = NULL;
	delete _mds[1];
	_mds[1] = NULL;
}

bool FugaDetector::Input(uint16_t value)
{
	if (value == 0) {
		return false;
	}

	MelodyDetector* md = _mds[_pos];

	if (md->Input(value)) {
		_pos++;
		if (_pos == MAX_MELODY_DETECTORS) {
			_pos = 0;
			return true;
		}
		return false;
	}

	return false;
}
