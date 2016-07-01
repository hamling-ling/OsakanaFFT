#ifndef _GLOBALCONFIG_H_
#define _GLOBALCONFIG_H_

#if 0 // N=512, fixed Q7.8 fixed point
#define USE_HARDCORD_TABLE
//#define _USE_Q7_8_FIXEDPOINT
#define _USE_Q1_14_FIXEDPOINT
#define USE_BIT_REVERSE_N512
#define USE_TWIDDLE_TABLE_N512
#endif

#if 1 // N=1024, floating point
#define USE_HARDCORD_TABLE
#endif

#endif
