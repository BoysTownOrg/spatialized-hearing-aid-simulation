#pragma once

#include "playing-audio-exports.h"
#include "AudioPlayer.h"
#include "Model.h"

class AudioPlayerModel : public Model {
	std::shared_ptr<AudioPlayerFactory> factory;
public:
	PLAYING_AUDIO_API explicit AudioPlayerModel(
		std::shared_ptr<AudioPlayerFactory> factory
	);
	PLAYING_AUDIO_API void playRequest(PlayRequest) override;
private:
	void throwIfNotDouble(std::string x, std::string identifier);
	void throwIfNotPositiveInteger(std::string x, std::string identifier);
	void throwRequestFailure(std::string x, std::string identifier);
};
