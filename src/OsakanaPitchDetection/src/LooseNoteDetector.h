#ifndef _LOOSENOTEDETECTOR_H_
#define _LOOSENOTEDETECTOR_H_

#include <stdint.h>

typedef enum LooseNoteDetectionResult_tag {
	kLooseNoteDetectionResultNone,
	kLooseNoteDetectionResultDetected,
	kLooseNoteDetectionResultDetectedWithNextNote,
	kLooseNoteDetectionResultInvalid,
	kLooseNoteDetectionResultInvalidWithNextNote,
} LooseNoteDetectionResult_t;

typedef enum LooseNoteDetectorState_tag {
	// initial state. waiting for note input
	kLooseNoteDetectorStateInitial,
	// takes this state when 1st input is silent, become this state
	kLooseNoteDetectorStateInitialSilent,
	// takes this st1st when 1st input is not silent and not expected note. waiting for note input
	kLooseNoteDetectorStateInitialIntermediate,
	// detecting state. waiting for note end or current note reaches long enough
	kLooseNoteDetectorStateDetecting,
	// silent or note change detected. uncertern if it's a noise
	kLooseNoteDetectorStateIntermediate,
	// detection complete. comes after long enough detecting or intermediate state.
	kLooseNoteDetectorStateComplete,
} LooseNoteDetectorState_t;

typedef unsigned long(*MillsFunc_t)(void);

class LooseNoteDetector
{
public:
	LooseNoteDetector(uint16_t note, uint16_t minInterval, uint16_t minNoiseInterval, MillsFunc_t millsFunc);
	~LooseNoteDetector();

	virtual LooseNoteDetectionResult_t Input(uint16_t value);
	void Reset();
	void Reset(uint16_t note);

protected:
	uint16_t _note;// [0,12]
	const uint16_t _minInterval;
	const uint16_t _minInitialSilentInterval;
	const uint16_t _minNoiseInterval;
	MillsFunc_t _millsFunc;
	LooseNoteDetectorState_t _state;
	// when migrated to current state
	unsigned long _timeMigrated;
	// when migrated to detecting state
	unsigned long _timeDetectingStart;

	typedef LooseNoteDetectionResult_t (LooseNoteDetector::*LooseDetectorInputFunc_t)(uint16_t);
	static const LooseDetectorInputFunc_t _stateFuncs[];

	LooseNoteDetectionResult_t inputForInitial(uint16_t value);
	LooseNoteDetectionResult_t inputForInitialSilent(uint16_t value);
	LooseNoteDetectionResult_t inputForInitialIntermediate(uint16_t value);
	LooseNoteDetectionResult_t inputForDetecting(uint16_t value);
	LooseNoteDetectionResult_t inputForIntermediate(uint16_t value);
	LooseNoteDetectionResult_t inputForComplete(uint16_t value);

	// get how long detector has been staying current state
	unsigned long getRetentionTime(unsigned long now);
	// get how long passed after migrated to detecting state
	unsigned long getDetectingTime(unsigned long now);
	void gotoState(LooseNoteDetectorState_t state);

public:
	void SetDebugMillsFunc(MillsFunc_t func);
};

#endif
