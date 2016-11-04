#include "PitchDiagnostic.h"

PitchDiagnostic::PitchDiagnostic(uint16_t interval)
	: kInterval(interval)
{
	_interval = 0;
	_sum = 0;
	_func = &PitchDiagnostic::diagnoseOnNonEdgeState;
}


PitchDiagnostic::~PitchDiagnostic()
{
}

DiagnoseResult_t PitchDiagnostic::Diagnose(int8_t pitch, bool edge)
{
	if (pitch == INT8_MIN) {
		Reset();
		return kDiagnoseResultNone;
	}

	return (this->*_func)(pitch, edge);
}

void PitchDiagnostic::Reset()
{
	_interval = 0;
	_sum = 0;
	_func = &PitchDiagnostic::diagnoseOnNonEdgeState;
}

DiagnoseResult_t PitchDiagnostic::diagnoseOnEdgeState(int8_t pitch, bool edge)
{
	if (edge) {
		Reset();
		return kDiagnoseResultNone;
	}

	_sum += pitch;
	_interval++;
	if (kInterval <= _interval) {
		uint16_t kIntervalHalf = (kInterval >> 1);
		DiagnoseResult_t ret = kDiagnoseResultGood;
		if (kIntervalHalf < _sum) {
			ret = kDiagnoseResultHigh;
		}
		else if (_sum < -kIntervalHalf) {
			ret = kDiagnoseResultLow;
		}
		// reset only interval. everthing else stay the same
		_interval = 0;
		return ret;
	}

	return kDiagnoseResultNone;
}

DiagnoseResult_t PitchDiagnostic::diagnoseOnNonEdgeState(int8_t pitch, bool edge)
{
	if (edge) {
		_sum = pitch;
		_func = &PitchDiagnostic::diagnoseOnEdgeState;
		_interval++;
	}

	return kDiagnoseResultNone;
}

