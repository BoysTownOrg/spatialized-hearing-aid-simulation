#include "AudioPlayerModel.h"

void AudioPlayerModel::playRequest(PlayRequest request) {
	try {
		std::stod(request.level_dB_Spl);
	}
	catch (const std::invalid_argument &) {
		throw RequestFailure{ 
			"'" + request.level_dB_Spl + "' is not a valid level..." };
	}
	try {
		std::stod(request.attack_ms);
	}
	catch (const std::invalid_argument &) {
		throw RequestFailure{
			"'" + request.attack_ms + "' is not a valid attack time." };
	}
}
