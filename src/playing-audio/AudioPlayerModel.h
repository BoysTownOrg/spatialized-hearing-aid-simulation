#pragma once

#ifdef PLAYING_AUDIO_EXPORTS
	#define PLAYING_AUDIO_API __declspec(dllexport)
#else
	#define PLAYING_AUDIO_API __declspec(dllimport)
#endif

#include <common-includes/Interface.h>
#include <string>

class AudioPlayer {
public:
	INTERFACE_OPERATIONS(AudioPlayer);
	struct Parameters {
		std::string audioFilePath;
	};
};

#include <memory>

class AudioPlayerFactory {
public:
	INTERFACE_OPERATIONS(AudioPlayerFactory);
	virtual std::shared_ptr<AudioPlayer> make(AudioPlayer::Parameters) = 0;
};

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
