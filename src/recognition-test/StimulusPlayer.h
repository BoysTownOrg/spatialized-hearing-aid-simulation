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

	struct Preparation {
		std::string audioFilePath;
		std::string audioDevice;
		double level_dB_Spl;
	};
	virtual void prepareToPlay(Preparation) = 0;
	RUNTIME_ERROR(PreparationFailure);

	virtual void play() = 0;
	virtual std::vector<std::string> audioDeviceDescriptions() = 0;
	virtual bool isPlaying() = 0;
	virtual void stop() = 0;
};