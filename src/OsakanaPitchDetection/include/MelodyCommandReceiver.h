#ifndef _MELODYCOMMANDRECEIVER_H_
#define _MELODYCOMMANDRECEIVER_H_

#include <stdint.h>

typedef uint8_t MelodyCommandState_t;

static const uint8_t kMelodyCommandStateBase = 0;
static const uint8_t kMelodyCommandStateExcited = 1;
static const uint8_t kMelodyCommandStateFired = 2;

typedef struct MelodyCommand_tag {
	uint16_t* melody0;
	uint16_t* melody1;
	uint8_t melody0_len;
	uint8_t melody1_len;
} MelodyCommand_t;

typedef struct MelodyCommandResponse_tag {
	
	uint8_t commandIdx;
	MelodyCommandState_t state;

	bool IsEmpty() {
		return (commandIdx == 0 && state == kMelodyCommandStateBase);
	}
} MelodyCommandResponse_t;

class ResponsiveMelodyDetector;

class MelodyCommandReceiver
{
public:
	MelodyCommandReceiver(MelodyCommand_t* commands, int length);
	~MelodyCommandReceiver();

	MelodyCommandResponse_t Input(uint16_t value);

private:
	const int kCommandNum;
	ResponsiveMelodyDetector** _commands;
	MelodyCommandResponse_t _resp;
	typedef MelodyCommandResponse_t (MelodyCommandReceiver::*InputFunc_t)(uint16_t);
	InputFunc_t _inputFunc;

	MelodyCommandResponse_t BaseStateInput(uint16_t value);
	MelodyCommandResponse_t ExcitedStateInput(uint16_t value);

	void ResetAllDetectors();
};

#endif
