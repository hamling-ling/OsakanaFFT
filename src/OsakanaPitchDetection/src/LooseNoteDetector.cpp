
#include "LooseNoteDetector.h"
#include <cassert>

#if defined(ARDUINO_PLATFORM) || defined(RLDUINO78_VERSION) || defined(ARDUINO)      // arduino
#include <Arduino.h>
//#define DEBUG_ARDUINO
#else
#include <cstdlib>
#include <algorithm>
#endif

#if defined(DEBUG_ARDUINO)
#define LOG_STATE(x,y) {Serial.print(x);Serial.println(y, DEC);}
#define LOG_INPUT(x) {Serial.println(x, DEC);}
#define LOG_STATECHANGE(x,y) {Serial.print(x,DEC);Serial.print(" -> ");Serial.println(y,DEC);}
#else
#define LOG_STATE(x,y)
#define LOG_INPUT(x)
#define LOG_STATECHANGE(x,y)
#endif

using namespace std;

const LooseNoteDetector::LooseDetectorInputFunc_t LooseNoteDetector::_stateFuncs[] = {
	&LooseNoteDetector::inputForInitial,
	&LooseNoteDetector::inputForInitialSilent,
	&LooseNoteDetector::inputForInitialIntermediate,
	&LooseNoteDetector::inputForDetecting,
	&LooseNoteDetector::inputForIntermediate,
	&LooseNoteDetector::inputForComplete
};

LooseNoteDetector::LooseNoteDetector(uint16_t note, uint16_t minInterval, uint16_t minNoiseInterval, MillsFunc_t millsFunc)
	:
	_note(note % 12),
	_minInterval(minInterval),
	_minInitialSilentInterval(minInterval << 1),
	_minNoiseInterval(minNoiseInterval),
	_millsFunc(millsFunc),
	_state(kLooseNoteDetectorStateInitial),
	_timeMigrated(0),
	_timeDetectingStart(0)
{
}

LooseNoteDetector::~LooseNoteDetector()
{
}

LooseNoteDetectionResult_t LooseNoteDetector::Input(uint16_t value)
{
	LooseDetectorInputFunc_t stateFunc = _stateFuncs[_state];
	return (this->*stateFunc)(value);
}

void LooseNoteDetector::Reset()
{
	_state = kLooseNoteDetectorStateInitial;
	_timeMigrated = 0;
	_timeDetectingStart = 0;
}

void LooseNoteDetector::Reset(uint16_t note)
{
	Reset();
	_note = note % 12;

	LOG_STATE("wait for ", note);
}

void LooseNoteDetector::SetDebugMillsFunc(MillsFunc_t func)
{
	_millsFunc = func;
}

LooseNoteDetectionResult_t LooseNoteDetector::inputForInitial(uint16_t value)
{
	LOG_INPUT(value);

	if (value != 0) {
		if (value % 12 == _note) {
			gotoState(kLooseNoteDetectorStateDetecting);
			return kLooseNoteDetectionResultNone;
		}
		gotoState(kLooseNoteDetectorStateInitialIntermediate);
	}
	else {
		gotoState(kLooseNoteDetectorStateInitialSilent);
	}
	return kLooseNoteDetectionResultNone;
}

LooseNoteDetectionResult_t LooseNoteDetector::inputForInitialSilent(uint16_t value)
{
	LOG_INPUT(value);

	unsigned long now = (*_millsFunc)();
	unsigned long retentionTime = getRetentionTime(now);
	if (_minInitialSilentInterval < retentionTime) {

		gotoState(kLooseNoteDetectorStateComplete);
		unsigned long detectingTime = getDetectingTime(now);

		if (value == 0) {
			return kLooseNoteDetectionResultInvalid;
		}
		else {
			return kLooseNoteDetectionResultInvalidWithNextNote;
		}
	}

	if (value != 0 && value % 12 == _note) {
		gotoState(kLooseNoteDetectorStateDetecting);
	}

	return kLooseNoteDetectionResultNone;
}

LooseNoteDetectionResult_t LooseNoteDetector::inputForInitialIntermediate(uint16_t value)
{
	LOG_INPUT(value);

	unsigned long now = (*_millsFunc)();
	unsigned long retentionTime = getRetentionTime(now);
	if (_minNoiseInterval < retentionTime) {

		gotoState(kLooseNoteDetectorStateComplete);
		unsigned long detectingTime = getDetectingTime(now);

		if (value == 0) {
			return kLooseNoteDetectionResultInvalid;
		}
		else {
			return kLooseNoteDetectionResultInvalidWithNextNote;
		}
	}

	if (value != 0 && value % 12 == _note) {
		gotoState(kLooseNoteDetectorStateDetecting);
	}

	return kLooseNoteDetectionResultNone;
}

LooseNoteDetectionResult_t LooseNoteDetector::inputForDetecting(uint16_t value)
{
	LOG_INPUT(value);

	if (value == 0) {
		gotoState(kLooseNoteDetectorStateIntermediate);
		return kLooseNoteDetectionResultNone;
	}

	if (value % 12 == _note) {
		return kLooseNoteDetectionResultNone;
	}

	gotoState(kLooseNoteDetectorStateIntermediate);

	return kLooseNoteDetectionResultNone;
}

LooseNoteDetectionResult_t LooseNoteDetector::inputForIntermediate(uint16_t value)
{
	LOG_INPUT(value);

	unsigned long now = (*_millsFunc)();
	unsigned long retentionTime = getRetentionTime(now);
	if (_minNoiseInterval < retentionTime) {

		gotoState(kLooseNoteDetectorStateComplete);
		unsigned long detectingTime = getDetectingTime(now);

		if (_minInterval < detectingTime) {
			if (value == 0) {
				return kLooseNoteDetectionResultDetected;
			}
			else {
				return kLooseNoteDetectionResultDetectedWithNextNote;
			}
		}
		else {
			if (value == 0) {
				return kLooseNoteDetectionResultInvalid;
			}
			else {
				return kLooseNoteDetectionResultInvalidWithNextNote;
			}
		}
	}

	if (value != 0 && value % 12 == _note) {
		gotoState(kLooseNoteDetectorStateDetecting);
	}

	return kLooseNoteDetectionResultNone;
}

LooseNoteDetectionResult_t LooseNoteDetector::inputForComplete(uint16_t value)
{
	LOG_INPUT(value);

	// should not be called
	return kLooseNoteDetectionResultDetected;
}

unsigned long LooseNoteDetector::getRetentionTime(unsigned long now)
{
	return (now - _timeMigrated);
}

unsigned long LooseNoteDetector::getDetectingTime(unsigned long now)
{
	return (now - _timeDetectingStart);
}

void LooseNoteDetector::gotoState(LooseNoteDetectorState_t state)
{
	assert(_state != state);
	LOG_STATECHANGE(_state, state)

		_state = state;
	_timeMigrated = (*_millsFunc)();
}