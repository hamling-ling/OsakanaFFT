
#include "LooseNoteMelodyDetector.h"

#if defined(ARDUINO_PLATFORM) || defined(RLDUINO78_VERSION) || defined(ARDUINO)      // arduino
#include <Arduino.h>
static inline unsigned long millisec()
{
	return millis();
}

#else
#include <cstdlib>
#include <algorithm>
#include <chrono>

unsigned long millisec()
{
	using namespace std::chrono;
	milliseconds ms = duration_cast< milliseconds >(
		system_clock::now().time_since_epoch()
	);
	return ms.count();
}

#endif


using namespace std;

LooseNoteMelodyDetector::LooseNoteMelodyDetector(const uint16_t* melody, uint8_t melodyLen)
	:
	MelodyDetector(melody, melodyLen),
	_nd(new LooseNoteDetector(0, 999, 499, millisec))
{
	for (int i = 0; i < _melodyLength; i++) {
		_melody[i] %= 12;
	}
	_nd->Reset(_melody[0]);
}

LooseNoteMelodyDetector::~LooseNoteMelodyDetector()
{
	delete _nd;
}

int LooseNoteMelodyDetector::Input(uint16_t value)
{
	LooseNoteDetectionResult_t result = _nd->Input(value);

	int ret = -1;
	switch (result) {
		case kLooseNoteDetectionResultNone:
			break;
		case kLooseNoteDetectionResultInvalid:
		case kLooseNoteDetectionResultInvalidWithNextNote:
			_pos++;
			if (_pos == _melodyLength) {
				_pos = 0;
				ret = -1;
			}
			break;
		case kLooseNoteDetectionResultDetected:
		case kLooseNoteDetectionResultDetectedWithNextNote:
			_pos++;
			if (_pos == _melodyLength) {
				_pos = 0;
				ret = 1;
			}
			break;
	}

	return ret;
}

void LooseNoteMelodyDetector::Reset()
{
	_pos = 0;
}

void LooseNoteMelodyDetector::SetDebugMillsFunc(LooseNoteMelodyDetectorMillsFunc_t func)
{
	_nd->SetDebugMillsFunc(static_cast<MillsFunc_t>(func));
}
