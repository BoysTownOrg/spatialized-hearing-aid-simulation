#pragma once

#ifdef PLAYING_AUDIO_EXPORTS
	#define PLAYING_AUDIO_API __declspec(dllexport)
#else
	#define PLAYING_AUDIO_API __declspec(dllimport)
#endif

class AudioPlayerFactory {

};

#include "Model.h"

class AudioPlayerModel : public Model {
public:
	PLAYING_AUDIO_API explicit AudioPlayerModel(
		std::shared_ptr<AudioPlayerFactory>
	);
	PLAYING_AUDIO_API void playRequest(PlayRequest) override;
private:
	void throwIfNotDouble(std::string x, std::string identifier);
	void throwIfNotPositiveInteger(std::string x, std::string identifier);
	void throwRequestFailure(std::string x, std::string identifier);
};
