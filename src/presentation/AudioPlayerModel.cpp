#include "AudioPlayerModel.h"

void AudioPlayerModel::throwIfNotDouble(std::string x, std::string identifier) {
	try {
		std::stod(x);
	}
	catch (const std::invalid_argument &) {
		throw RequestFailure{
			"'" + x + "' is not a valid " + identifier + "." };
	}
}

void AudioPlayerModel::playRequest(PlayRequest request) {
	throwIfNotDouble(request.level_dB_Spl, "level");
	throwIfNotDouble(request.attack_ms, "attack time");
	throwIfNotDouble(request.release_ms, "release time");
	throwIfNotDouble(request.windowSize, "window size");
}
