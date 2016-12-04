#ifndef _NOTETABLES_H_
#define _NOTETABLES_H_


#if 0
// N=256
// sampling rate = 0.0740234375(HRM1017).
// todo:this platform dependent table should not be here
static const uint8_t kNoteTable[] = {
	0,   128,  116,  109,  104,  100,   97,   94,   92,   90,   88,   86,
	85,   83,   82,   81,   80,   79,   78,   77,   76,   75,   74,   73,
	73,   72,   71,   71,   70,   69,   69,   68,   68,   67,   67,   66,
	66,   65,   65,   64,   64,   63,   63,   62,   62,   62,   61,   61,
	61,   60,   60,   60,   59,   59,   59,   58,   58,   58,   57,   57,
	57,   56,   56,   56,   56,   55,   55,   55,   55,   54,   54,   54,
	54,   53,   53,   53,   53,   52,   52,   52,   52,   52,   51,   51,
	51,   51,   50,   50,   50,   50,   50,   50,   49,   49,   49,   49,
	49,   48,   48,   48,   48,   48,   48,   47,   47,   47,   47,   47,
	47,   46,   46,   46,   46,   46,   46,   45,   45,   45,   45,   45,
	45,   45,   44,   44,   44,   44,   44,   44
};
#endif

#if 0
// N=256
// sampling rate = 0.07558072916669921875(GR-Kurumi with min, max)
// todo:this platform dependent table should not be here
static const uint8_t kNoteTable[] = {
	0,   128,  116,  109,  104,  100,   97,   95,   92,   90,   88,   87,
	85,   84,   83,   81,   80,   79,   78,   77,   76,   76,   75,   74,
	73,   73,   72,   71,   71,   70,   69,   69,   68,   68,   67,   67,
	66,   66,   65,   65,   64,   64,   64,   63,   63,   62,   62,   62,
	61,   61,   61,   60,   60,   60,   59,   59,   59,   58,   58,   58,
	57,   57,   57,   57,   56,   56,   56,   55,   55,   55,   55,   54,
	54,   54,   54,   53,   53,   53,   53,   53,   52,   52,   52,   52,
	52,   51,   51,   51,   51,   51,   50,   50,   50,   50,   50,   49,
	49,   49,   49,   49,   49,   48,   48,   48,   48,   48,   48,   47,
	47,   47,   47,   47,   47,   46,   46,   46,   46,   46,   46,   46,
	45,   45,   45,   45,   45,   45,   45,   44
};
#endif

// N=256
// sampling rate = 0.07558072916669921875(GR-Kurumi with min, max)
// x8 precision
#if 0
#include "NoteTable8.h"
#include "NoteTable8IndexRange.h"
#endif
#if 0
#include "NoteTable8_N512.h"
#include "NoteTable8IndexRange_N512.h"
#endif
#if 1
#include "NoteTable8_N1024.h"
#include "NoteTable8IndexRange_N1024.h"
#endif


#endif // _NOTETABLES_H_