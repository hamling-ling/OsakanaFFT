#ifndef OSAKANAPITCHDETECTION_H_
#define OSAKANAPITCHDETECTION_H_

#include "OsakanaPitchDetectionCommon.h"
#include "PeakDetectMachine.h"

int DetectPitch(OsakanaFftContext_t* ctx, MachineContext_t* mctx, const std::string& filename);

#endif
