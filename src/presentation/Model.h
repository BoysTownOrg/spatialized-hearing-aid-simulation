#pragma once

#include <common-includes/Interface.h>
#include <string>
#include <stdexcept>

class Model {
public:
	INTERFACE_OPERATIONS(Model);
	class RequestFailure : public std::runtime_error {
	public:
		explicit RequestFailure(std::string s) : std::runtime_error{ s } {}
	};
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