#ifndef OSAKANAPITCHDETECTIONFP_H_
#define OSAKANAPITCHDETECTIONFP_H_

#include <string>
#include "OsakanaPitchDetectionCommon.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

	typedef struct _MachineContextFp_t MachineContextFp_t;
	int DetectPitchFp(OsakanaFpFftContext_t* ctx, MachineContextFp_t* mctx, const std::string& filename);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
