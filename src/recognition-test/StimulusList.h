#pragma once

#include <common-includes/Interface.h>
#include <string>

class StimulusList {
public:
	INTERFACE_OPERATIONS(StimulusList);
	virtual void initialize(std::string directory) = 0;
	virtual std::string next() = 0;
	virtual bool empty() = 0;
};