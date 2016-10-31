#ifndef _FUGADETECTOR_H_
#define _FUGADETECTOR_H_

#include <stdint.h>

#define MAX_MELODY_DETECTORS 2

class MelodyDetector;

class FugaDetector
{
public:
	FugaDetector();
	~FugaDetector();

	uint8_t Input(uint16_t value);

private:
	uint8_t _pos;
	MelodyDetector* _mds[MAX_MELODY_DETECTORS];
};

#endif
