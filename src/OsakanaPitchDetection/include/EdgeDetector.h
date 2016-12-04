#ifndef _EDGEDETECTOR_H_
#define _EDGEDETECTOR_H_

#include <stdint.h>

#if defined(USE_CONTINUITY_DETECTOR)
class ContinuityDetector;
#endif

class VolumeComparator;

// threshold of rawdata_max - rawdata_min
#define VOLUME_THRESHOLD_OFF_TO_ON	180
#define VOLUME_THRESHOLD_ON_TO_OFF	64

class EdgeDetector
{
public:
	EdgeDetector();
	~EdgeDetector();
	/**
	 *	detect note change.
	 *	@return	true when edge(value change) detected
	 *	@note	given value culd different from current note.
	 *			so use CurrentNote() to get latest internal value.
	 */
	bool Input(uint16_t value, uint16_t volume);
	uint16_t CurrentNote();
	void Reset();

private:
	uint16_t _lastNotifiedVal;
	VolumeComparator* _vc;
#if defined(USE_CONTINUITY_DETECTOR)
	ContinuityDetector* _cd;
#endif
};

#endif
