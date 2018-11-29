#include "PlayAudioModel.h"

PlayAudioModel::PlayAudioModel(
	std::shared_ptr<AudioDeviceFactory> deviceFactory
) :
	deviceFactory{ std::move(deviceFactory) } {}

void PlayAudioModel::playRequest(PlayRequest request) {
	AudioDevice::Parameters forDevice{};
	forDevice.framesPerBuffer = request.chunkSize;
	deviceFactory->make(forDevice);
}

void PlayAudioModel::throwIfNotDouble(std::string x, std::string identifier) {
	try {
		std::stod(x);
	}
	catch (const std::invalid_argument &) {
		throwRequestFailure(x, identifier);
	}
}

void PlayAudioModel::throwRequestFailure(std::string x, std::string identifier)
{
	throw RequestFailure{
		"'" + x + "' is not a valid " + identifier + "." };
}

static bool onlyContainsDigits(const std::string s) {
	return s.find_first_not_of("0123456789") == std::string::npos;
}

void PlayAudioModel::throwIfNotPositiveInteger(std::string x, std::string identifier) {
	if (!onlyContainsDigits(x))
		throwRequestFailure(x, identifier);
}
