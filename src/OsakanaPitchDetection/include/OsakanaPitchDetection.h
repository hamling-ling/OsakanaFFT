#ifndef OSAKANAPITCHDETECTION_H_
#define OSAKANAPITCHDETECTION_H_

#include "OsakanaPitchDetectionCommon.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

	typedef struct _MachineContext_t MachineContext_t;

	int DetectPitch(OsakanaFftContext_t* ctx, MachineContext_t* mctx, const std::string& filename);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
