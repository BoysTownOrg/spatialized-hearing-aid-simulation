#pragma once

#include <common-includes/Interface.h>
#include <string>

class AudioPlayer {
public:
	INTERFACE_OPERATIONS(AudioPlayer);
	struct Parameters {
		std::string leftDslPrescriptionFilePath;
		std::string rightDslPrescriptionFilePath;
		std::string audioFilePath;
		std::string brirFilePath;
	};
};

#include <memory>

class AudioPlayerFactory {
public:
	INTERFACE_OPERATIONS(AudioPlayerFactory);
	virtual std::shared_ptr<AudioPlayer> make(AudioPlayer::Parameters) = 0;
};
