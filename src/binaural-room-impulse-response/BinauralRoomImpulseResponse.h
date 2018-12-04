#pragma once

#ifdef BINAURAL_ROOM_IMPULSE_RESPONSE_EXPORTS
#define BINAURAL_ROOM_IMPULSE_RESPONSE_API __declspec(dllexport)
#else
#define BINAURAL_ROOM_IMPULSE_RESPONSE_API __declspec(dllimport)
#endif

#include <string>

namespace binaural_room_impulse_response {
	enum class Property {
		leftImpulseResponse,
		rightImpulseResponse,
		sampleRate
	};

	BINAURAL_ROOM_IMPULSE_RESPONSE_API std::string propertyName(Property);
}

#include <dsl-prescription/ConfigurationFileParser.h>
#include <common-includes/RuntimeError.h>

class BinauralRoomImpulseResponse {
	std::vector<float> _left;
	std::vector<float> _right;
	int _sampleRate;
public:
	RUNTIME_ERROR(InvalidResponse);
	BINAURAL_ROOM_IMPULSE_RESPONSE_API explicit BinauralRoomImpulseResponse(
		const ConfigurationFileParser &parser
	);
	BINAURAL_ROOM_IMPULSE_RESPONSE_API const std::vector<float> &left() const;
	BINAURAL_ROOM_IMPULSE_RESPONSE_API const std::vector<float> &right() const;
	BINAURAL_ROOM_IMPULSE_RESPONSE_API int sampleRate() const;
};

