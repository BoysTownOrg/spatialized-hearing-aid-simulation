#include "AudioPlayerModel.h"

void AudioPlayerModel::playRequest(PlayRequest request) {
	try {
		std::stod(request.level_dB_Spl);
	}
	catch (const std::invalid_argument &) {
		throw RequestFailure{ 
			"'" + request.level_dB_Spl + "' is not a valid level..." };
	}
}
