#ifndef _PEAKDETECTMACHINECOMMON_H_
#define _PEAKDETECTMACHINECOMMON_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define	kKeyMax	16

typedef enum {
	kStateSearchingBell,
	kStateWalkingOnBell,
	kStateEnd,
	kStateNum
} PeakDetectMachineState_t;

typedef enum {
	kEventPosCross,
	kEventNegCross,
	kEventNmlData,
	kEventEndOfData,
	kEventNum
} PeakDetectMachineEvent_t;

static const char* kNoteStrings[] = {
	"C",	"C#",	"D",	"D#",
	"E",	 "F",	"F#",	"G",
	"G#",	"A",	"Bb",	"B"
	};

// N=256
// sampling rate = 0.0740234375(HRM1017).
// todo:this platform dependent table should not be here
static const uint8_t kNoteTable[] = {
	255,  129,  117,  110,  105,  101,   98,   95,   93,   91,   89,   87,
	86,   84,   83,   82,   81,   80,   79,   78,   77,   76,   75,   74,
	74,   73,   72,   72,   71,   70,   70,   69,   69,   68,   68,   67,
	67,   66,   66,   65,   65,   64,   64,   64,   63,   63,   62,   62,
	62,   61,   61,   61,   60,   60,   60,   59,   59,   59,   58,   58,
	58,   58,   57,   57,   57,   56,   56,   56,   56,   55,   55,   55,
	55,   54,   54,   54,   54,   53,   53,   53,   53,   53,   52,   52,
	52,   52,   52,   51,   51,   51,   51,   51,   50,   50,   50,   50,
	50,   49,   49,   49,   49,   49,   49,   48,   48,   48,   48,   48,
	48,   47,   47,   47,   47,   47,   47,   47,   46,   46,   46,   46,
	46,   46,   46,   45,   45,   45,   45,   45,   45,   45,   44,   44,
	44,   44,   44,   44,   44,   44,   43,   43,   43,   43,   43,   43,
	43,   43,   42,   42,   42,   42,   42,   42,   42,   42,   41,   41,
	41,   41,   41,   41,   41,   41,   41,   41,   40,   40,   40,   40,
	40,   40,   40,   40,   40,   39,   39,   39,   39,   39,   39,   39,
	39,   39,   39,   39,   38,   38,   38,   38,   38,   38,   38,   38,
	38,   38,   37,   37,   37,   37,   37,   37,   37,   37,   37,   37,
	37,   37,   36,   36,   36,   36,   36,   36,   36,   36,   36,   36,
	36,   36,   35,   35,   35,   35,   35,   35,   35,   35,   35,   35,
	35,   35,   35,   34,   34,   34,   34,   34,   34,   34,   34,   34,
	34,   34,   34,   34,   34,   33,   33,   33,   33,   33,   33,   33,
	33,   33,   33,   33
};

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif
