#ifndef _RESPONSIVEMELODYDETECTOR_H_
#define _RESPONSIVEMELODYDETECTOR_H_

#include <stdint.h>

#define MAX_MELODY_DETECTORS 2

class MelodyDetector;

class ResponsiveMelodyDetector
{
public:
	ResponsiveMelodyDetector(uint16_t* mel0, int mel0_len, uint16_t* mel1, int mel1_len);
	~ResponsiveMelodyDetector();

	uint8_t Input(uint16_t value);

private:
	uint8_t _pos;
	MelodyDetector* _mds[MAX_MELODY_DETECTORS];
};

#endif
