#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include "OsakanaPitchDetection.h"
#include "OsakanaPitchDetectionFp.h"

int main(int argc, char* argv[])
{
	if (argc < 1) {
		DLOG("need to give file in command-line paremeter");
		return 1;
	}

#if 1
	MachineContextFp_t* mctx = NULL;
	mctx = CreatePeakDetectMachineContextFp();

	OsakanaFpFftContext_t* ctx;
	if (InitOsakanaFpFft(&ctx, N, LOG2N) != 0) {
		DLOG("InitOsakanaFpFft error");
		return 1;
	}

	while (1) {
		DetectPitchFp(ctx, mctx, argv[1]);
		ResetMachineFp(mctx);
		break;// for debug
	}
	CleanOsakanaFpFft(ctx);

	DestroyPeakDetectMachineContextFp(mctx);
#else
	MachineContext_t* mctx = NULL;
	mctx = CreatePeakDetectMachineContext();

	OsakanaFftContext_t* ctx;
	if (InitOsakanaFft(&ctx, N, LOG2N) != 0) {
		DLOG("InitOsakanaFpFft error");
		return 1;
	}

	while (1) {
		DetectPitch(ctx, mctx, argv[1]);
		ResetMachine(mctx);
		break;// for debug
	}

	DestroyPeakDetectMachineContext(mctx);
#endif

	return 0;
}

