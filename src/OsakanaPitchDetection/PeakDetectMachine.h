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
	void ParabolicInterp(MachineContext_t* ctx, Fp_t* xs, uint16_t N);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif
