#pragma once

#include <common-includes/Interface.h>
#include <string>

class Model {
public:
	INTERFACE_OPERATIONS(Model);
	struct PlayRequest {
		std::string leftDslPrescriptionFilePath;
		std::string rightDslPrescriptionFilePath;
		std::string audioFilePath;
		std::string brirFilePath;
		std::string level_dB_Spl;
		std::string attack_ms;
		std::string release_ms;
	};
	virtual void playRequest(PlayRequest) = 0;
};