#ifndef OSAKANAPITCHDETECTIONFP_H_
#define OSAKANAPITCHDETECTIONFP_H_

#include <string>
#include "OsakanaPitchDetectionCommon.h"
#include "PeakDetectMachineFp.h"

int DetectPitchFp(OsakanaFpFftContext_t* ctx, MachineContextFp_t* mctx, const std::string& filename);

#endif
