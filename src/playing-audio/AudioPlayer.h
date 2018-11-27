#pragma once

#include <common-includes/Interface.h>
#include <audio-stream-processing/AudioFrameProcessor.h>
#include <string>

class AudioPlayer {
public:
	INTERFACE_OPERATIONS(AudioPlayer);
	struct Parameters {
		struct ForAudioDevice {
			int framesPerBuffer;
			int sampleRate;
		};
		std::string audioFilePath;
		AudioFrameProcessor::Parameters forHearingAidSimulation;
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
