#ifndef _PEAKDETECTMACHINE_H_
#define _PEAKDETECTMACHINE_H_

#include "PeakDetectMachineCommon.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

	typedef struct _PeekInfo {
		float value;
		uint16_t index;
	} PeekInfo;

	//struct _MachineContext_t;
	typedef struct _MachineContext_t MachineContext_t;
	typedef void (*StateFunc_t)(MachineContext_t* ctx, float x);

	MachineContext_t* CreatePeakDetectMachineContext();
	void DestroyPeakDetectMachineContext(MachineContext_t* ctx);
	void Input(MachineContext_t* ctx, float x);
	void ResetMachine(MachineContext_t* ctx);
	void GetKeyMaximums(MachineContext_t* ctx, float filter, PeekInfo* list, int listmaxlen, int *num);
	bool ParabolicInterp(MachineContext_t* ctx, int index, float* xs, int N, float* x);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif
