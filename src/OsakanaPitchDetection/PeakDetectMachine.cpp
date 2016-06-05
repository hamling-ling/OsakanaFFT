#include "stdafx.h"
#include <assert.h>
#include "PeakDetectMachine.h"

static PeakDetectMachineEvent_t SearchingBell_DetectEvent(MachineContext_t* ctx, Fp_t x);
static PeakDetectMachineEvent_t WalkingOnBell_DetectEvent(MachineContext_t* ctx, Fp_t x);
static PeakDetectMachineEvent_t End_DetectEvent(MachineContext_t* ctx, Fp_t x);
static void ChangeState(MachineContext_t* ctx, PeakDetectMachineState_t state);

static void SeachingBell_PosCross(MachineContext_t* ctx, Fp_t x);
static void SeachingBell_NegCross(MachineContext_t* ctx, Fp_t x);
static void SeachingBell_NmlData(MachineContext_t* ctx, Fp_t x);
static void SeachingBell_EndOfData(MachineContext_t* ctx, Fp_t x);

static void WalkingOnBell_PosCross(MachineContext_t* ctx, Fp_t x);
static void WalkingOnBell_NegCross(MachineContext_t* ctx, Fp_t x);
static void WalkingOnBell_NmlData(MachineContext_t* ctx, Fp_t x);
static void WalkingOnBell_EndOfData(MachineContext_t* ctx, Fp_t x);

static void End_PosCross(MachineContext_t* ctx, Fp_t x);
static void End_NegCross(MachineContext_t* ctx, Fp_t x);
static void End_NmlData(MachineContext_t* ctx, Fp_t x);
static void End_EndOfData(MachineContext_t* ctx, Fp_t x);

typedef PeakDetectMachineEvent_t (*EventDetector_t)(MachineContext_t* ctx, Fp_t x);

static StateFunc_t s_funcs[kStateNum][kEventNum] = {
	{ SeachingBell_PosCross,	SeachingBell_NegCross,	SeachingBell_NmlData,	SeachingBell_EndOfData },
	{ WalkingOnBell_PosCross,	WalkingOnBell_NegCross, WalkingOnBell_NmlData , WalkingOnBell_EndOfData },
	{ End_PosCross,				End_NegCross,			End_NmlData,			End_EndOfData }
};

static EventDetector_t s_eventDetectors[kStateNum] = {
	SearchingBell_DetectEvent, WalkingOnBell_DetectEvent, End_DetectEvent
};

typedef struct _MachineContext_t {
	uint16_t maxDataNum;
	uint16_t currentIndex;
	PeekInfo lastInput;
	// collection of key maximum for each bell
	PeekInfo keyMaxs[kKeyMax];
	uint16_t keyMaxsNum;
	// max of all bell
	PeekInfo globalKeyMax;
	// max of current bell
	PeekInfo localKeyMax;
	PeakDetectMachineState_t state;
	StateFunc_t (*funcs)[kEventNum];
	EventDetector_t* detectors;
} MachineContext_t;

/////////////////////////////////////////////////////////////////////
// Public
/////////////////////////////////////////////////////////////////////

MachineContext_t* CreatePeakDetectMachineContext()
{
	MachineContext_t* ctx = (MachineContext_t*)malloc(sizeof(MachineContext_t));
	if (ctx == NULL) {
		return NULL;
	}
	ResetMachine(ctx);

	return ctx;
}


void DestroyPeakDetectMachineContext(MachineContext_t* ctx)
{
	free(ctx);
}

void Input(MachineContext_t* ctx, Fp_t x)
{
	EventDetector_t detector = ctx->detectors[ctx->state];
	PeakDetectMachineEvent_t evt = detector(ctx, x);
	
	StateFunc_t stateFunc = ctx->funcs[ctx->state][(int)evt];
	stateFunc(ctx, x);
}

void ResetMachine(MachineContext_t* ctx)
{
	memset(ctx, 0, sizeof(MachineContext_t));
	ctx->funcs = s_funcs;
	ctx->detectors = s_eventDetectors;
}

void ParabolicInterp(MachineContext_t* ctx, Fp_t* xs, uint16_t N)
{
	for (int i = 0; i < ctx->keyMaxsNum; i++) {
		PeekInfo* pPeak = &(ctx->keyMaxs[i]);
		if (0 == pPeak->index || N <= pPeak->index + 1) {
			continue;
		}
		Fp_t x0 = Int2Fp(pPeak->index - 1);
		Fp_t x1 = Int2Fp(pPeak->index + 1);
		Fp_t y0 = xs[pPeak->index - 1];
		Fp_t y1 = xs[pPeak->index + 1];

		Fp_t x0x0 = FpMul(x0, x0);
		Fp_t x1x1 = FpMul(x1, x1);
		Fp_t a = FpDiv((y0 - y1), (x0x0 - x1x1));
		Fp_t b = y0 - FpMul(a, x0x0);

	}
}

