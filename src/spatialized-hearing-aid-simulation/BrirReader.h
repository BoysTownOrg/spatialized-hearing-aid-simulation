#pragma once

#include <common-includes/Interface.h>
#include <common-includes/RuntimeError.h>
#include <vector>

class BrirReader {
public:
    INTERFACE_OPERATIONS(BrirReader)
	using impulse_response_type = std::vector<float>;
	struct BinauralRoomImpulseResponse {
		impulse_response_type left{ 0 };
		impulse_response_type right{ 0 };
		int sampleRate;
	};
	virtual BinauralRoomImpulseResponse read(std::string filePath) = 0;
    RUNTIME_ERROR(ReadFailure)
};

