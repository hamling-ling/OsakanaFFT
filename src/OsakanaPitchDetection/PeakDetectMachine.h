#ifndef _PEAKDETECTMACHINE_H_
#define _PEAKDETECTMACHINE_H_

#include "OsakanaFp.h"

#define	kKeyMax	16

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

	typedef enum {
		kStateSearchingBell,
		kStateWalkingOnBell,
		kStateEnd,
		kStateNum
	} PeakDetectMachineState_t;

	typedef enum {
		kEventPosCross,
		kEventNegCross,
		kEventNmlData,
		kEventEndOfData,
		kEventNum
	} PeakDetectMachineEvent_t;

	typedef struct _PeekInfo {
		Fp_t value;
		uint16_t index;
	} PeekInfo;

	typedef struct _PeakInfoHIghP {
		Fp_t value;
		Fp_t index;
	} PeakInfoHIghP;

	//struct _MachineContext_t;
	typedef struct _MachineContext_t MachineContext_t;
	typedef void (*StateFunc_t)(MachineContext_t* ctx, Fp_t x);

	MachineContext_t* CreatePeakDetectMachineContext();
	void DestroyPeakDetectMachineContext(MachineContext_t* ctx);
	void Input(MachineContext_t* ctx, Fp_t x);
	void ResetMachine(MachineContext_t* ctx);
	void GetKeyMaximums(MachineContext_t* ctx, Fp_t filter, PeekInfo* list, int listmaxlen, int *num);
	bool ParabolicInterp(MachineContext_t* ctx, int index, Fp_t* xs, int N, Fp_t* x);

	static const char* kNoteStrings[] = {
		"A", "BÅÛ", "B", "C",
		"C#", "D", "D#", "E",
		"F", "F#", "G", "G#",
	};

	static const uint8_t kNoteTable[] = {
		0,   0,   0,   5,   0,   8,   5,   2,   0,  10,   8,   6,   5,   3,   2,
		1,   0,  11,  10,   9,   8,   7,   6,   5,   5,   4,   3,   3,   2,   1,
		1,   0,   0,  11,  11,  10,  10,   9,   9,   8,   8,   7,   7,   7,   6,
		6,   5,   5,   5,   4,   4,   4,   3,   3,   3,   2,   2,   2,   1,   1,
		1,   1,   0,   0,   0,  11,  11,  11,  11,  10,  10,  10,  10,   9,   9,
		9,   9,   8,   8,   8,   8,   8,   7,   7,   7,   7,   7,   6,   6,   6,
		6,   6,   5,   5,   5,   5,   5,   4,   4,   4,   4,   4,   4,   3,   3,
		3,   3,   3,   3,   2,   2,   2,   2,   2,   2,   2,   1,   1,   1,   1,
		1,   1,   1,   0,   0,   0,   0,   0,   0,   0,  11,  11,  11,  11,  11,
		11,  11,  11,  10,  10,  10,  10,  10,  10,  10,  10,   9,   9,   9,   9,
		9,   9,   9,   9,   8,   8,   8,   8,   8,   8,   8,   8,   8,   8,   7,
		7,   7,   7,   7,   7,   7,   7,   7,   6,   6,   6,   6,   6,   6,   6,
		6,   6,   6,   6,   5,   5,   5,   5,   5,   5,   5,   5,   5,   5,   4,
		4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   3,   3,   3,   3,
		3,   3,   3,   3,   3,   3,   3,   3,   2,   2,   2,   2,   2,   2,   2,
		2,   2,   2,   2,   2,   2,   1,   1,   1,   1,   1,   1,   1,   1,   1,
		1,   1,   1,   1,   1,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,  11,  11,  11,  11,  11,  11,  11,  11,  11,  11,  11,
		11,  11,  11,  11,  11,  10,  10,  10,  10,  10,  10,  10,  10,  10,  10,
		10,  10,  10,  10,  10,  10,   9,   9,   9,   9,   9,   9,   9,   9,   9,
		9,   9,   9,   9,   9,   9,   9,   9,   8,   8,   8,   8,   8,   8,   8,
		8,   8,   8,   8,   8,   8,   8,   8,   8,   8,   8,   8,   7,   7,   7,
		7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,
		7,   6,   6,   6,   6,   6,   6,   6,   6,   6,   6,   6,   6,   6,   6,
		6,   6,   6,   6,   6,   6,   6,   5,   5,   5,   5,   5,   5,   5,   5,
		5,   5,   5,   5,   5,   5,   5,   5,   5,   5,   5,   5,   5,   4,   4,
		4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,
		4,   4,   4,   4,   4,   4,   3,   3,   3,   3,   3,   3,   3,   3,   3,
		3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,
		3,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
		2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   1,   1,   1,
		1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
		1,   1,   1,   1,   1,   1,   1,   1,   1,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   0,   0,  -0,  -0,  -0,  -0,  -0,  -0,
		-0,  -0
	};

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif
