#pragma once

#include <common-includes/Interface.h>
#include <common-includes/RuntimeError.h>
#include <string>

class StimulusList {
public:
	INTERFACE_OPERATIONS(StimulusList);
	virtual void initialize(std::string directory) = 0;
	RUNTIME_ERROR(InitializationFailure);
	virtual std::string next() = 0;
	virtual bool empty() = 0;
};