//#include "../include/EdgeDetector.h"
#include "EdgeDetector.h"
#include "ContinuityDetector.h"
#include "VolumeComparator.h"
#include <string.h>

EdgeDetector::EdgeDetector()
	:
	_lastNotifiedVal(0),
	_cd(new ContinuityDetector()),
	_vc(new VolumeComparator(VOLUME_THRESHOLD_OFF_TO_ON, VOLUME_THRESHOLD_ON_TO_OFF))
{
}

EdgeDetector::~EdgeDetector()
{
	delete _cd;
	delete _vc;
}

bool EdgeDetector::Input(uint16_t value,  uint16_t volume)
{
	if (!_vc->Input(volume)) {
		// cutoff to small value
		value = 0;
	}

	if (_cd->Input(value)) {
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
	_cd->Reset();
	_lastNotifiedVal = 0;;
}
