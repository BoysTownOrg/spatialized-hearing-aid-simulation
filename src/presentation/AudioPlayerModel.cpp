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

bool onlyContainsDigits(const std::string s) {
	return s.find_first_not_of("0123456789") == std::string::npos;
}

void AudioPlayerModel::throwIfNotInteger(std::string x, std::string identifier) {
	if (!onlyContainsDigits(x))
		throw RequestFailure{
			"'" + x + "' is not a valid " + identifier + "." };
}

void AudioPlayerModel::playRequest(PlayRequest request) {
	throwIfNotDouble(request.level_dB_Spl, "level");
	throwIfNotDouble(request.attack_ms, "attack time");
	throwIfNotDouble(request.release_ms, "release time");
	throwIfNotInteger(request.windowSize, "window size");
	throwIfNotDouble(request.chunkSize, "chunk size");
}
