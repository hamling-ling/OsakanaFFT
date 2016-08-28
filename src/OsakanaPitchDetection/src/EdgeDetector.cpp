#include "stdafx.h"
#include "EdgeDetector.h"


EdgeDetector::EdgeDetector(int val) : _val(val)
{
}


EdgeDetector::~EdgeDetector()
{
}

bool EdgeDetector::Input(uint16_t val)
{
	uint16_t _oldVal = _val;
	_val = val;
	return (_oldVal != _val);
}
