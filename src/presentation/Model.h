#pragma once

#include <common-includes/Interface.h>
#include <string>

class Model {
public:
	INTERFACE_OPERATIONS(Model);
	struct PlayRequest {
		std::string dslPrescriptionFilePath;
		std::string audioFilePath;
		std::string brirFilePath;
	};
	virtual void playRequest(PlayRequest) = 0;
};