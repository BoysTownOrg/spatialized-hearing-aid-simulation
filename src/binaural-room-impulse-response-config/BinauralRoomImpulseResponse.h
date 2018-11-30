#pragma once

#ifdef BRIR_CONFIG_EXPORTS
#define BRIR_CONFIG_API __declspec(dllexport)
#else
#define BRIR_CONFIG_API __declspec(dllimport)
#endif

#include <string>

namespace brir_config {
	enum class Property {
		leftImpulseResponse,
		rightImpulseResponse,
		sampleRate
	};

	BRIR_CONFIG_API std::string propertyName(Property);
}

#include <dsl-prescription/ConfigurationFileParser.h>

class BinauralRoomImpulseResponse {
	std::vector<float> _left;
	std::vector<float> _right;
	int _sampleRate;
public:
	BRIR_CONFIG_API explicit BinauralRoomImpulseResponse(
		const ConfigurationFileParser &parser
	);
	BRIR_CONFIG_API const std::vector<float> &left() const;
	BRIR_CONFIG_API const std::vector<float> &right() const;
	BRIR_CONFIG_API int sampleRate() const;
};

