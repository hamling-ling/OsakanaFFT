#ifndef _PITCHDIAGNOSTIC_H_
#define _PITCHDIAGNOSTIC_H_

#include <stdint.h>

typedef uint8_t DiagnoseResult_t;

#define DiagnoseResultNone		0
#define DiagnoseResultHigh		1
#define DiagnoseResultLow		2
#define DiagnoseResultGood		3

class PitchDiagnostic
{
public:
	PitchDiagnostic(uint16_t interval);
	~PitchDiagnostic();
	DiagnoseResult_t Diagnose(int8_t pitch, bool edge);
	void Reset();
private:
	const uint16_t kInterval;
	uint16_t _interval;
	int16_t _sum;
	typedef DiagnoseResult_t(PitchDiagnostic::*func_t)(int8_t, bool);
	func_t _func;

	DiagnoseResult_t diagnoseOnEdgeState(int8_t pitch, bool edge);
	DiagnoseResult_t diagnoseOnNonEdgeState(int8_t pitch, bool edge);
};

#endif
