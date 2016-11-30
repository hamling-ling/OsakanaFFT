#ifndef _LOOSEMELODYDETECTOR_H_
#define _LOOSEMELODYDETECTOR_H_

#include <stdint.h>
#include "MelodyDetector.h"

class LooseMelodyDetector : public MelodyDetector
{
public:
	LooseMelodyDetector(const uint16_t* melody, uint8_t melodyLen);
	~LooseMelodyDetector();

	/**
	 *	@return -1:state backed to initial
	 *	@return 0:state not changed
	 *	@return 1:detected and state backed to initial
	 */
	virtual int Input(uint16_t value);
};

#endif
