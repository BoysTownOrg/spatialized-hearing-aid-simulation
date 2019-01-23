#pragma once

#include <common-includes/RuntimeError.h>
#include <common-includes/Interface.h>
#include <vector>
#include <string>

struct GlobalTestParameters;

class StimulusPlayer {
public:
	INTERFACE_OPERATIONS(StimulusPlayer);
	RUNTIME_ERROR(DeviceFailure);

	struct Initialization {
		GlobalTestParameters *global;
	};
	virtual void initialize(Initialization) = 0;
	RUNTIME_ERROR(InitializationFailure);

	struct PlayRequest {
		std::string audioFilePath;
		std::string audioDevice;
		double level_dB_Spl;
	};
	virtual void play(PlayRequest) = 0;
	RUNTIME_ERROR(RequestFailure);

	virtual std::vector<std::string> audioDeviceDescriptions() = 0;
	virtual std::vector<int> preferredProcessingSizes() = 0;
	virtual bool isPlaying() = 0;
};