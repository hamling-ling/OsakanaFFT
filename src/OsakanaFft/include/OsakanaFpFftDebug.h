#ifndef _DEBUG_H_
#define _DEBUG_H_

#define DEBUG_OUTPUT_NUM    512
#if defined(_DEBUG)
#define DLOG(fmt, ...) fprintf(stderr, fmt "\n", __VA_ARGS__);
#define DCOMPLEX(cs, num) 	for (int i = 0; i < num; i++) { \
								fp_complex_str(&cs[i], buf, sizeof(buf)); \
								DLOG("%s", buf); \
							}
#define DFPS(fps, num)		for (int i = 0; i < num; i++) { \
								Fp2CStr(fps[i], buf, sizeof(buf)); \
								DLOG("%s", buf); \
							}
#define DRAWDATA(x, num)	for (int i = 0; i < num; i++) { \
								DLOG("%u", x[i].re); \
							}
#else
#define DLOG(fmt, ...)
#define DCOMPLEX(cs, num)
#define DFPS(fps, num)
#define DRAWDATA(x, num)
#endif

#endif
