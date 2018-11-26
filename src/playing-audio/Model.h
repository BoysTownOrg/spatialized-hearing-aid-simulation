#pragma once

#include <common-includes/Interface.h>
#include <common-includes/RuntimeError.h>
#include <string>
#include <stdexcept>

class Model {
public:
	RUNTIME_ERROR(RequestFailure);
	INTERFACE_OPERATIONS(Model);
	struct PlayRequest {
		std::string leftDslPrescriptionFilePath;
		std::string rightDslPrescriptionFilePath;
		std::string audioFilePath;
		std::string brirFilePath;
		std::string level_dB_Spl;
		std::string attack_ms;
		std::string release_ms;
		std::string windowSize;
		std::string chunkSize;
	};
	virtual void playRequest(PlayRequest) = 0;
};
