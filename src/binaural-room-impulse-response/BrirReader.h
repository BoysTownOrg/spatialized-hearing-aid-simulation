#pragma once

#include <common-includes/Interface.h>
#include <common-includes/RuntimeError.h>
#include <vector>

class BrirReader {
public:
	INTERFACE_OPERATIONS(BrirReader);
	struct BinauralRoomImpulseResponse {
		std::vector<float> left;
		std::vector<float> right;
		int sampleRate;
	};
	virtual BinauralRoomImpulseResponse read(std::string filePath) = 0;
	RUNTIME_ERROR(ReadError);
};

