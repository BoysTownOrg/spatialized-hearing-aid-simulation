#pragma once

#include <common-includes/Interface.h>
#include <common-includes/RuntimeError.h>
#include <vector>

class BrirReader {
public:
	INTERFACE_OPERATIONS(BrirReader);
	RUNTIME_ERROR(ReadError);
	struct BinauralRoomImpulseResponse {
		std::vector<float> left;
		std::vector<float> right;
		int sampleRate;
	};
	virtual BinauralRoomImpulseResponse read(std::string filePath) = 0;
};

