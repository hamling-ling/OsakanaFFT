#include "MelodyCommandReceiver.h"
#include "ResponsiveMelodyDetector.h"
#include <memory>

MelodyCommandReceiver::MelodyCommandReceiver(MelodyCommand_t* commands, int length)
	:
	kCommandNum(length)
{
	_commands = static_cast<ResponsiveMelodyDetector**>(malloc(sizeof(ResponsiveMelodyDetector*) * length));
	for (int i = 0; i < kCommandNum; i++) {
		MelodyCommand_t* cmd = &(commands[i]);
		_commands[i] = new ResponsiveMelodyDetector(cmd->melody0, cmd->melody0_len, cmd->melody1, cmd->melody1_len);
	}
	ResetAllDetectors();
}

MelodyCommandReceiver::~MelodyCommandReceiver()
{
	for (int i = 0; i < kCommandNum; i++) {
		delete _commands[i];
	}
	free(_commands);
	_commands = NULL;
}

MelodyCommandResponse_t MelodyCommandReceiver::Input(uint16_t value)
{
	return (this->*_inputFunc)(value);
}

MelodyCommandResponse_t MelodyCommandReceiver::ExcitedStateInput(uint16_t value)
{
	ResponsiveMelodyDetector* det = _commands[_resp.commandIdx];
	int result = det->Input(value);
	switch(result) {
	case 0:
		break;
	case -1:
		ResetAllDetectors();
		break;
	case 2:
		_resp.state = kMelodyCommandStateFired;
		MelodyCommandResponse_t retResp = _resp;
		ResetAllDetectors();

		return retResp;
	default:
		break;
	}

	return _resp;
}

MelodyCommandResponse_t MelodyCommandReceiver::BaseStateInput(uint16_t value)
{
	for (int i = 0; i < kCommandNum; i++) {
		ResponsiveMelodyDetector* det = _commands[i];
		if (det->Input(value) == 1) {
			_resp.commandIdx = i;
			_resp.state = kMelodyCommandStateExcited;
			_inputFunc = &MelodyCommandReceiver::ExcitedStateInput;
			return _resp;
		}
	}

	return _resp;
}

void MelodyCommandReceiver::ResetAllDetectors()
{
	for (int i = 0; i < kCommandNum; i++) {
		ResponsiveMelodyDetector* det = _commands[i];
		det->Reset();
	}
	memset(&_resp, 0, sizeof(_resp));
	_inputFunc = &MelodyCommandReceiver::BaseStateInput;
}
