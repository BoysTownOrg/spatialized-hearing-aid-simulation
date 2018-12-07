#pragma once

#ifdef BINAURAL_ROOM_IMPULSE_RESPONSE_EXPORTS
#define BINAURAL_ROOM_IMPULSE_RESPONSE_API __declspec(dllexport)
#else
#define BINAURAL_ROOM_IMPULSE_RESPONSE_API __declspec(dllimport)
#endif

#include <vector>

struct BinauralRoomImpulseResponse {
	std::vector<float> left;
	std::vector<float> right;
	int sampleRate;
};

