#ifndef _EDGEDETECTOR_H_
#define _EDGEDETECTOR_H_

#include <stdint.h>

class EdgeDetector
{
public:
	EdgeDetector(int val);
	~EdgeDetector();
	bool Input(uint16_t val);
private:
	uint16_t _history[3];
	uint16_t _lastNotifiedVal;
};

#endif
