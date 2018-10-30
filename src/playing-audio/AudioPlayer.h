#pragma once

#include <common-includes/Interface.h>
#include <string>

class AudioPlayer {
public:
	INTERFACE_OPERATIONS(AudioPlayer);
	struct Parameters {
		struct ForHearingAidSimulation {
			std::string leftDslPrescriptionFilePath;
			std::string rightDslPrescriptionFilePath;
			double level_dB_Spl;
			double attack_ms;
			double release_ms;
			int windowSize;
			int chunkSize;
		};
		struct ForAudioDevice {
			int framesPerBuffer;
			int sampleRate;
		};
		std::string audioFilePath;
		std::string brirFilePath;
		ForHearingAidSimulation forHearingAidSimulation;
		ForAudioDevice forAudioDevice;
	};
	virtual void play() = 0;
};

#include <memory>

class AudioPlayerFactory {
public:
	INTERFACE_OPERATIONS(AudioPlayerFactory);
	virtual std::shared_ptr<AudioPlayer> make(AudioPlayer::Parameters) = 0;
};
