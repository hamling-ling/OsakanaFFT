#ifndef _LOOSENOTEMELODYDETECTOR_H_
#define _LOOSENOTEMELODYDETECTOR_H_

#include <stdint.h>
#include "MelodyDetector.h"
#include "LooseNoteDetector.h"

typedef unsigned long(*LooseNoteMelodyDetectorMillsFunc_t)(void);

class LooseNoteMelodyDetector : public MelodyDetector
{
public:
	LooseNoteMelodyDetector(const uint16_t* melody, uint8_t melodyLen);
	~LooseNoteMelodyDetector();

	/**
	 *	@return -1:state backed to initial
	 *	@return 0:state not changed
	 *	@return 1:detected and state backed to initial
	 */
	virtual int Input(uint16_t value);
	virtual void Reset();
	void SetDebugMillsFunc(LooseNoteMelodyDetectorMillsFunc_t func);

private:

	LooseNoteDetector* _nd;
};

#endif
