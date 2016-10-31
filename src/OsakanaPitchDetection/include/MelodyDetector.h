#ifndef _MELODYDETECTOR_H_
#define _MELODYDETECTOR_H_

#include <stdint.h>

#define MAX_MELODY_LENGTH	8

class MelodyDetector
{
public:
	MelodyDetector(uint16_t* melody, uint8_t melodyLen);
	~MelodyDetector();

	bool Input(uint16_t value);

private:
	uint8_t _melodyLength;
	uint8_t _pos;
	uint16_t _melody[MAX_MELODY_LENGTH];
};

#endif
