#include "EdgeDetector.h"
#include "VolumeComparator.h"
#if defined(USE_CONTINUITY_DETECTOR)
#include "ContinuityDetector.h"
#endif

EdgeDetector::EdgeDetector()
	:
	_lastNotifiedVal(0),
#if defined(USE_CONTINUITY_DETECTOR)
	_cd(new ContinuityDetector()),
#endif
	_vc(new VolumeComparator(VOLUME_THRESHOLD_OFF_TO_ON, VOLUME_THRESHOLD_ON_TO_OFF))
{
}

EdgeDetector::~EdgeDetector()
{
#if defined(USE_CONTINUITY_DETECTOR)
	delete _cd;
#endif
	delete _vc;
}

bool EdgeDetector::Input(uint16_t value, uint16_t volume)
{
	if (!_vc->Input(volume)) {
		// cutoff to small value
		value = 0;
	}
#if defined(USE_CONTINUITY_DETECTOR)
	if (_cd->Input(value))
#endif
	{
		// HISTROY_LEN times continued value!
		if (_lastNotifiedVal != value) {
			_lastNotifiedVal = value;
			return true;
		}
	}
	return false;
}

uint16_t EdgeDetector::CurrentNote()
{
	return _lastNotifiedVal;
}

void EdgeDetector::Reset()
{
	_vc->Input(0);
	_lastNotifiedVal = 0;
#if defined(USE_CONTINUITY_DETECTOR)
	_cd->Reset();
#endif
}
