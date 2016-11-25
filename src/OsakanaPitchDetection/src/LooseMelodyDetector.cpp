
#include "LooseMelodyDetector.h"

#if defined(ARDUINO_PLATFORM) || defined(RLDUINO78_VERSION) || defined(ARDUINO)      // arduino
#include <Arduino.h>
#else
#include <cstdlib>
#include <algorithm>
#endif


using namespace std;

LooseMelodyDetector::LooseMelodyDetector(const uint16_t* melody, uint8_t melodyLen)
	:
	MelodyDetector(melody, melodyLen)
{
	for (int i = 0; i < _melodyLength; i++) {
		_melody[i] %= 12;
	}
}

LooseMelodyDetector::~LooseMelodyDetector()
{
}

int LooseMelodyDetector::Input(uint16_t value)
{
	if (value == 0) {
		return 0;
	}

	if (_melody[_pos] != (value%12)) {
		_pos = 0;
		return -1;
	}

	_pos++;
	if (_pos == _melodyLength) {
		_pos = 0;
		return 1;
	}

	return 0;
}
