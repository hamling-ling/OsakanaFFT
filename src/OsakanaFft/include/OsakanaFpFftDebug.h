#ifndef _OSAKANAFPFFTDEBUG_H_
#define _OSAKANAFPFFTDEBUG_H_

#include "OsakanaFftDebug.h"

#if defined(_DEBUG)
#define DCOMPLEXFp(cs, num) 	for (int i = 0; i < num; i++) { \
								fp_complex_str(&cs[i], buf, sizeof(buf)); \
								DLOG("%s", buf); \
							}
#define DFPSFp(fps, num)		for (int i = 0; i < num; i++) { \
								Fp2CStr(fps[i], buf, sizeof(buf)); \
								DLOG("%s", buf); \
							}
#else
#define DCOMPLEXFp(cs, num)
#define DFPSFp(fps, num)
#endif

#endif
