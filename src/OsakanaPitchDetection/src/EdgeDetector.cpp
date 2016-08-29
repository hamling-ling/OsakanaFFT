#include "EdgeDetector.h"
#include <string.h>

EdgeDetector::EdgeDetector(int val) : _lastNotifiedVal(0)
{
    memset(_history, 0, sizeof(_history));
}

EdgeDetector::~EdgeDetector()
{
}

bool EdgeDetector::Input(uint16_t val)
{
	_history[0] = _history[1];
	_history[1] = _history[2];
	_history[2] = val;
	if(_history[1] == _history[2]) {
	    if(_history[0] != _history[1]) {
	        if(_history[1] != _lastNotifiedVal) {
	            _lastNotifiedVal = val;
	            return true;
	        }
	    }
	}
	return false;
}
