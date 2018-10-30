#include "AudioPlayerModel.h"

AudioPlayerModel::AudioPlayerModel(std::shared_ptr<AudioPlayerFactory> factory) :
	factory{ std::move(factory) } {}

void AudioPlayerModel::playRequest(PlayRequest request) {
	throwIfNotDouble(request.level_dB_Spl, "level");
	throwIfNotDouble(request.attack_ms, "attack time");
	throwIfNotDouble(request.release_ms, "release time");
	throwIfNotPositiveInteger(request.windowSize, "window size");
	throwIfNotPositiveInteger(request.chunkSize, "chunk size");
	const auto player = factory->make(
		{ 
			request.audioFilePath,
			request.brirFilePath,
			{
				request.leftDslPrescriptionFilePath,
				request.rightDslPrescriptionFilePath,
				std::stod(request.level_dB_Spl),
				std::stod(request.attack_ms),
				std::stod(request.release_ms),
				std::stoi(request.windowSize),
				std::stoi(request.chunkSize)
			},
			{
				std::stoi(request.chunkSize)
			}
		}
	);
	player->play();
}

void AudioPlayerModel::throwIfNotDouble(std::string x, std::string identifier) {
	try {
		std::stod(x);
	}
	catch (const std::invalid_argument &) {
		throwRequestFailure(x, identifier);
	}
}

void AudioPlayerModel::throwRequestFailure(std::string x, std::string identifier)
{
	throw RequestFailure{
		"'" + x + "' is not a valid " + identifier + "." };
}

static bool onlyContainsDigits(const std::string s) {
	return s.find_first_not_of("0123456789") == std::string::npos;
}

void AudioPlayerModel::throwIfNotPositiveInteger(std::string x, std::string identifier) {
	if (!onlyContainsDigits(x))
		throwRequestFailure(x, identifier);
}
