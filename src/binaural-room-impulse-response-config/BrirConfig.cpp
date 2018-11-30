#include "BrirConfig.h"

std::string brir_config::propertyName(Property p) {
	if (p == Property::leftImpulseResponse)
		return "left impulse response";
	if (p == Property::rightImpulseResponse)
		return "right impulse response";
	if (p == Property::sampleRate)
		return "sample rate";
	return "unknown";
}

BinauralRoomImpulseResponse::BinauralRoomImpulseResponse(const ConfigurationFileParser & parser) :
	_sampleRate{ parser.asInt(propertyName(brir_config::Property::sampleRate)) } {}

int BinauralRoomImpulseResponse::sampleRate() const {
	return _sampleRate;
}
