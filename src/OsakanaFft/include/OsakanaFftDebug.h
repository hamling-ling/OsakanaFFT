#ifndef _OSAKANAFFTDEBUG_H_
#define _OSAKANAFFTDEBUG_H_

#if defined(_DEBUG)

#if !defined(LOG_NEWLINE) 
#define LOG_NEWLINE "\n"
#endif

char debug_output_buf_[128] = { 0 };// not thread safe!
#if defined(LOG_PRINTF)
char debug_output_buf2_[128] = { 0 };
#define DLOG(...)			snprintf(debug_output_buf2_,sizeof(debug_output_buf2_), __VA_ARGS__); \
                            LOG_PRINTF(debug_output_buf2_); \
                            LOG_PRINTF(LOG_NEWLINE);

#else
#define DLOG(fmt, ...)		fprintf(stderr, fmt LOG_NEWLINE, __VA_ARGS__);
#endif

#define DCOMPLEX(cs, num) 	for (int i = 0; i < num; i++) { \
								complex_str(&cs[i], debug_output_buf_, sizeof(debug_output_buf_)); \
								DLOG("%s", debug_output_buf_); \
							}
#define DFPS(x, num)		for (int i = 0; i < num; i++) { \
								DLOG("%f", x[i]); \
							}
#define DRAWDATA(x, num)	for (int i = 0; i < num; i++) {	\
 								DLOG("%u", x[i].re); \
 							}
#define DRAWDATAf(x, num)	for (int i = 0; i < num; i++) {	\
 								DLOG("%f", x[i].re); \
 							}
#else

#define DLOG(fmt, ...)
#define DCOMPLEX(cs, num)
#define DFPS(fps, num)
#define DRAWDATA(x, num)
#define DRAWDATAf(x, num)
#endif

#endif

