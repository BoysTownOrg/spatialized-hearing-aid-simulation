#include "BrirConfig.h"
#include <gsl/gsl>
#include <algorithm>

std::string brir_config::propertyName(Property p) {
	if (p == Property::leftImpulseResponse)
		return "left impulse response";
	if (p == Property::rightImpulseResponse)
		return "right impulse response";
	if (p == Property::sampleRate)
		return "sample rate";
	return "unknown";
}

static std::vector<float> transformToFloat(std::vector<double> v) {
	std::vector<float> result;
	std::transform(
		v.begin(),
		v.end(),
		std::back_inserter(result),
		[](double x) -> float { return gsl::narrow_cast<float>(x); });
	return result;
}

BinauralRoomImpulseResponse::BinauralRoomImpulseResponse(const ConfigurationFileParser & parser) :
	_left{ transformToFloat(parser.asVector(propertyName(brir_config::Property::leftImpulseResponse))) },
	_right{ transformToFloat(parser.asVector(propertyName(brir_config::Property::leftImpulseResponse))) },
	_sampleRate{ parser.asInt(propertyName(brir_config::Property::sampleRate)) } {}

const std::vector<float> &BinauralRoomImpulseResponse::left() const {
	return _left;
}

const std::vector<float>& BinauralRoomImpulseResponse::right() const
{
	return _right;
}

int BinauralRoomImpulseResponse::sampleRate() const {
	return _sampleRate;
}