/////////////////////////////////////////////////////////////////////
// Private
/////////////////////////////////////////////////////////////////////

static void UpdateValueHistory(MachineContext_t* ctx, Fp_t x)
{
	ctx->lastInput.value = x;
	ctx->lastInput.index = ctx->currentIndex;
	ctx->currentIndex++;
}

static void UpdateLocalKeyMax(MachineContext_t* ctx, Fp_t x, uint16_t index)
{
	PeekInfo localMax = { x, index };
	ctx->localKeyMax = localMax;
}

static void PushLocalKeyMax(MachineContext_t* ctx)
{
	ctx->keyMaxs[ctx->keyMaxsNum] = ctx->localKeyMax;
	ctx->keyMaxsNum++;
	if (ctx->globalKeyMax.value < ctx->localKeyMax.value) {
		ctx->globalKeyMax = ctx->localKeyMax;
	}
}

static PeakDetectMachineEvent_t SearchingBell_DetectEvent(MachineContext_t* ctx, Fp_t x) {
	if (ctx->lastInput.value < 0 && 0 <= x) {
		return kEventPosCross;
	}
	else if(ctx->maxDataNum <= ctx->currentIndex+1){
		return kEventNmlData;
	}
	else {
		return kEventEndOfData;
	}
}

static PeakDetectMachineEvent_t WalkingOnBell_DetectEvent(MachineContext_t* ctx, Fp_t x) {
	if (0 <= ctx->lastInput.value && x < 0) {
		return kEventNegCross;
	}
	else {
		return kEventNmlData;
	}
}

static PeakDetectMachineEvent_t End_DetectEvent(MachineContext_t* ctx, Fp_t x) {
	return kEventEndOfData;
}

static void ChangeState(MachineContext_t* ctx, PeakDetectMachineState_t state)
{
	printf("state %d -> %d\n", ctx->state, state);
	ctx->state = state;
}

static void SeachingBell_PosCross(MachineContext_t* ctx, Fp_t x)
{
	// reset local max
	UpdateLocalKeyMax(ctx, x, ctx->currentIndex);
	UpdateValueHistory(ctx, x);
	ChangeState(ctx, kStateWalkingOnBell);
}

static void SeachingBell_NegCross(MachineContext_t* ctx, Fp_t x)
{
	UpdateValueHistory(ctx, x);
	ChangeState(ctx, kStateSearchingBell);
}

static void SeachingBell_NmlData(MachineContext_t* ctx, Fp_t x)
{
	UpdateValueHistory(ctx, x);
}

static void SeachingBell_EndOfData(MachineContext_t* ctx, Fp_t x)
{
	UpdateValueHistory(ctx, x);
	ChangeState(ctx, kStateEnd);
}

static void WalkingOnBell_PosCross(MachineContext_t* ctx, Fp_t x)
{
	assert(true);
}

static void WalkingOnBell_NegCross(MachineContext_t* ctx, Fp_t x)
{
	PushLocalKeyMax(ctx);
	UpdateValueHistory(ctx, x);
	ChangeState(ctx, kStateSearchingBell);
}

static void WalkingOnBell_NmlData(MachineContext_t* ctx, Fp_t x)
{
	// update local key max
	if (ctx->localKeyMax.value < x) {
		UpdateLocalKeyMax(ctx, x, ctx->currentIndex);
	}

	UpdateValueHistory(ctx, x);
}

static void WalkingOnBell_EndOfData(MachineContext_t* ctx, Fp_t x)
{
	UpdateValueHistory(ctx, x);
}

static void End_PosCross(MachineContext_t* ctx, Fp_t x)
{
	UpdateValueHistory(ctx, x);
}

static void End_NegCross(MachineContext_t* ctx, Fp_t x)
{
	UpdateValueHistory(ctx, x);
}

static void End_NmlData(MachineContext_t* ctx, Fp_t x)
{
	UpdateValueHistory(ctx, x);
}

static void End_EndOfData(MachineContext_t* ctx, Fp_t x)
{
	UpdateValueHistory(ctx, x);
}
